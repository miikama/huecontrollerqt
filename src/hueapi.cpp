#include "hueapi.h"
#include <src/light.h>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <iostream>


#define DEBUG 0

HueApi::HueApi(QWidget* parent) :
    QObject(parent)
{
    // Load bridge config on initialization
    auto success = load_bridge_config();
    if(!success)
        qDebug() << "No previous config available";

    // small helper
    auto confirm_reply = [&](QNetworkReply* reply){        
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "returning due failed query, which has data: " << reply->readAll();
            return false;
        }
        return true;
    };

    // connect find_bridge reply manager
    m_bridge_api_manager = new QNetworkAccessManager(this);
    QObject::connect(m_bridge_api_manager, &QNetworkAccessManager::finished,
                     [&](QNetworkReply* reply){

        if(confirm_reply(reply))
            this->on_bridge_search_complete(reply);
    });

    // connect authenticate bridge reply manager
    m_authenticate_bridge_manager = new QNetworkAccessManager(this);
    QObject::connect(m_authenticate_bridge_manager, &QNetworkAccessManager::finished,
                     [&](QNetworkReply* reply){

        if(confirm_reply(reply))
            this->on_authorized(reply);
    });

    // connect bridge light api reply manager
    m_query_lights_manager = new QNetworkAccessManager(this);
    QObject::connect(m_query_lights_manager, &QNetworkAccessManager::finished,
                     [&](QNetworkReply* reply){

        if(confirm_reply(reply))
            this->on_light_query_response(reply);
    });


}

void HueApi::on_bridge_search_complete(QNetworkReply* reply)
{
    QString reply_string = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply_string.toUtf8());

    qDebug() << "json response: " << jsonResponse;
    qDebug() << "document is valid: " << !jsonResponse.isNull() << "response is array: " << jsonResponse.isArray() << ", is object: " << jsonResponse.isObject();

    auto jsonObject = jsonResponse.array().first().toObject();

    qDebug() << "parsed ip address: " << jsonObject["internalipaddress"].toString();
    auto ip_string = jsonObject["internalipaddress"].toString();

    QUrl hue_bridge_ip = QUrl(ip_string);

    // TODO: emit a signal from these and move the messageboxes to gui code
    if(hue_bridge_ip.isValid())
    {
        qDebug() << "Got a valid bridge api: " <<  hue_bridge_ip.isValid();        
        emit validBridgeFound(hue_bridge_ip);
    }
    else
    {
        emit noBridgeFound();
        return;
    }

    // update bridge ip config
    m_bridge_config.bridge_ip = hue_bridge_ip;
    store_bridge_config();
}

void HueApi::on_authorized(QNetworkReply *reply) {

    QString response_data = reply->readAll();
    qDebug() << "Got reply for on_authorized: " << response_data;

    /*   parse json response
    *
    *   ERROR: Response due to no having pressed button on hue bridge.
    *       [{"error":{
    *           "type":101,
    *           "address":"",
    *           "description":"link button not pressed"
    *        }}]
    *
    *
    *   SUCCESS: Button is pressed and we have a new user that can manage the lights
    *
    *       [{"success":{
    *               "username":"ankey"
    *         }}]
    */

    QJsonDocument jsonDoc = QJsonDocument::fromJson(response_data.toUtf8());
    QJsonObject rootObj = jsonDoc.object();
    auto response = jsonDoc.array().first().toObject();

    // First check for error responses
    QJsonObject error_response = response.find("error")->toObject();

    if(!error_response.empty())
    {
        qDebug() << "got error: " << error_response;
        QString error_message = "Authentication failed: " + error_response.find("description")->toString();
        emit authenticationFailed(error_message);
        return;
    }


    // get the username from the response
    auto success_response = response.find("success")->toObject();
    qDebug() << "got success: " << success_response;
    QString username = success_response.find("username")->toString();

    if(!username.isEmpty())
    {
        qDebug() << "got username: " << username;
        m_bridge_config.username = username;
        store_bridge_config();
        emit authenticationSucceeded(username);
    }

}

void HueApi::on_light_query_response(QNetworkReply *reply) {
    /*
     *  Handles a response of the light status, in the form
     *
     *  https://developers.meethue.com/develop/hue-api/lights-api/
     *
     *      {
                "1": {
                        "state": {
                            "on": false,
                            "bri": 1,
                            "hue": 33761,
                            "sat": 254,
                            "effect": "none",
                            "xy": [
                                0.3171,
                                0.3366
                            ],
                            "ct": 159,
                            "alert": "none",
                            "colormode": "xy",
                            "mode": "homeautomation",
                            "reachable": true
                        },
                        "swupdate": {
                            "state": "noupdates",
                            "lastinstall": "2018-01-02T19:24:20"
                        },
                        "capabilities": {
                        },
                        "type": "Extended color light",
                        "name": "Hue color lamp 7",
                        "modelid": "LCT007",
                        "manufacturername": "Philips",
                        "productname": "Hue color lamp",
                        "uniqueid": "0HueApi0:17:88:01:00:bd:c7:b9-0b",
                        "swversion": "5.105.0.21169"
                    }
            }
     *
     */

    QString response_data = reply->readAll();


    QJsonDocument jsonDoc = QJsonDocument::fromJson(response_data.toUtf8());
    QJsonObject rootObj = jsonDoc.object();

#if DEBUG
    qDebug() << "Got reply for on_light_query_response";
    // qDebug() << "And we have a response for lights query: " << rootObj;
#endif

    // Initialize a vector for lights
    m_lights = {};

    auto keys = rootObj.keys();
    qDebug() << "Got me keys: " << keys;

    for(auto key : keys)
    {
        QJsonObject entry = rootObj.find(key)->toObject();
        // lets say that each valid light object requires a state
        // to be present to be considered valid light

        if(entry.find("state")->toObject().isEmpty())
        {
            continue;
        }

        Light::LightData light_data = {};

        // parse state
        auto state = entry.find("state")->toObject();
        light_data.on = state.find("on")->toBool();
        light_data.bri = state.find("bri")->toInt();
        light_data.hue = state.find("hue")->toInt();
        light_data.sat = state.find("sat")->toInt();

        auto xy = state.find("xy")->toArray();
        if(xy.empty()) {
            light_data.xy = {0,0};
        }
        else {
            light_data.xy = { xy.first().toDouble(), xy.last().toDouble() };
        }

        // Todo: parse more that xy mode
        light_data.colormode = state.find("colormode")->toString() == "xy" ? Light::LampColorMode::XY : Light::LampColorMode::UNKNOWN;
        light_data.reachable = state.find("reachable")->toBool();

        // parse general information
        light_data.type = entry.find("type")->toString();
        light_data.name = entry.find("name")->toString();
        light_data.uniqueid = entry.find("uniqueid")->toString();

        Light light(std::move(light_data), this);

        m_lights.append(std::move(light));
    }

    qDebug() << "Found total of " << m_lights.length() << " lights.";

    emit lightsLoaded();

}

void HueApi::authorize_bridge() {

    if(!m_authenticate_bridge_manager)
        return;

    if(!m_bridge_config.bridge_ip.isValid())
        return;

    // set the authentication data
    QJsonObject auth_data;
    auth_data.insert("devicetype", "HueControllerQT");

    // build the query
    QUrl endpoint =  api_endpoint();
    QNetworkRequest auth_request(endpoint);
    auth_request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // send it
    qDebug() << "sending query to endpoint: " << auth_request.url() << "json data: " << auth_data;
    m_authenticate_bridge_manager->post(auth_request, QJsonDocument(auth_data).toJson());
}


void HueApi::find_bridge()
{
    if(!m_bridge_api_manager) {
        qDebug() << "No manager to connect with";
        return;
    }

    auto api_address = QString("https://www.meethue.com/api/nupnp");
    auto url = QUrl(api_address);

    auto request = QNetworkRequest(url);
    m_bridge_api_manager->get(request);
}


void HueApi::load_light_info() {

    if(!m_query_lights_manager)
        return;

    if(!bridge_found() || !bridge_authenticated())
        return;

    qDebug() << "at load light ind";
    auto request = QNetworkRequest(api_lights_endpoint());
    m_query_lights_manager->get(request);
}

bool HueApi::load_bridge_config() {

    // get the save location
    auto system_config_directory = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    auto config_directory = QDir::cleanPath(system_config_directory + QDir::separator() + m_config_dir);
    auto config_full_path = QDir::cleanPath(config_directory +  QDir::separator() + m_config_file_name);


    qDebug() << "Loading bridge config from file: " << config_full_path;
    QFile config_file(config_full_path);

    // if it does not exist -> quit
    if(!config_file.exists())
        return false;

    // load it
    config_file.open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(config_file.readAll(), &jsonError);
    config_file.close();

    // parse json
    QJsonObject config_root = jsonDoc.object();

    qDebug() << "Got json config: " << config_root;

    auto username = config_root.find("username").value().toString();
    auto ip_string = config_root.find("bridge_ip").value().toString();
    QUrl bridge_ip(ip_string);

    // if ip is valid update the current configuration
    if(!bridge_ip.isValid())
        return false;


    m_bridge_config.bridge_ip = bridge_ip;
    m_bridge_config.username = username;
    return true;



}

void HueApi::store_bridge_config() {

    // make a json from config
    QJsonObject config;
    config.insert("username", m_bridge_config.username);
    config.insert("bridge_ip", m_bridge_config.bridge_ip.toString());

    QJsonDocument jsonDoc;
    jsonDoc.setObject(config);

    // get the save location
    auto system_config_directory = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    auto config_directory = QDir::cleanPath(system_config_directory + QDir::separator() + m_config_dir);
    auto config_full_path = QDir::cleanPath(config_directory +  QDir::separator() + m_config_file_name);

    // make the directory if it does not exist
    if(!QDir(config_directory).exists())
        QDir().mkdir(config_directory);

    // save json config
    QFile config_file(config_full_path);
    config_file.open(QIODevice::WriteOnly | QIODevice::Text);
    config_file.write(jsonDoc.toJson());
    config_file.close();


    qDebug() << "writable config locations: " << QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}



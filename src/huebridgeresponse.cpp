#include "huebridgeresponse.h"

#include <optional>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define DEBUG 0

HueBridgeResponse::HueBridgeResponse(const QJsonObject& response)
{
    // assume we got this
    m_success = true;

    // the reponse wasn't what we excpected
    if(response.empty())
    {
        m_success = false;
        m_error_message = "Got invalid response.";
        return;
    }

    // First check for error responses
    QJsonObject error_response = response.find("error")->toObject();

    if(!error_response.empty())
    {
        m_success = false;
        m_error_message = error_response.find("description")->toString();
        return;
    }

    // get the success response
    auto success_response = response.find("success")->toObject();
    if(success_response.empty()) {
        m_success = false;
        m_error_message = "No 'success' field in the response";
        return;
    }    

    auto light_keys = success_response.keys();

    // I guess bridge can return more than one key?
    if(light_keys.length() > 1)
        qDebug() << "Got more than one key as response to query to bridge!";

    if(!parse_light_id(light_keys.first()))
        return;

    // parse possible state parameters
    parse_on_state(success_response);
    parse_brightness(success_response);

}

HueBridgeResponse HueBridgeResponse::from_json(const QString& response_string) {

    QJsonDocument jsonDoc = QJsonDocument::fromJson(response_string.toUtf8());

    auto response = jsonDoc.array().first().toObject();    

    return HueBridgeResponse(response);
}

QString HueBridgeResponse::id_from_key(const QString& key)
{
    auto paths = key.split('/');

    if(paths.length() < 3)
        return "";

    // Sometimes the response contains starting slash, sometimes not.
    if( key[0] == '/' )
        return paths[2];
    else
        return paths[1];

}

bool HueBridgeResponse::parse_light_id(const QString& light_key)
{
    auto light_id = id_from_key(light_key);
    if(light_id.length() == 0)
    {
        m_success = false;
        m_error_message = "No valid light id found in the response";
        return false;
    }
    m_light_id = light_id;
    return true;
}

void HueBridgeResponse::parse_on_state(QJsonObject state)
{
    auto key = state.keys().first();
    auto paths = key.split('/');

    if(paths.length() < 4)
        return;

    if(paths.back() == "on") {
        m_is_on = state[key].toBool();
        qDebug() << "Got on state: " << m_is_on.value();
    }

}

void HueBridgeResponse::parse_brightness(QJsonObject state)
{
    auto key = state.keys().first();
    auto paths = key.split('/');

    if(paths.length() < 4)
        return;

    if(paths.back() == "bri") {
        m_new_brighness = std::optional<int> { state[key].toInt() };
        qDebug() << "Got brightness: " << m_new_brighness.value();
    }
}



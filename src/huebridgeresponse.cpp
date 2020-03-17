#include "huebridgeresponse.h"

#include <optional>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define DEBUG 0

HueBridgeResponse::HueBridgeResponse(const QJsonObject& response)
{
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
    else {
        m_success = true;
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

std::optional<HueBridgeResponse> HueBridgeResponse::from_json(const QString response_string) {

    QJsonDocument jsonDoc = QJsonDocument::fromJson(response_string.toUtf8());

    auto response = jsonDoc.array().first().toObject();

    // the reponse wasn't what we excpected
    if(response.empty())
    {
        return {};
    }

    return std::optional<HueBridgeResponse> { HueBridgeResponse(response) };
}

QString HueBridgeResponse::id_from_key(QString key)
{
    auto paths = key.split('/');

    if(paths.length() < 2)
        return "";

    return paths[1];
}

bool HueBridgeResponse::parse_light_id(QString light_key)
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


#ifndef HUEBRIDGERESPONSE_H
#define HUEBRIDGERESPONSE_H

#include <optional>
#include <QJsonDocument>

class HueBridgeResponse
{
public:

    /**
     *  Hue brige sends a response for all queries sent to the api.
     *  This contains helpers to handle that response.
     *
     *
     *  A response to PUT request to the api_set_light_state_endpoint()
     *
     * results in either an error message:
     *
     * [
     *   {"error":
     *      {
     *          "type":3,
     *          "address":"/lights/00:17:88:01:04:56:3b:bd-0b/state",
     *          "description":"resource, /lights/00:17:88:01:04:56:3b:bd-0b/state, not available"
     *      }
     *   }
     * ]
     *
     * of a successfull response
     *
     * [
     *   {"success":
     *      {
     *          "/lights/1/name":"Bedroom Light",
     *          "/lights/1/state/bri":214},
     *          ...
     *      }
     *   }
     * ]
     *
     *
     */
    HueBridgeResponse(const QJsonObject& response);

    /**
     *   A bridge object or none if response could not be parsed.
     */
    static HueBridgeResponse from_json(const QString response);

    /**
     * whether the operation this response was sent for was successfull
     */
    bool success() const { return m_success; }

    /**
     *  If the response fails, you can get the error message from here.
     */
    QString errorMessage() const { return  m_error_message; }

    /**
     *  The light id in the response from the bridge if there was one in the response.
     */
    QString light_id() const { return m_light_id; }

    /**
     *  Different fiels that might be present in the response
     */
    bool isOnResponse() const { return m_is_on.has_value(); }
    int on() const { return m_is_on.value_or(false); }
    bool isBrightnessResponse() const { return m_new_brighness.has_value(); }
    int brigthness() const { return m_new_brighness.value_or(-1); }


private:

    /**
     * parse the bridge response key from 'lights/1/state/bri' -> '1'
     */
    QString id_from_key(QString key);
    bool parse_light_id(QString light_key);

    /**
     * parse possible states in the response
     */
    void parse_on_state(QJsonObject response);
    void parse_brightness(QJsonObject response);

    bool m_success = false;
    QString m_error_message = "";
    QString m_light_id = "";

    std::optional<int> m_new_brighness {};
    std::optional<bool> m_is_on {};
};

#endif // HUEBRIDGERESPONSE_H

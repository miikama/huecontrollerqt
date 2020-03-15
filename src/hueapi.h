#ifndef HUEAPI_H
#define HUEAPI_H

#include <memory>
#include <QtNetwork/QNetworkAccessManager>
#include <src/light.h>

class HueApi
{
public:

    struct ApiConfig {
        QString username = {};
        QUrl bridge_ip = {};
    };

    /*
     *  This class should always be deleted when the parent dies.
     *  Otherwise we will be left with null pointers to the NetworkAccessManager
     *  which is linked to the parent QWidget.
     */
    HueApi(QWidget* parent);

    /*
     * Calls api to initiate upnp to find Hue bridge on the local network
     * Then tries to authenticate the user with the bridge
     */
    void find_bridge();

    /*
     *  Hue bridge requires physical authentication by pressing button
     *  on the first connection, initialize that.
    */
    void authorize_bridge();

    /*
     *  Whether we have found the bridge on this execution or on
     *  previous runs of the program and have the address stored in
     *  the congiguration.
    */
    bool bridge_found() { return m_bridge_config.bridge_ip.isValid(); }

    /*
     *  Whether we have found the bridge and authenticated (pressed the auth
     *  button on the actual physical bridge) on this execution or on
     *  previous runs of the program and have the address stored in
     *  the congiguration.
    */
    bool bridge_authenticated() { return !m_bridge_config.username.isEmpty(); }

    /*
     *  Load light info, updates m_lights
    */
    void load_light_info();

    /*
     *  Get the light objects from the bridge
    */
    QVector<Light>& lights() { return m_lights; }

private:

     void on_bridge_search_complete(QNetworkReply* reply);
     void on_authorized(QNetworkReply* reply);
     void on_light_query_response(QNetworkReply* reply);

     bool load_bridge_config();
     void store_bridge_config();

     QUrl api_endpoint() { return "http://" + m_bridge_config.bridge_ip.toString() + "/api"; }
     QUrl api_lights_endpoint() { return "http://" + m_bridge_config.bridge_ip.toString() + "/api/" + m_bridge_config.username + "/lights"; }

     ApiConfig m_bridge_config = {};

     QWidget* m_parent;

     // Feels stupid keeping own members for each query type, but lets do
     // it like this for now...
     QNetworkAccessManager* m_bridge_api_manager;
     QNetworkAccessManager* m_authenticate_bridge_manager;
     QNetworkAccessManager* m_query_lights_manager;


     // Configuration file will be loaded from system StandardLocation
     // QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
     // The configuration file name
     QString m_config_dir = "HueControllerQT";
     QString m_config_file_name = "hue_app_config.json";

    QVector<Light> m_lights;

};

#endif // HUEAPI_H

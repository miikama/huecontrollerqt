#ifndef LIGHT_H
#define LIGHT_H

#include <QVector>
#include <QPair>

class HueApi;

class Light
{    

public:

    enum LampColorMode {
        UNKNOWN,
        XY
    };

    struct Room {
        QVector<Light> lights = {};
    };

    struct Rooms {
        QVector<Room> rooms = {};
    };

    struct LightData {
        QString bridge_id = "";
        bool on = false;
        int bri = 0;
        int hue = 0;
        int sat = 0;
        QPair<double, double> xy = {0,0};
        int ct = 0;
        LampColorMode colormode = LampColorMode::UNKNOWN;
        bool reachable = false;
        QString type = "";
        QString name = "";
        QString uniqueid = "";
    };


    /*
     *  Create a light object that is used to interface with the light
     */
    Light(LightData, HueApi* api);

    /*
     *  Use this for setting max values for sliders etc.
     */
    static int maxBrightness() { return 255; }

    /**
      *  toggle on/off
      */
    void toggle();

    /*
     * set light on/off
     */
    void setOn(bool state);

    /*
     * set light brightness
     */
    void setBrightness(int brightness);

    /*
     * Access functions to the data for this light.
     *
     * the bridgeID is used by the bridge as the main entrypoint
     * id for lights, not uniqueid
     */
    QString getBridgeID() const { return m_data.bridge_id; }
    bool getOn() const { return m_data.on; }
    int getBrightness() const { return m_data.bri; }
    int getHue() const { return m_data.hue; }
    int getSaturation() const { return m_data.sat; }
    LampColorMode getColormode() const { return m_data.colormode; }
    QString getType() const { return m_data.type; }
    QString getName() const { return m_data.name; }
    bool getReachable() const { return m_data.reachable; }
    QString getUniqueid() const { return m_data.uniqueid; }
    int getColorTemperature() const { return m_data.ct; }
    QPair<double, double> getXy() const { return m_data.xy; }

private:

    LightData m_data;
    HueApi* m_hue_api = nullptr;

};

#endif // LIGHT_H

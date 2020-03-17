#include "light.h"
#include <src/hueapi.h>

Light::Light(Light::LightData light_data, HueApi* api)
{
    m_hue_api = api;
    m_data = std::move(light_data);
}

void Light::toggle()
{
    if(!m_hue_api)
        return;

    // TODO do not set the value here but wait for success from api
    m_data.on = !m_data.on;
    m_hue_api->setLightOn(*this, m_data.on);
}

void Light::setOn(bool state)
{
    m_data.on = state;
}

void Light::setBrightness(int brightness)
{
    if(!m_hue_api)
        return;

    brightness = std::max(0, brightness);
    brightness = std::min(brightness, maxBrightness());
    // TODO do not set the value here but wait for success from api
    m_data.bri = brightness;
    m_hue_api->setLightBrightness(*this, brightness);
}

#include "light.h"
#include <src/hueapi.h>

Light::Light(Light::LightData light_data, HueApi* api)
{
    m_hue_api = api;
    m_data = std::move(light_data);
}

void Light::setOn(bool state)
{
    m_data.on = state;
}

void Light::setBrightness(int brightness)
{
    brightness = std::max(0, brightness);
    brightness = std::min(brightness, maxBrightness());
    m_data.bri = brightness;
}

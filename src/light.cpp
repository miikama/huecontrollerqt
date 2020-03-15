#include "light.h"
#include <src/hueapi.h>

Light::Light(Light::LightData light_data, HueApi* api)
{
    m_api = api;
    m_data = std::move(light_data);
}

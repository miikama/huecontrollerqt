#include "lightwidget.h"
#include <src/hueapi.h>
#include <QSlider>
#include <QLabel>
#include <QSizePolicy>
#include <QPushButton>
#include <QGridLayout>
#include <QDebug>

const std::string LabelClassName = "LightWidgetLabel";
const std::string GridClassName = "LightWidgetGrid";
const std::string ToggleClassName = "LightWidgetToggle";
const std::string SliderClassName = "LightWidgetSlider";

LightWidget::LightWidget(QWidget *parent, Light& light, HueApi& api)
    : QWidget (parent)
{
    m_light = &light;
    m_hue_api = &api;
    this->build_widget();
}

void LightWidget::onOnOffToggleChange(bool checked)
{
    qDebug() << "onOnOffToggleChange, checked: " << checked;
    if(m_light && m_hue_api)
        m_hue_api->setLightOn(*m_light, !m_light->getOn());
}

void LightWidget::onSliderMoved()
{
    qDebug() << "onSliderMoved";
    if(m_light && m_hue_api)
        m_hue_api->setLightBrightness(*m_light, m_slider->value());
}

void LightWidget::build_widget()
{
    if(!m_light)
        return;

    setObjectName("LightWidget");

    m_inner_layout = new QGridLayout(this);
    m_inner_layout->setObjectName(QString::fromStdString(GridClassName));
    m_inner_layout->setSpacing(6);

    // name of the light
    m_label = new QLabel(this);
    m_label->setText(m_light->getName());
    m_label->setObjectName(QString::fromStdString(LabelClassName));
    m_inner_layout->addWidget(m_label, 0, 0, 1, 4);


    // switch to toggle light on/off
    m_on_toggle_button = new QPushButton(this);
    m_on_toggle_button->setText(m_light->getOn() ?  "On" : "Off");
    m_on_toggle_button->setCheckable(true);
    m_on_toggle_button->setChecked(m_light->getOn());
    m_on_toggle_button->setObjectName(QString::fromStdString(ToggleClassName));

    m_inner_layout->addWidget(m_on_toggle_button, 0, 4, 1, 1);
    QObject::connect(m_on_toggle_button, &QPushButton::clicked, [this](bool toggled){ this->onOnOffToggleChange(toggled); });

    // lamp has known brightness
    switch (m_light->getColormode()) {
    case Light::LampColorMode::XY:
    case Light::LampColorMode::CT:      // intentional fallthrough
    {
        m_slider = new QSlider(this);
        m_slider->setOrientation(Qt::Orientation::Horizontal);
        m_slider->setMaximum(Light::maxBrightness());
        m_slider->setValue(m_light->getBrightness());
        m_slider->setTickInterval(1);
        m_slider->setEnabled(m_light->getOn());
        m_slider->setObjectName(QString::fromStdString(SliderClassName));
        // do not emit events during drag
        m_slider->setTracking(false);
        QObject::connect(m_slider, &QSlider::valueChanged, [this](){ this->onSliderMoved(); });

        m_inner_layout->addWidget(m_slider, 1, 0, 1, 5);
        break;
    }
    case Light::LampColorMode::UNKNOWN:
        break;
    }


}

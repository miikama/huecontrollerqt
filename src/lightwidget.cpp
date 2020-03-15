#include "lightwidget.h"
#include <src/hueapi.h>
#include <QSlider>
#include <QLabel>
#include <QSizePolicy>
#include <QPushButton>
#include <QGridLayout>

LightWidget::LightWidget(QWidget *parent, Light* light)
    : QWidget (parent)
{
    m_light = light;
    this->build_widget();
}

void LightWidget::build_widget()
{
    if(!m_light)
        return;

    m_inner_layout = new QGridLayout(this);

    // name of the light
    m_label = new QLabel(this);
    m_label->setText(m_light->getName());
    m_inner_layout->addWidget(m_label, 0, 0, 1, 4);

    // switch to toggle light on/off
    m_on_toggle_button = new QPushButton(this);
    m_on_toggle_button->setText("On/Off");
    m_inner_layout->addWidget(m_on_toggle_button, 0, 4, 1, 1);

    // lamp has kwown brightness
    if( m_light->getColormode() == Light::LampColorMode::XY)
    {
        m_slider = new QSlider(this);
        m_slider->setOrientation(Qt::Orientation::Horizontal);
        m_slider->setMaximum(m_light->maxBrightness());
        m_slider->setValue(m_light->getBrightness());

        m_inner_layout->addWidget(m_slider, 1, 0, 1, 5);
    }

}

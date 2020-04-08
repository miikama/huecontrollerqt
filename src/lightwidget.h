#ifndef LIGHTWIDGET_H
#define LIGHTWIDGET_H

#include <QWidget>
#include <src/hueapi.h>
#include <src/light.h>

class QPushButton;
class QGridLayout;
class QLabel;
class QSlider;

class LightWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LightWidget(QWidget *parent, Light& light, HueApi& api);

signals:

public slots:

    void onOnOffToggleChange(bool checked);
    void onSliderMoved();

private:

    void build_widget();

    Light* m_light;
    HueApi* m_hue_api;

    QGridLayout* m_inner_layout = nullptr;
    QSlider* m_slider = nullptr;
    QLabel* m_label = nullptr;
    QPushButton* m_on_toggle_button = nullptr;
};

#endif // LIGHTWIDGET_H

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
    explicit LightWidget(QWidget *parent, Light* light);

signals:

public slots:


private:

    void build_widget();

    Light* m_light;

    QGridLayout* m_inner_layout;
    QSlider* m_slider;
    QLabel* m_label;
    QPushButton* m_on_toggle_button;
};

#endif // LIGHTWIDGET_H

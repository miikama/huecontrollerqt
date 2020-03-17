#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <src/lightwidget.h>
#include <src/hueapi.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();    

    void on_actionFind_Hue_bridge_triggered();

    void on_actionAuthenticate_triggered();

    void draw_lights();

    void on_actionRefresh_triggered();

private:

    /*
     *  Set up responses to the API events.
    */
    void setupConnections();

    void clearLightWidgets();

    Ui::MainWindow *ui;    
    HueApi* m_hue_api;

    QVector<LightWidget*> m_light_widgets;

};

#endif // MAINWINDOW_H

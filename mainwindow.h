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

    void on_testingButton_clicked();

private:

    void draw_lights();

    Ui::MainWindow *ui;    
    std::unique_ptr<HueApi> m_hue_api;

    QVector<LightWidget*> m_light_widgets;

};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
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

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;    
    std::unique_ptr<HueApi> m_hue_api;

};

#endif // MAINWINDOW_H

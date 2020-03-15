#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <src/hueapi.h>
#include <src/lightwidget.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_hue_api = std::unique_ptr<HueApi> {new HueApi(this)};


    if(!m_hue_api->bridge_found())
    {
        // if we have not previously found the bridge, try to look for it
        // If we then find the bridge the api will try to authenticate it
        qDebug() << "No previous bridge available";
        m_hue_api->find_bridge();
    }
    else if(!m_hue_api->bridge_authenticated())
    {
        // if we have bridge ip but are not authenticated,try to authenticate
        qDebug() << "No previous authentication available";
        m_hue_api->authorize_bridge();
    }

    // load lights
    if(m_hue_api->bridge_authenticated())
    {
        m_hue_api->load_light_info();
    }

    // add the light widgets
    draw_lights();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionFind_Hue_bridge_triggered()
{
    if(!m_hue_api)
    {
        qDebug() << "No hue api available";
        return;
    }
    m_hue_api->find_bridge();
}

void MainWindow::on_actionAuthenticate_triggered()
{
    if(!m_hue_api)
    {
        qDebug() << "No hue api available";
        return;
    }
    m_hue_api->authorize_bridge();
}

void MainWindow::draw_lights()
{
    for( auto& light : m_hue_api->lights())
    {
        auto light_widget = new LightWidget(this, &light);
        m_light_widgets.append(light_widget);

        this->ui->LightWidgetLayout->addWidget(light_widget);
    }


}

void MainWindow::on_testingButton_clicked()
{
    if(!m_hue_api)
    {
        qDebug() << "No hue api available";
        return;
    }

    qDebug() << "testing";
    draw_lights();

}

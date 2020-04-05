#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <src/hueapi.h>
#include <src/lightwidget.h>
#include <src/light.h>
#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_hue_api = new HueApi(this);

    // Connect all ui events
    setupConnections();

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

    // load light info
    if(m_hue_api->bridge_authenticated())
    {
        m_hue_api->load_light_info();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections() {

    // add the light widgets
    connect(m_hue_api, SIGNAL(lightsUpdated()) , this, SLOT(draw_lights()));

    connect(m_hue_api, &HueApi::validBridgeFound, [this](QUrl bridge_ip){
        QString found_text = "Found Hue bridge at ip." + bridge_ip.toString();
        QMessageBox::information(this, tr("HueControllerapp"), tr(found_text.toUtf8()));
    } );

    connect(m_hue_api, &HueApi::noBridgeFound, [this](){
        QMessageBox::warning(this, tr("HueControllerapp"), tr("No Hue bridge found."));
    } );

    connect(m_hue_api, &HueApi::authenticationFailed, [this](QString error_message){
        QMessageBox::warning(this, tr("HueControllerapp"), tr(error_message.toUtf8()));
    } );

    connect(m_hue_api, &HueApi::authenticationSucceeded, [this](QString username){
        QString success_message = "Succesfully autheticated with username: " + username;
        QMessageBox::information(this, tr("HueControllerapp"), tr(success_message.toUtf8()));
    } );

    connect(m_hue_api, &HueApi::lightUpdateFailed, [this](QString error_message){
        QMessageBox::warning(this, tr("HueControllerapp"), tr(error_message.toUtf8()));
    } );



}

void MainWindow::clearLightWidgets()
{
    for(auto& light : m_light_widgets)
    {
        delete light;
    }
    m_light_widgets.clear();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionRefresh_triggered()
{
    if(!m_hue_api)
        return;
    m_hue_api->load_light_info();
}


void MainWindow::on_actionFind_Hue_bridge_triggered()
{
    if(!m_hue_api)
    {
        return;
    }
    m_hue_api->find_bridge();
}

void MainWindow::on_actionAuthenticate_triggered()
{
    if(!m_hue_api)
    {
        return;
    }
    m_hue_api->authorize_bridge();
}

void MainWindow::draw_lights()
{
    // start by clearing out lights
    clearLightWidgets();

    // and the initialize new light objects

    for( auto& light : m_hue_api->lights())
    {
        auto light_widget = new LightWidget(this, &light);
        m_light_widgets.append(light_widget);

        this->ui->LightWidgetLayout->addWidget(light_widget);
    }
}


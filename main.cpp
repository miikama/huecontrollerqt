#include <src/mainwindow.h>
#include <QApplication>
#include <QIcon>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;   

    // set app icon
    QIcon icon(":/src/graphics/logo.ico");
    if(icon.isNull())
        qDebug() << "Did not find app icon.";
    else
        w.setWindowIcon(icon);

    // Set app name
    QCoreApplication::setApplicationName("HueControllerQT");
    w.setWindowTitle("HueControllerQT");

    // Set initial app size
    w.resize(370, -1);

    // Read styles
    QFile styles(":/src/appstyles.qss");
    styles.open(QFile::ReadOnly);
    if(!styles.isOpen())
        qDebug() << "Cannot open stylesheet";
    else
        a.setStyleSheet(styles.readAll());

    // Show window
    w.show();    

    return a.exec();
}

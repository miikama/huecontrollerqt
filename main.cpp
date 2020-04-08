#include <src/mainwindow.h>
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;   

    QIcon icon("src/graphics/logo.ico");
    qDebug() << "got icon: " << !icon.isNull();
    w.setWindowIcon(icon);

    w.show();    

    return a.exec();
}

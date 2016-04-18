#include "mainwindow.h"
#include <QApplication>
#include<QTranslator>
#include<QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    QString fileName="/home/zero/text_eater/text_eater_"+QLocale::system().name();
    if(translator.load(fileName))
        a.installTranslator(&translator);
    MainWindow w(argc > 1 ? argv[1] : "");
    w.show();

    return a.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include<fmt/color.h>
#include<fmt/core.h>

int main(int argc, char *argv[])
{
    fmt::print(fmt::emphasis::bold|fmt::fg(fmt::color::yellow),"fmt installed through conan!\n");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

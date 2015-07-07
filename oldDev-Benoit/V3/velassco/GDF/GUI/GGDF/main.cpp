#include "guigdf.h"
#include <QApplication>

#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GuiGDF w;
    w.show();

    return a.exec();


      return 0;
}

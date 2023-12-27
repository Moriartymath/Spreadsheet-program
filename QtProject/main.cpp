#include "mytable.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyTable w;
    w.show();
    return a.exec();
}

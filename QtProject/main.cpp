#include "mytable.h"
#include "tableoptions.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyTable w;
    w.show();

    TableOptions options;

    options.show();

    return a.exec();
}

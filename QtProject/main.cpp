#include "mytable.h"
#include "tableoptions.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyTable w;
    w.show();

    TableOptions opt_table{&w};

    QObject::connect(&w,SIGNAL(EmitOpenTableOptionsDialog()),&opt_table,SLOT(SlotOpenTableOptionsDialog()));

    return a.exec();
}

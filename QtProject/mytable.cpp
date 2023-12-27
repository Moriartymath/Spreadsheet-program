#include "mytable.h"
#include "ui_mytable.h"

MyTable::MyTable(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyTable)
{
    ui->setupUi(this);
}

MyTable::~MyTable()
{
    delete ui;
}


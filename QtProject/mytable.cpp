#include "mytable.h"
#include "ui_mytable.h"
#include "QTableWidget"

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

void MyTable::on_actionQuit_triggered()
{
    this->close();
}


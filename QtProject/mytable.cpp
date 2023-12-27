#include "mytable.h"
#include "ui_mytable.h"
#include "windows.h"
#include "QMouseEvent"
#include "QMessageBox"
#include "QTableWidget"

MyTable::MyTable(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyTable)
{
    ui->setupUi(this);

    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(CloseMyTab(int)));
}

void MyTable::mousePressEvent(QMouseEvent *eventPress)
{
    if(eventPress->button() == Qt::RightButton)
    {
        HandleRightClick();
    }

}

MyTable::~MyTable()
{
    delete ui;
}

void MyTable::HandleRightClick()
{
    QString Info{"Do you want to create a table?"};

    QMessageBox::StandardButton reply = QMessageBox::question(this,"Question", Info,QMessageBox::Yes | QMessageBox::No);

    switch (reply) {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
        break;
    }
}

void MyTable::on_actionQuit_triggered()
{
    this->close();
}

void MyTable::CloseMyTab(int indexTab)
{
    ui->tabWidget->removeTab(indexTab);
}

void MyTable::on_actionInfo_triggered()
{
    ShellExecute(NULL,NULL,INFO_URL,NULL,NULL,SW_SHOWNORMAL);
}


#include "mytable.h"
#include "ui_mytable.h"
#include "windows.h"
#include "QMouseEvent"
#include "QMessageBox"
#include "QTableWidget"
#include "QHeaderView"
#include "QPixmap"

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
        emit EmitOpenTableOptionsDialog();
        break;
    case QMessageBox::No:
        break;
    }
}

bool MyTable::CheckNameAvailability(QString sheet_name)
{
    for (int tab_index = 0; tab_index < ui->tabWidget->count(); ++tab_index) {

        if(sheet_name == ui->tabWidget->tabText(tab_index))
        {
            QMessageBox::warning(this,"Warning","There are already sheet with such name, try again.");
            return false;
        }
    }

    return true;
}

void MyTable::CreateTable(QStringList &list)
{
    QTableWidget* widget = new QTableWidget{ui->tabWidget};
    connect(widget->itemDelegate(),SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),this,SLOT(SlotCloseEditor(QWidget*)));
}

void MyTable::ResizeTable(int scale_to_resize) const
{

    QFont NewFont;

    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    int new_pixel_size = widget->horizontalHeader()->font().pixelSize() + (scale_to_resize);
    if(new_pixel_size <= 0)
        return;

    NewFont.setPixelSize(new_pixel_size);
    NewFont.setFamily(widget->horizontalHeader()->font().family());

    widget->verticalHeader()->setFont(NewFont);
    widget->horizontalHeader()->setFont(NewFont);


    for (int i = 0; i < widget->rowCount(); ++i) {
        for (int j = 0; j < widget->columnCount(); ++j) {
            QTableWidgetItem* item = widget->item(i,j);
            item->setFont(NewFont);
        }
    }

    widget->resizeColumnsToContents();
    widget->resizeRowsToContents();
}

void MyTable::SlotCloseEditor(QWidget* item)
{
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    widget->selectionModel()->clear();
    widget->clearFocus();
    widget->resizeColumnsToContents();
    widget->resizeRowsToContents();
}

void MyTable::RecieveInputData(QStringList list)
{
    bool isAvailable = CheckNameAvailability(list[3]);

    if(!isAvailable)
         emit EmitOpenTableOptionsDialog();
    else
         CreateTable(list);
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


void MyTable::on_actionZoom_in_triggered()
{
    ResizeTable(+2);
}


void MyTable::on_actionZoom_out_triggered()
{
    ResizeTable(-2);
}


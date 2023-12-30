#include "mytable.h"
#include "ui_mytable.h"
#include "windows.h"
#include "QMouseEvent"
#include "QMessageBox"
#include "QTableWidget"
#include "QHeaderView"

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
    QFont font;

    font.setFamily(list[2]);
    font.setPixelSize(list[3].toInt());

    widget->setColumnCount(list[5].toInt());
    widget->setRowCount(list[6].toInt());

    widget->setHorizontalHeaderLabels(GenerateHeaders(list[0],widget->columnCount()));
    widget->setVerticalHeaderLabels(GenerateHeaders(list[1],widget->rowCount()));
    widget->verticalHeader()->setFont(font);
    widget->horizontalHeader()->setFont(font);

    for (int i = 0; i < widget->rowCount(); ++i) {
        for (int j = 0; j < widget->columnCount(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem;
            item->setFont(font);
            widget->setItem(i,j,item);
        }
    }
    ui->tabWidget->addTab(widget,list[4]);
}

void MyTable::ResizeTable(int scale_to_resize) const
{
    if(!ui->tabWidget->count())
        return;

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

QStringList MyTable::GenerateHeaders(const QString &type, int count)
{
    QStringList list;

    if(type == "Numerical")
    {
        QString temporary;

        for (int i = 1; i <= count; ++i) {
            temporary.setNum(i);
            list.append(temporary);
        }
    }
    else
    {
        int length = 0;
        QString str;

        for (int i = 0; i < count; i++) {
            if(std::pow(26,i) < count)
            {
                length++;
                continue;
            }
            break;
        }

            for (int cycle = 0; cycle < length; ++cycle) {

                str.append('A');
                for (int i = 0; i <= cycle; ++i) {
                    str.fill('A');
                }
                for (int iteration = 1; iteration <= std::pow(26,str.length() - 1); ++iteration) {

                    char first_letter = 'A';
                    for (int letter_count = 1; letter_count <= 26; ++letter_count) {
                        str[str.length() - 1] = first_letter;
                        list.append(str);
                        first_letter++;
                    }


                    int previous_element_index = (str.length() - 1) - cycle;
                    wchar_t temp = str[previous_element_index].unicode();
                    str[previous_element_index] = static_cast<QChar>(temp + 1);
                }
            }

    }
    return list;
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
    bool isAvailable = CheckNameAvailability(list[NAME_INDEX]);

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


#include "mytable.h"
#include "ui_mytable.h"
#include "windows.h"
#include "QMouseEvent"
#include "QMessageBox"
#include "QTableWidget"
#include "QHeaderView"
#include "QFile"
#include "QFileDialog"

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

void MyTable::readFromTxtFile() const
{

}

void MyTable::writeToTxtFile(QString path_to_file) const
{
    QFile file{path_to_file};

    if(file.open(QIODevice::WriteOnly))
    {
            QTextStream stream{&file};
            QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

            QStringList list;
            QString last_vertical_header_text = widget->verticalHeaderItem(widget->rowCount() - 1)->text();

            for (int i = 0; i < widget->columnCount(); ++i) {

                list.append(FindLongestTextInColumn(i));
            }

            stream << GenerateSpacesForTxtFile(last_vertical_header_text);

            for (int i = 0; i < widget->columnCount(); ++i) {
                stream << GenerateDashForTxtFile(list[i]);
                stream << ' ';
            }

            stream << '\n';

            stream << GenerateSpacesForTxtFile(last_vertical_header_text);

            for (int i = 0; i < widget->columnCount(); ++i) {
                stream << "|";
                QString current_header_text = widget->horizontalHeaderItem(i)->text();
                int current_header_length = current_header_text.length();

                for (int space = 0; space < list[i].length() - current_header_length; ++space) {
                    current_header_text.append(' ');
                }

                current_header_text.append('|');

                stream << current_header_text + ' ';
            }

            stream << '\n';

            for (int i = 0; i < widget->rowCount(); ++i) {
                QString temporary = widget->verticalHeaderItem(i)->text();

                stream << "----";

               // stream << GenerateSpacesForTxtFile(last_vertical_header_text);

                for (int i = 0; i < widget->columnCount(); ++i) {
                    stream << GenerateDashForTxtFile(list[i]);
                    stream << ' ';
                }
                stream << '\n';

                stream << temporary;

                for (int i = 0; i < last_vertical_header_text.length() - temporary.length(); ++i) {
                    stream << ' ';
                }

                stream << "  |";

                for (int j = 0; j < widget->columnCount(); ++j) {
                    if (j != 0)
                        stream << '|';

                    QString current_text = widget->item(i,j)->text();
                    int current_text_length = current_text.length();

                    for (int space = 0; space < list[j].length() - current_text_length; ++space) {
                        current_text.append(' ');
                    }

                    current_text.append('|');
                    stream << current_text + ' ';
                }

                 stream << "\n";
            }

            stream << "----";

            for (int i = 0; i < widget->columnCount(); ++i) {
                 stream << GenerateDashForTxtFile(list[i]);
                 stream << ' ';
            }
    }
    file.close();


}

QString MyTable::FindLongestTextInColumn(int column) const
{
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    QString longest = widget->item(0,column)->text();

    for (int row = 1; row < widget->rowCount(); ++row) {
            QString current_item_text = widget->item(row,column)->text();
            if(longest.length() < current_item_text.length())
                longest = current_item_text;
    }

    if (longest.length() < widget->horizontalHeaderItem(column)->text().length())
            longest = widget->horizontalHeaderItem(column)->text();
    return longest;
}

QString MyTable::GenerateDashForTxtFile(const QString &longest_str) const
{
    QString dashes;

    int length_of_longets_str = longest_str.length();
    int total_length = length_of_longets_str + 2;

    for (int i = 0; i < total_length; ++i) {
            dashes.append('-');
    }
    return dashes;
}

QString MyTable::GenerateSpacesForTxtFile(const QString &longest_str) const
{
    QString spaces;
    int length_of_longets_str = longest_str.length();
    int total_length = length_of_longets_str + 2;

    for (int i = 0; i < total_length; ++i) {
            spaces.append(' ');
    }

    return spaces;
}

void MyTable::SlotCloseEditor(QWidget* item)
{
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    widget->resizeColumnToContents(widget->currentItem()->column());
    widget->selectionModel()->clear();
    widget->clearFocus();
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


void MyTable::on_actionSave_as_triggered()
{
    QString filter = "Text file (*.txt)";
    QString new_file_name = QFileDialog::getSaveFileName(this,"Save string","C:/",filter);
    if(!new_file_name.isEmpty())
         writeToTxtFile(new_file_name);

    qDebug() << new_file_name;
}


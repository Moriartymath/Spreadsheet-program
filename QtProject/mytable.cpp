#include "mytable.h"
#include "ui_mytable.h"
#include "windows.h"
#include "QMouseEvent"
#include "QMessageBox"
#include "QTableWidget"
#include "QHeaderView"
#include "QFile"
#include "QFileDialog"
#include "QInputDialog"

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
    connect(widget->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(HorizontalSectionDoubleClicked(int)));
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

void MyTable::readFromTxtFile(const QString& path_to_file)
{
    QFile file{path_to_file};
    QFileInfo file_info{file};
    QString base_file_name = file_info.baseName();
    qDebug() << base_file_name;


    if(file.open(QIODevice::ReadOnly))
    {
            QTextStream stream{&file};
            QStringList list;
            QString str  = stream.readLine();

            int index_row = 0;

            while(true)
            {
                QString current_row = stream.readLine();

                if(current_row.isEmpty())
                    break;

                if(index_row % 2 == 0)
                {
                    QString a = current_row.remove('|');
                    int spaces_to_erase = 2;
                    for (int space = 0; space < spaces_to_erase; ++space) {
                        a.removeLast();
                    }
                    qDebug() << a << a.length();
                    list.append(a);

                }

                index_row++;
            }

            if(!list.isEmpty())
               WriteToTable(list);


    }

}

void MyTable::WriteToTable(QStringList &list)
{
    QList<int> list_of_indexes;
    QList<QList<int>> sliced_list;
    QStringList Vertical_Headers_Labels;
    QStringList Horizontal_Headers_Labels;
    QTableWidget* widget = new QTableWidget{ui->tabWidget};
    widget->setRowCount(list.length() - 1);

    for (int row = 0; row < list.length(); ++row) {
            if(row == 0)
            {
                QString horizontal_header = list[row];

                std::string str  = horizontal_header.toStdString();
                const char* ptr_to_str = str.c_str();

                size_t length_of_str = std::strlen(ptr_to_str);

                int index = 0;

                while (ptr_to_str[index] != '\n') {

                    if(ptr_to_str[index] == ' ')
                    {
                        index++;
                        continue;
                    }

                    bool IsEndOfLine = false;

                    qDebug() <<"before " <<index;
                    list_of_indexes.append(index);
                    index++;
                    while(ptr_to_str[index] != ' ')
                    {
                        index++;

                        if(index >= length_of_str)
                        {
                            IsEndOfLine = true;
                            index = length_of_str - 1;
                            break;
                        }
                    }
                    if (!IsEndOfLine)
                    {
                        while(ptr_to_str[index] == ' ')
                        {
                            index++;

                            if(index == length_of_str)
                            {
                                IsEndOfLine = true;
                                break;
                            }
                        }
                    }

                    if(IsEndOfLine)
                    {
                        list_of_indexes.append(index);
                        break;
                    }

                    index -= 3;

                    list_of_indexes.append(index);
                    qDebug() <<"after " <<index;
                    if(list_of_indexes[list_of_indexes.length() - 1]  == list_of_indexes[list_of_indexes.length() - 2])
                        index +=3;

                }
                qDebug() << list_of_indexes;

                int length_of_list = list_of_indexes.length();
                for (int i = 0; i < length_of_list / 2; ++i) {

                    QList<int> list_of_two_indexes;
                    int first_two_element = 2;

                    for (int index = 0; index < first_two_element; ++index) {

                        list_of_two_indexes.append(list_of_indexes[index]);
                    }

                    sliced_list.append(list_of_two_indexes);

                    for (int i = 0; i < first_two_element; ++i) {
                        list_of_indexes.removeFirst();
                    }

                }

                for (const auto& index_list : sliced_list) {

                    QString horizontal_label;
                    for (int start = index_list.first(); start <= index_list.last(); ++start) {
                        if(start > list[row].length() - 1)
                            horizontal_label.append(list[row][start - 1]);
                        else
                           horizontal_label.append(list[row][start]);
                    }

                    Horizontal_Headers_Labels.append(horizontal_label.remove(' '));
                }

                qDebug() << Horizontal_Headers_Labels;
            }
            else
            {
                size_t index_end_of_vertical_headers = sliced_list[0][0] - 3;

                QString vertical_header_label =  list[row].first(index_end_of_vertical_headers + 1);

                Vertical_Headers_Labels.append(vertical_header_label.remove(' '));

            }

    }

    qDebug() << Vertical_Headers_Labels;
    qDebug() << sliced_list;
}

void MyTable::writeToTxtFile(const QString &path_to_file) const
{
    QFile file{path_to_file};

    if(file.open(QIODevice::WriteOnly))
    {
            QTextStream stream{&file};
            QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

            QStringList list;
            QString longest_vertical_header_text = FindLongestVerticalHeader();

            for (int i = 0; i < widget->columnCount(); ++i) {

                list.append(FindLongestTextInColumn(i));
            }

            stream << GenerateSpacesForTxtFile(longest_vertical_header_text);

            for (int i = 0; i < widget->columnCount(); ++i) {
                stream << GenerateDashForTxtFile(list[i]);
                stream << "  ";
            }

            stream << '\n';

            stream << GenerateSpacesForTxtFile(longest_vertical_header_text);

            for (int i = 0; i < widget->columnCount(); ++i) {
                stream << "|";
                QString current_header_text = widget->horizontalHeaderItem(i)->text();
                int current_header_length = current_header_text.length();

                for (int space = 0; space < list[i].length() - current_header_length; ++space) {
                    current_header_text.append(' ');
                }

                current_header_text.append('|');

                stream << current_header_text + "  ";
            }

            stream << '\n';

            for (int i = 0; i < widget->rowCount(); ++i) {
                QString temporary = widget->verticalHeaderItem(i)->text();

                stream << GenerateDashForTxtFile(longest_vertical_header_text);

                for (int i = 0; i < widget->columnCount(); ++i) {
                    stream << GenerateDashForTxtFile(list[i]);
                    stream << "  ";
                }
                stream << '\n';

                stream << temporary;

                for (int i = 0; i < longest_vertical_header_text.length() - temporary.length(); ++i) {
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
                    stream << current_text + "  ";
                }

                 stream << "\n";
            }

            stream << GenerateDashForTxtFile(longest_vertical_header_text);

            for (int i = 0; i < widget->columnCount(); ++i) {
                 stream << GenerateDashForTxtFile(list[i]);
                 stream << "  ";
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
    int indents = 2;
    int total_length = length_of_longets_str + indents;

    for (int i = 0; i < total_length; ++i) {
            dashes.append('-');
    }
    return dashes;
}

QString MyTable::GenerateSpacesForTxtFile(const QString &longest_str) const
{
    QString spaces;
    int length_of_longets_str = longest_str.length();
    int indents = 2;
    int total_length = length_of_longets_str + indents;

    for (int i = 0; i < total_length; ++i) {
            spaces.append(' ');
    }

    return spaces;
}

QString MyTable::FindLongestVerticalHeader() const
{
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    QString longest_vertical_header = widget->verticalHeaderItem(0)->text();

    for (int vertical_header_index = 1; vertical_header_index < widget->rowCount(); ++vertical_header_index) {
            QString current_header_text = widget->verticalHeaderItem(vertical_header_index)->text();

            if(longest_vertical_header.length() < current_header_text.length())
                longest_vertical_header = current_header_text;
    }

    return longest_vertical_header;

}

void MyTable::SlotCloseEditor(QWidget* item)
{
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    widget->resizeColumnToContents(widget->currentItem()->column());
    widget->selectionModel()->clear();
    widget->clearFocus();
}

void MyTable::HorizontalSectionDoubleClicked(int index)
{
    qDebug() << index;
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());
    QString new_text = QInputDialog::getText(this,"Label for header","Enter new header label",
                                             QLineEdit::Normal,QString(),nullptr,Qt::MSWindowsFixedSizeDialogHint);

    bool IsNameAvailable = true;
    if (new_text == widget->horizontalHeaderItem(index)->text())
            return;

    for (int i = 0; i < widget->columnCount(); ++i) {
            if(i != index)
            {
                if(widget->horizontalHeaderItem(i)->text() == new_text)
                {
                    QMessageBox::warning(this,"Invalid label`s name","There is already label with such name.");
                    IsNameAvailable = false;
                    break;
                }
            }
    }

    if(IsNameAvailable)
    {
        widget->horizontalHeaderItem(index)->setText(new_text);
        widget->resizeColumnToContents(index);
    }


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
    if (ui->tabWidget->count() == 0)
         return;
    QString tab_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QString filter = "Text file (*.txt)";
    QString new_file_name = QFileDialog::getSaveFileName(this,"Save Table","C:/" + tab_name,filter);
    if(!new_file_name.isEmpty())
         writeToTxtFile(new_file_name);
}


void MyTable::on_actionSave_triggered()
{
}


void MyTable::on_actionOpen_File_triggered()
{
    QString filter = "Text file (*.txt)";
    QString new_file_name = QFileDialog::getOpenFileName(this,"Open file","C:/",filter);
    if(!new_file_name.isEmpty())
         readFromTxtFile(new_file_name);
}


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
    , line_edit_for_status_bar{new QLineEdit}
{
    ui->setupUi(this);
    setWindowTitle(window_title);
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(CloseMyTab(int)));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(CurrentTabChanged(int)));

    //line_edit_for_status_bar->setText("Hello");
    line_edit_for_status_bar->setAlignment(Qt::AlignmentFlag::AlignCenter);
    line_edit_for_status_bar->setModified(false);
    ui->statusbar->addPermanentWidget(line_edit_for_status_bar,3);
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
    widget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
    connect(widget->itemDelegate(),SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),this,SLOT(SlotCloseEditor(QWidget*)));
    connect(widget->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(HorizontalSectionDoubleClicked(int)));
    connect(widget->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(VerticalSectionDoubleClicked(int)));
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
    QStringList list_of_info;
    list_of_info.append(list[4]);
    list_of_info.append("Saved");
    list_of_info.append("0");


    connect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));
    AddToStack(list_of_info,widget);
    ui->tabWidget->addTab(widget,list[4]);
}

void MyTable::ResizeTable(int scale_to_resize) const
{
    if(!ui->tabWidget->count())
        return;

    QFont NewFont;

    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());
    disconnect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));

    int new_pixel_size = widget->horizontalHeader()->font().pixelSize() + (scale_to_resize);
    if(new_pixel_size <= 0)
        return;

    NewFont.setPixelSize(new_pixel_size);
    NewFont.setFamily(widget->horizontalHeader()->font().family());

    widget->verticalHeader()->setFont(NewFont);
    widget->horizontalHeader()->setFont(NewFont);


    for (int i = 0; i < widget->rowCount(); ++i) {
        for (int j = 0; j < widget->columnCount(); ++j) {
            if(i == 0 && j == 0)
               connect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));
            else if(i == 0 && j == 1)
               disconnect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));
            QTableWidgetItem* item = widget->item(i,j);
            item->setFont(NewFont);
        }
    }

    widget->resizeColumnsToContents();
    widget->resizeRowsToContents();

    connect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));
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
    bool IsNameAvailable = CheckNameAvailability(base_file_name);

    if(!IsNameAvailable)
    {
            EnterNewName(base_file_name);
            if(file_info.baseName() == base_file_name)
                return;
    }



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
            {
                QStringList list_of_file_info;
                list_of_file_info.append(base_file_name);
                list_of_file_info.append(path_to_file);
                list_of_file_info.append("Saved");
                list_of_file_info.append("0");
                list_of_sheet_referenced_to_file.append(list_of_file_info);

                WriteToTable(list,base_file_name,list_of_file_info);

            }


    }

}

void MyTable::WriteToTable(QStringList &list, const QString& name_of_file,const QStringList& list_of_file_info)
{
    QList<int> list_of_indexes;
    QList<QList<int>> sliced_list;
    QStringList Vertical_Headers_Labels;
    QStringList Horizontal_Headers_Labels;
    QTableWidget* widget = new QTableWidget{ui->tabWidget};

    widget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(widget->itemDelegate(),SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),this,SLOT(SlotCloseEditor(QWidget*)));
    connect(widget->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(HorizontalSectionDoubleClicked(int)));
    connect(widget->verticalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(VerticalSectionDoubleClicked(int)));
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
                    else
                        index++;

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
                widget->setColumnCount(sliced_list.length());
                widget->setHorizontalHeaderLabels(Horizontal_Headers_Labels);
            }
            else
            {
                size_t index_end_of_vertical_headers = sliced_list[0][0] - 3;

                QString vertical_header_label =  list[row].first(index_end_of_vertical_headers + 1);
                Vertical_Headers_Labels.append(vertical_header_label.remove(' '));

                int column = 0;

                for (const auto& index_list : sliced_list) {

                    QString item_text;
                    for (int start = index_list.first(); start <= index_list.last(); ++start) {
                        if(start > list[row].length() - 1)
                           item_text.append(list[row][start - 1]);
                        else
                           item_text.append(list[row][start]);
                    }

                    QTableWidgetItem* item = new QTableWidgetItem;
                    item->setText(item_text.remove(' '));
                    widget->setItem(row - 1,column,item);
                    column++;
                }

            }
    }


    widget->setVerticalHeaderLabels(Vertical_Headers_Labels);


    connect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));
    AddToStack(list_of_file_info,widget);
    ui->tabWidget->addTab(widget,name_of_file);
}

void MyTable::AddToStack(const QStringList &file_info, QTableWidget *table_widget)
{
    std::pair<std::pair<QStringList,QList<QList<QTableWidgetItem*>>>,QTableWidget*> my_pair;

    std::pair<QStringList,QList<QList<QTableWidgetItem*>>> inner_pair;
    inner_pair.first = file_info;

    QList<QTableWidgetItem*> inner_list;

    for (int row = 0; row < table_widget->rowCount(); ++row) {

            for (int column = 0; column < table_widget->columnCount(); ++column) {
                QTableWidgetItem* copy_item = new QTableWidgetItem;

                copy_item->setText(table_widget->item(row,column)->text());
                copy_item->setFont(table_widget->item(row,column)->font());
                inner_list.append(copy_item);
            }
    }
    inner_pair.second.append(inner_list);


    my_pair.first = inner_pair;
    my_pair.second = table_widget;

    stack_list.append(my_pair);

}

void MyTable::ReverseChanges(int IsReverseDirection)
{

    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());

    disconnect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));

    for (int i = 0; i < stack_list.length(); ++i) {

            if(stack_list[i].second == widget)
            {
                QString current_state;
                if(stack_list[i].first.first.length() == 3)
                    current_state = stack_list[i].first.first[2];
                else
                    current_state = stack_list[i].first.first[3];

                if(IsReverseDirection)
                {
                if(current_state.toInt() >= 1)
                {
                    int current_state_int = current_state.toInt();
                    current_state_int--;
                    current_state.clear();
                    current_state.setNum(current_state_int);
                    if(stack_list[i].first.first.length() == 3)
                        stack_list[i].first.first[2] = current_state;
                    else
                        stack_list[i].first.first[3] = current_state;
                }
                else
                {
                    int current_state_int = current_state.toInt();
                    current_state.clear();
                    current_state.setNum(current_state_int);
                    if(stack_list[i].first.first.length() == 3)
                        stack_list[i].first.first[2] = current_state;
                    else
                        stack_list[i].first.first[3] = current_state;
                }
                }
                else
                {
                if(current_state.toInt() < stack_list[i].first.second.length() - 1)
                {
                    int current_state_int = current_state.toInt();
                    current_state_int++;
                    current_state.clear();
                    current_state.setNum(current_state_int);
                    if(stack_list[i].first.first.length() == 3)
                        stack_list[i].first.first[2] = current_state;
                    else
                        stack_list[i].first.first[3] = current_state;
                }
                else
                {
                    int current_state_int = current_state.toInt();
                    current_state.clear();
                    current_state.setNum(current_state_int);
                    if(stack_list[i].first.first.length() == 3)
                        stack_list[i].first.first[2] = current_state;
                    else
                        stack_list[i].first.first[3] = current_state;
                }

                }
                int column_count = widget->columnCount();
                QList<QTableWidgetItem*> temporary_list;

                int row_index = 0;
                bool if_break_statement = false;

                for (const auto& item: stack_list[i].first.second[current_state.toInt()]) {

                    if (row_index >= widget->rowCount())
                        break;
                    while (temporary_list.length() != column_count) {

                        temporary_list.append(item);

                        if_break_statement = true;
                        break;
                    }

                    if(temporary_list.length() == column_count)
                        if_break_statement = false;

                    if(!if_break_statement){

                        int column_index = 0;
                        for (const auto& inner_item : temporary_list) {

                           QTableWidgetItem* item_to_change = widget->item(row_index,column_index);

                           item_to_change->setText(inner_item->text());
                           //item_to_change->setFont(inner_item->font());
                           column_index++;
                        }

                        temporary_list.clear();
                        row_index++;
                    }
                }

                break;
            }
    }

     widget->horizontalHeader()->setFont(widget->item(0,0)->font());
     widget->verticalHeader()->setFont(widget->item(0,0)->font());
     //widget->resizeColumnsToContents();
     //widget->resizeRowsToContents();

    connect(widget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(ItemChanged(QTableWidgetItem*)));
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

void MyTable::EnterNewName(QString& sheet_name)
{
    QString new_sheet_name = QInputDialog::getText(this,"New Sheet`s name","Enter new name to be assigned",
                             QLineEdit::Normal,QString(),nullptr,Qt::MSWindowsFixedSizeDialogHint);

    bool IsNameAvailable = CheckNameAvailability(new_sheet_name);
    if( IsNameAvailable && !new_sheet_name.isEmpty())
        sheet_name = new_sheet_name;
    else
    {
        QMessageBox::Button reply = QMessageBox::question(this,"Question","Do you want to try again?");

        if(reply == QMessageBox::Yes)
            EnterNewName(sheet_name);
        else
            return;
    }




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
                    QMessageBox::warning(this,"Invalid header`s name","There is already header with such name.");
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

void MyTable::VerticalSectionDoubleClicked(int index)
{
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());
    QString new_text = QInputDialog::getText(this,"Label for header","Enter new header labels",
                                             QLineEdit::Normal,QString(),nullptr,Qt::MSWindowsFixedSizeDialogHint);

    bool IsNameAvailable = true;
    if (new_text == widget->verticalHeaderItem(index)->text())
        return;

    for (int i = 0; i < widget->rowCount(); ++i) {
        if(i != index)
        {
                if(widget->verticalHeaderItem(i)->text() == new_text)
                {
                    QMessageBox::warning(this,"Invalid header`s name","There is already header with such name.");
                    IsNameAvailable = false;
                    break;
                }
        }
    }

    if(IsNameAvailable)
    {
        widget->verticalHeaderItem(index)->setText(new_text);
        widget->resizeColumnToContents(index);
        widget->resizeRowToContents(index);
    }
}

void MyTable::CurrentTabChanged(int tab_index)
{
    bool IsWindowTitleChanged = false;
    for (int i = 0; i < list_of_sheet_referenced_to_file.length(); ++i) {
        QString name_of_uploaded_sheet = list_of_sheet_referenced_to_file[i][0];

        if(ui->tabWidget->tabText(tab_index) == name_of_uploaded_sheet)
        {
                QString path_to_file = list_of_sheet_referenced_to_file[i][1];
                this->setWindowTitle(window_title + "\tCurrent Tab - \" " + name_of_uploaded_sheet + " \" referenced to file - " +  path_to_file);
                IsWindowTitleChanged = true;
                break;
        }
    }
    QTableWidgetItem item;


    if(!IsWindowTitleChanged)
    {
        setWindowTitle(window_title + "\tCurrent Tab - \" " + ui->tabWidget->tabText(tab_index) + " \"");
    }
}

void MyTable::ItemChanged(QTableWidgetItem *item)
{
    qDebug() << "item changed";
    QTableWidget* current_changed_table = item->tableWidget();
    for (int i = 0; i < stack_list.length(); ++i) {
        if(stack_list[i].second == current_changed_table)
        {
                QString current_state;
                int current_state_int;
                if(stack_list[i].first.first.length() == 3)
                {
                    stack_list[i].first.first[1] = "Unsaved Changes";
                    current_state = stack_list[i].first.first[2];
                    current_state_int = current_state.toInt();
                    current_state_int++;
                    current_state.clear();
                    current_state.setNum(current_state_int);
                    stack_list[i].first.first[2] = current_state;
                }
                else
                {
                    stack_list[i].first.first[2] = "Unsaved Changes";
                    current_state = stack_list[i].first.first[3];

                    current_state_int = current_state.toInt();
                    current_state_int++;

                    if(stack_list[i].first.second.length() - 1 > current_state_int)
                        current_state_int = stack_list[i].first.second.length();


                    current_state.clear();
                    current_state.setNum(current_state_int);
                    stack_list[i].first.first[3] = current_state;

                    qDebug() << stack_list[i].first.first[3];
                }
                line_edit_for_status_bar->setText("Unsaved Changes");

            QList<QTableWidgetItem*> list_of_new_items;

            for (int row= 0; row < current_changed_table->rowCount(); ++row) {

                    for (int column = 0; column < current_changed_table->columnCount(); ++column) {

                        QTableWidgetItem* copy_item = new QTableWidgetItem;

                        copy_item->setFont(current_changed_table->item(row,column)->font());
                        copy_item->setText(current_changed_table->item(row,column)->text());

                        list_of_new_items.append(copy_item);
                    }
            }
            stack_list[i].first.second.append(list_of_new_items);
            break;
        }
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
    if(!ui->tabWidget->count())
         setWindowTitle(window_title);
    line_edit_for_status_bar->setText("");
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
    QTableWidget* widget = static_cast<QTableWidget*>(ui->tabWidget->currentWidget());


    for (int i = 0; i < stack_list.length(); ++i) {

         if(stack_list[i].second == widget)
         {
            if(stack_list[i].first.first.length() == 4)
            {
                QString path_to_file  = stack_list[i].first.first[1];
                writeToTxtFile(path_to_file);
                stack_list[i].first.first[2] = "Saved";
                line_edit_for_status_bar->setText("Saved");
            }

         }
    }


}


void MyTable::on_actionOpen_File_triggered()
{
    QString filter = "Text file (*.txt)";
    QString new_file_name = QFileDialog::getOpenFileName(this,"Open file","C:/",filter);
    if(!new_file_name.isEmpty())
         readFromTxtFile(new_file_name);
}


void MyTable::on_action_undo_triggered()
{
    ReverseChanges(true);
}

void MyTable::on_actionredo_triggered()
{
    ReverseChanges(false);
}


void MyTable::on_actionCreate_new_table_triggered()
{
    HandleRightClick();
}


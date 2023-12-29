#include "tableoptions.h"
#include "ui_tableoptions.h"

TableOptions::TableOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableOptions)
{
    ui->setupUi(this);
    SetPlaceHolderText();
}

TableOptions::~TableOptions()
{
    delete ui;
}

void TableOptions::SetPlaceHolderText()
{
    QStringList str_list;
    for (const QString& str : {"Horizontal Headers","Vertical Headers","Font"}) {
        str_list.append("--Select " + str + "--");
    }

    int index = 0;
    for (QComboBox* combo_box : {ui->Horiz_headers,ui->Vert_headers,(QComboBox*)ui->fontComboBox})
    {
        combo_box->setPlaceholderText(str_list[index++]);
        combo_box->setCurrentIndex(-1);
    }

    ui->sheet_name->setPlaceholderText("Sheet Name");

}

void TableOptions::AppendParaments(QStringList &list)
{
    for (QComboBox* combo_box : {ui->Horiz_headers,ui->Vert_headers,(QComboBox*)ui->fontComboBox})
    {
        if(combo_box->currentIndex() != (-1))
        {
            combo_box->setStyleSheet("");
            list.append(combo_box->currentText());
        }
        else
            combo_box->setStyleSheet("border: 2px solid red");
    }

    if(ui->sheet_name->text().isEmpty())
        ui->sheet_name->setStyleSheet("border: 2px solid red");
    else
    {
        ui->sheet_name->setStyleSheet("");
        list.append(ui->sheet_name->text());
    }

    list.append(ui->row_count->text());
    list.append(ui->column_count->text());
}

void TableOptions::SlotOpenTableOptionsDialog()
{
    setModal(true);
    show();
}

void TableOptions::on_continue_button_clicked()
{
    QStringList list;

    AppendParaments(list);

    if (list.length() == VALID_PARAMETRS)
    {
        close();
        emit SendInputData(list);
    }
}


void TableOptions::on_cancel_button_clicked()
{
    close();
}


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

}

void TableOptions::SlotOpenTableOptionsDialog()
{
    setModal(true);
    show();
}

void TableOptions::on_continue_button_clicked()
{
    for (QComboBox* combo_box : {ui->Horiz_headers,ui->Vert_headers,(QComboBox*)ui->fontComboBox})
    {
        if(combo_box->currentIndex() != (-1))
        {
            qDebug() << combo_box->currentText();
        }
    }
}


void TableOptions::on_cancel_button_clicked()
{
    close();
}


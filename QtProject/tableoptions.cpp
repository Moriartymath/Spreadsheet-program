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

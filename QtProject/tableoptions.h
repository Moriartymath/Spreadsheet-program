#ifndef TABLEOPTIONS_H
#define TABLEOPTIONS_H

#include <QDialog>

namespace Ui {
class TableOptions;
}

class TableOptions : public QDialog
{
    Q_OBJECT

public:
    explicit TableOptions(QWidget *parent = nullptr);
    ~TableOptions();
private:
    void SetPlaceHolderText();

private slots:
    void SlotOpenTableOptionsDialog();

    void on_continue_button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::TableOptions *ui;
};

#endif // TABLEOPTIONS_H

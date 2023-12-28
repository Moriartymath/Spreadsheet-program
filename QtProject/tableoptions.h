#ifndef TABLEOPTIONS_H
#define TABLEOPTIONS_H

#include <QDialog>

namespace Ui {
class TableOptions;
}

#define VALID_PARAMETRS 6

class TableOptions : public QDialog
{
    Q_OBJECT

public:
    explicit TableOptions(QWidget *parent = nullptr);
    ~TableOptions();

private:
    void SetPlaceHolderText();
    void AppendParaments(QStringList& list);

public slots:
    void SlotOpenTableOptionsDialog();

private slots:
    void on_continue_button_clicked();

    void on_cancel_button_clicked();

signals:
    void SendInputData(QStringList list);

private:
    Ui::TableOptions *ui;
};

#endif // TABLEOPTIONS_H

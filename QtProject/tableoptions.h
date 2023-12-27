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

private:
    Ui::TableOptions *ui;
};

#endif // TABLEOPTIONS_H

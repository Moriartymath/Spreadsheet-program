#ifndef MYTABLE_H
#define MYTABLE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MyTable; }
QT_END_NAMESPACE

class MyTable : public QMainWindow
{
    Q_OBJECT

public:
    MyTable(QWidget *parent = nullptr);
    ~MyTable();

private:
    Ui::MyTable *ui;
};
#endif // MYTABLE_H

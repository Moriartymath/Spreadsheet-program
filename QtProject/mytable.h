#ifndef MYTABLE_H
#define MYTABLE_H

#include <QMainWindow>

#define INFO_URL L"https://github.com/Moriartymath/Spreadsheet-program"

QT_BEGIN_NAMESPACE
namespace Ui { class MyTable; }
QT_END_NAMESPACE

class MyTable : public QMainWindow
{
    Q_OBJECT

public:
    MyTable(QWidget *parent = nullptr);
    virtual void mousePressEvent(QMouseEvent *eventPress) override;
    ~MyTable();
private:
    void HandleRightClick();

private slots:
    void on_actionQuit_triggered();
    void CloseMyTab(int indexTab);
    void on_actionInfo_triggered();

private:
    Ui::MyTable *ui;
};
#endif // MYTABLE_H

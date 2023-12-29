#ifndef MYTABLE_H
#define MYTABLE_H

#include <QMainWindow>

#define INFO_URL L"https://github.com/Moriartymath/Spreadsheet-program"

QT_BEGIN_NAMESPACE
namespace Ui { class MyTable; }
QT_END_NAMESPACE
class QTableWidgetItem;

class MyTable : public QMainWindow
{
    Q_OBJECT

public:
    MyTable(QWidget *parent = nullptr);
    virtual void mousePressEvent(QMouseEvent *eventPress) override;
    ~MyTable();
private:
    void HandleRightClick();
    bool CheckNameAvailability(QString sheet_name);
    void CreateTable(QStringList& list);
    void ResizeTable(int scale_to_resize) const;

public slots:
    void RecieveInputData(QStringList list);
private slots:
    void on_actionQuit_triggered();
    void CloseMyTab(int indexTab);
    void on_actionInfo_triggered();
    void SlotCloseEditor(QWidget* item);

    void on_actionZoom_in_triggered();

    void on_actionZoom_out_triggered();

signals:
    void EmitOpenTableOptionsDialog();

private:
    Ui::MyTable *ui;
};
#endif // MYTABLE_H

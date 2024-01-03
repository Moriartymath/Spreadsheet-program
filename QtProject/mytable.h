#ifndef MYTABLE_H
#define MYTABLE_H

#include <QMainWindow>

#define INFO_URL L"https://github.com/Moriartymath/Spreadsheet-program"
#define NUMBERS_HEADER "Numbers"
#define NAME_INDEX 4

QT_BEGIN_NAMESPACE
namespace Ui { class MyTable; }
QT_END_NAMESPACE
class QTableWidgetItem;
class QTableWidget;
class QLineEdit;

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
    QStringList GenerateHeaders(const QString& type, int count);
    void readFromTxtFile(const QString& path_to_file);
    void WriteToTable(QStringList& list, const QString& name_of_file,const QStringList& list_of_file_info);
    void writeToTxtFile(const QString& path_to_file) const;
    QString FindLongestTextInColumn(int column) const;
    QString GenerateDashForTxtFile(const QString& longest_str) const;
    QString GenerateSpacesForTxtFile(const QString& longest_str) const;
    QString FindLongestVerticalHeader() const;
    void EnterNewName(QString& sheet_name);
    void AddToStack(const QStringList& file_info,QTableWidget* table_widget);


public slots:
    void RecieveInputData(QStringList list);
private slots:
    void on_actionQuit_triggered();
    void CloseMyTab(int indexTab);
    void on_actionInfo_triggered();
    void SlotCloseEditor(QWidget* item);
    void HorizontalSectionDoubleClicked(int index);
    void VerticalSectionDoubleClicked(int index);
    void CurrentTabChanged(int tab_index);
    void ItemChanged(QTableWidgetItem* item);

    void on_actionZoom_in_triggered();

    void on_actionZoom_out_triggered();

    void on_actionSave_as_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_File_triggered();

    void on_action_undo_triggered();

signals:
    void EmitOpenTableOptionsDialog();

private:
    Ui::MyTable *ui;
    QList<QStringList> list_of_sheet_referenced_to_file;
    const QString window_title = "Spreadsheet-program";
    QList<std::pair<std::pair<QStringList,QList<QList<QTableWidgetItem*>>>,QTableWidget*>> stack_list;
    QLineEdit* line_edit_for_status_bar;
};
#endif // MYTABLE_H

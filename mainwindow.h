#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    Indexer* indx_ptr_;
    SearchType type_;
    CompareType comp_type_;
    Ui::MainWindow* ui;

    QToolBar* ptb_;
    QAction* start_action_;
    QAction* pause_action_;
    QAction* stop_action_;
    QAction* search_action_;
    QAction* s_line_action_;
    QAction* s_date_action_;
    QAction* stacked_wgt_action_;
    QStackedWidget* stacked_wgt_;
    QLineEdit* s_line_;
    QDateEdit* s_date_;
    QComboBox* s_combo_;
    QComboBox* s_combo_comp_;
    QFileSystemModel* f_model_;
    QTableWidget* table_wgt_;
    QHeaderView* header_;
    QTreeView* tree_view_;
    QSplitter* splitter_;
    QHBoxLayout* h_main_layout_;
    QProgressBar* pb;

    void BuildToolbar();
    void DefaultTableWgtInit();
    void DefaultTreeInit();
    void InitReadIndex();
    void SwitchButtons(Condition state);
public:
    MainWindow(Indexer* indx_ref, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onActionStart();
    void onActionStop();
    void onActionPause();
    void onActionSearch();

    void ActionsAfterIndexing();
    void ActionsAfterReadingIndex(QString msg);
    void ActionsAfterSearch(int count);

    void WriteIndexMsg(QString msg);
    void ShowProgressBar(int progress);
    void ShowMessage(QString msg);
    void ShowCurrDir(QString path, int count);
    void ShowDir(int row, int col);
    void ShowAlloc(int row, int col);
    void ShowMsgBox(int count);
    void DisplayFileInfo(FileInfo info);
    void setSearchType(QString type);
    void setCompareType(QString type);
    void CheckSearchLine(QString text);
};

#endif // MAINWINDOW_H

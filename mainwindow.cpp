#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(Indexer* indx_ref, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    indx_ptr_ = indx_ref;
    type_ = BY_NAME;
    comp_type_ = EQUAL;
    QWidget* wgt = new QWidget(this);
    setCentralWidget (wgt);

    ptb_ = new QToolBar(this);
    s_line_ = new QLineEdit(this);
    s_date_ = new QDateEdit(this);
    s_combo_ = new QComboBox(this);
    s_combo_comp_ = new QComboBox(this);
    f_model_ = new QFileSystemModel(this);
    tree_view_ = new QTreeView(this);
    table_wgt_ = new QTableWidget(this);
    h_main_layout_ = new QHBoxLayout();
    splitter_ = new QSplitter(Qt::Horizontal, this);
    stacked_wgt_ = new QStackedWidget(this);

    BuildToolbar ();
    DefaultTableWgtInit ();
    DefaultTreeInit ();
    QObject::connect(table_wgt_, &QTableWidget::cellDoubleClicked, this, &MainWindow::ShowDir);
    QObject::connect(table_wgt_, &QTableWidget::cellClicked, this, &MainWindow::ShowAlloc);

    splitter_->addWidget (table_wgt_);
    splitter_->addWidget (tree_view_);
    splitter_->setSizes (QList<int>() << 100 << 50);
    h_main_layout_->addWidget (splitter_);

    InitReadIndex();

    wgt->setLayout (h_main_layout_);
    setMinimumSize(600, 300);
}

MainWindow::~MainWindow() {
    delete ui;
}
// private methods -------------------------------------------------------------------------
void MainWindow::BuildToolbar() {
    s_line_->setFixedSize (150, SIZE_WID);
    s_line_->setPlaceholderText ("Search...");

    s_date_->setCalendarPopup (true);
    s_date_->setDisplayFormat ("dd.MM.yyyy");
    s_date_->setFixedSize (150, SIZE_WID);
    s_date_->setDate (QDate::currentDate ());

    stacked_wgt_->addWidget (s_line_);
    stacked_wgt_->addWidget (s_date_);
    stacked_wgt_->setFixedSize (150, SIZE_WID);

    s_combo_->setMaximumHeight (SIZE_WID);
    s_combo_->addItems (S_TYPE);
    s_combo_comp_->setMaximumHeight (SIZE_WID);
    s_combo_comp_->addItems (COMP_SET_1);

    start_action_ = ptb_->addAction(QPixmap(":/buttons/resources/start.png"), "Start indexing", this, SLOT(onActionStart()));
    ptb_->addSeparator ();
    pause_action_ = ptb_->addAction(QPixmap(":/buttons/resources/pause.png"), "Pause indexing", this, SLOT(onActionPause()));
    stop_action_ = ptb_->addAction(QPixmap(":/buttons/resources/stop.png"), "Stop indexing/searching", this, SLOT(onActionStop()));
    ptb_->addSeparator ();
    ptb_->addWidget (new QLabel(" Search by ", this));
    ptb_->addWidget (s_combo_);
    ptb_->addSeparator ();
    ptb_->addWidget (s_combo_comp_);
    ptb_->addSeparator ();
    stacked_wgt_action_ = ptb_->addWidget (stacked_wgt_);
    stacked_wgt_->setCurrentWidget (s_line_);
    search_action_ = ptb_->addAction(QPixmap(":/buttons/resources/search.png"), "Search in index", this, SLOT(onActionSearch()));

    ptb_->setMovable (false);
    ptb_->setIconSize (QSize(SIZE_WID, SIZE_WID));
    addToolBar(Qt::TopToolBarArea, ptb_);

    QObject::connect (s_combo_, &QComboBox::currentTextChanged, this, &MainWindow::setSearchType, Qt::UniqueConnection);
    QObject::connect (s_combo_comp_, &QComboBox::currentTextChanged, this, &MainWindow::setCompareType, Qt::UniqueConnection);
    QObject::connect (s_line_, &QLineEdit::textChanged, this, &MainWindow::CheckSearchLine);
}

void MainWindow::DefaultTableWgtInit() {
    table_wgt_->clear ();
#ifndef _WIN32
        malloc_trim(0);
#endif
    table_wgt_->setRowCount (0);
    table_wgt_->setColumnCount(5);
    table_wgt_->setHorizontalHeaderLabels (S_TYPE << "Path");
    table_wgt_->verticalHeader()->setVisible(false);
    table_wgt_->setShowGrid(false);
    table_wgt_->setWordWrap(false);
    table_wgt_->setEditTriggers (QAbstractItemView::NoEditTriggers);
    table_wgt_->setSelectionBehavior(QAbstractItemView::SelectRows);
    header_ = table_wgt_->horizontalHeader();
    header_->setSectionResizeMode(0, QHeaderView::Stretch);
}

void MainWindow::DefaultTreeInit() {
    f_model_->setRootPath(QDir::rootPath());
    tree_view_->setModel(f_model_);
    for(int i = 1; i < 4; ++i) {
        tree_view_->hideColumn (i);
    }
}

void MainWindow::InitReadIndex() {
    SwitchButtons (DISABLED);
    ShowMessage ("Reading from index... Please wait...");

    QThread* read_indx_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (read_indx_thread);

    QObject::connect (read_indx_thread, &QThread::started, contr, &Controller::ReadIndex, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::SendReadResult, this, &MainWindow::ActionsAfterReadingIndex, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, read_indx_thread, &QThread::quit, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, contr, &Controller::deleteLater, Qt::UniqueConnection);
    QObject::connect (read_indx_thread, &QThread::finished, read_indx_thread, &QThread::deleteLater, Qt::UniqueConnection);
    read_indx_thread->start ();
}

void MainWindow::SwitchButtons(Condition state) {
    switch(state) {
    case DEFAULT:
        start_action_->setEnabled (true);
        stop_action_->setDisabled (true);
        pause_action_->setDisabled (true);
        stacked_wgt_->setEnabled (true);
        s_combo_->setEnabled (true);
        s_combo_comp_->setEnabled (true);
        if (type_ != BY_DATE) {
            CheckSearchLine (s_line_->text ());
        }
        else{
            search_action_->setEnabled (true);
        }
        break;
    case START:
        start_action_->setDisabled (true);
        stop_action_->setEnabled (true);
        pause_action_->setEnabled (true);
        search_action_->setDisabled (true);
        stacked_wgt_->setDisabled (true);
        s_combo_->setDisabled (true);
        s_combo_comp_->setDisabled (true);
        break;
    case STOP:
        start_action_->setEnabled (true);
        stop_action_->setDisabled (true);
        pause_action_->setDisabled (true);
        search_action_->setEnabled (true);
        break;
    case PAUSE:
        start_action_->setEnabled (true);
        stop_action_->setEnabled (true);
        pause_action_->setDisabled (true);
        search_action_->setDisabled (true);
        break;
    case SEARCH:
        start_action_->setDisabled (true);
        stop_action_->setEnabled (true);
        pause_action_->setDisabled (true);
        search_action_->setDisabled (true);
        stacked_wgt_->setDisabled (true);
        s_combo_->setDisabled (true);
        s_combo_comp_->setDisabled (true);
        break;
    case DISABLED:
        start_action_->setDisabled (true);
        stop_action_->setDisabled (true);
        pause_action_->setDisabled (true);
        search_action_->setDisabled (true);
        stacked_wgt_->setDisabled (true);
        s_combo_->setDisabled (true);
        s_combo_comp_->setDisabled (true);
    }
}

// public methods ------------------------------------------------------------------------------------------
void MainWindow::onActionStart() {
    SwitchButtons(START);
    DefaultTableWgtInit();
    ShowMessage ("1/2 " + SEARCH_IN_FS);
    if (indx_ptr_->CheckState () == PAUSE){
        indx_ptr_->SetState (START);
        indx_ptr_->Resume();
    }
    else{
        QThread* start_thread = new QThread;
        Controller* contr = new Controller(indx_ptr_);
        contr->moveToThread (start_thread);

        QObject::connect (start_thread, &QThread::started, contr, &Controller::onStartButtonClick, Qt::UniqueConnection);
        QObject::connect (contr, &Controller::IndexWriteStarted, this, &MainWindow::WriteIndexMsg, Qt::UniqueConnection);
        QObject::connect (indx_ptr_, &Indexer::SendCurrDir, this, &MainWindow::ShowCurrDir, Qt::UniqueConnection);
        QObject::connect (indx_ptr_, &Indexer::SendCount, this, &MainWindow::ShowProgressBar, Qt::UniqueConnection);
        QObject::connect (contr, &Controller::finished, this, &MainWindow::ActionsAfterIndexing, Qt::UniqueConnection);
        QObject::connect (contr, &Controller::finished, start_thread, &QThread::quit, Qt::UniqueConnection);
        QObject::connect (contr, &Controller::finished, contr, &Controller::deleteLater, Qt::UniqueConnection);
        QObject::connect (start_thread, &QThread::finished, start_thread, &QThread::deleteLater, Qt::UniqueConnection);

        start_thread->start ();
    }
}

void MainWindow::onActionStop() {
    SwitchButtons(STOP);
    ShowMessage ("Stopped...");
    QThread* stop_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (stop_thread);

    QObject::connect (stop_thread, &QThread::started, contr, &Controller::onStopButtonClick, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, stop_thread, &QThread::quit, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, contr, &Controller::deleteLater, Qt::UniqueConnection);
    QObject::connect (stop_thread, &QThread::finished, stop_thread, &QThread::deleteLater, Qt::UniqueConnection);
    stop_thread->start ();
}

void MainWindow::onActionPause() {
    SwitchButtons(PAUSE);
    ui->s_bar->showMessage ("Paused...");

    QThread* pause_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (pause_thread);

    QObject::connect (pause_thread, &QThread::started, contr, &Controller::onPauseButtonClick, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, pause_thread, &QThread::quit, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, contr, &Controller::deleteLater, Qt::UniqueConnection);
    QObject::connect (pause_thread, &QThread::finished, pause_thread, &QThread::deleteLater, Qt::UniqueConnection);
    pause_thread->start ();
}

void MainWindow::onActionSearch() {
    indx_ptr_->SetSerchInFs (false);
    QString key = type_ == BY_DATE ? s_date_->text (): s_line_->text ();
    SwitchButtons(SEARCH);
    DefaultTableWgtInit();
    ShowMessage ("Searching...");
    QThread* search_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (search_thread);

    QObject::connect (search_thread, &QThread::started, contr, [=] { contr->onSearchButtonClick(type_, comp_type_, key); }, Qt::UniqueConnection);
    QObject::connect (indx_ptr_, &Indexer::SendInfoToView, this, &MainWindow::DisplayFileInfo, Qt::UniqueConnection);
    QObject::connect (indx_ptr_, &Indexer::SendCurrDir, this, &MainWindow::ShowCurrDir, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::SendSearchResCount, this, &MainWindow::ActionsAfterSearch, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::CallMsgBox, this, &MainWindow::ShowMsgBox, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, search_thread, &QThread::quit, Qt::UniqueConnection);
    QObject::connect (contr, &Controller::finished, contr, &Controller::deleteLater, Qt::UniqueConnection);
    QObject::connect (search_thread, &QThread::finished, search_thread, &QThread::deleteLater, Qt::UniqueConnection);
    search_thread->start ();
}

void MainWindow::ActionsAfterIndexing() {
    ui->s_bar->removeWidget (pb);
    InitReadIndex();
    ShowMessage ("Count of dirs: " + QString::number(indx_ptr_->GetDirCount ()) +
                            ",    Count of objects: " + QString::number (indx_ptr_->GetObjectCount ()));
}

void MainWindow::ActionsAfterReadingIndex(QString msg) {
    SwitchButtons(DEFAULT);
    ShowMessage(msg);
}

void MainWindow::ActionsAfterSearch (int count) {
    SwitchButtons(DEFAULT);
    ShowMessage ("Found " + QString::number(count) + " objects");
}

void MainWindow::WriteIndexMsg(QString msg) {
    SwitchButtons(DISABLED);
    ShowMessage(msg);

    pb = new QProgressBar(this);
    pb->setRange (0, (int)indx_ptr_->GetObjectCount ());
    pb->setFixedSize (150, 15);
    pb->setStyleSheet (STYLE_PB);
    ui->s_bar->addPermanentWidget (pb);
}

void MainWindow::ShowProgressBar(int progress){
    pb->setValue (progress);
}

void MainWindow::ShowMessage(QString msg){
    ui->s_bar->showMessage(msg);
}

void MainWindow::ShowCurrDir(QString path, int count) {
    QString pref = indx_ptr_->CheckState () == START ? "1/2 " : "";
    ShowMessage (pref + SEARCH_IN_FS + " | " + QString::number (count) + " objects found | Current dir: " + path);
}

void MainWindow::ShowDir(int row, int col) {
    QString path = table_wgt_->item(row, 4)->text ();
    QString extension = table_wgt_->item(row, 1)->text ();
    if(extension == DIR_EXT ? QDir(path).exists () : QFileInfo(path).exists ()) {
        QDesktopServices::openUrl (QUrl::fromLocalFile (path));
    }
    else {
        QMessageBox::critical (this, "Error!", "Object does not exist!");
        table_wgt_->removeRow (row);
        ActionsAfterSearch(table_wgt_->rowCount ());
    }
}

void MainWindow::ShowAlloc(int row, int col) {
    QModelIndex indx = f_model_->index (table_wgt_->item(row, 4)->text ());
    tree_view_->setCurrentIndex (indx);
}

void MainWindow::ShowMsgBox (int count) {
    SwitchButtons(SEARCH);
    QMessageBox* msgbox = new QMessageBox(QMessageBox::Question,
                                          "Search in filesystem...",
                                          "Found " + QString::number (count) + " object(s).\nDo you want to continue search in filesystem?",
                                          QMessageBox::No | QMessageBox::Yes,
                                          this);
    if(msgbox->exec () == QMessageBox::Yes) {
        indx_ptr_->SetSerchInFs (true);
        DefaultTableWgtInit ();
    }
    else {
        indx_ptr_->SetSerchInFs (false);
    }
    indx_ptr_->SetState (SEARCH);
    indx_ptr_->Resume();
}


void MainWindow::DisplayFileInfo(FileInfo info) {
    table_wgt_->insertRow(table_wgt_->rowCount());

    for(int col = 0; col < 5; ++col) {
        QTableWidgetItem* item = new QTableWidgetItem(col == 0 ? info.name :
                                                      col == 1 ? info.extension :
                                                      col == 2 ? info.size :
                                                      col == 3 ? info.date :
                                                               info.path);
        table_wgt_->setItem(table_wgt_->rowCount() - 1, col, item);
    }
    if (!(table_wgt_->rowCount() % 256) && indx_ptr_->SearchInFs () == false) {
        ShowMessage("Searching... " + QString::number(table_wgt_->rowCount()) + " objects already found...");
    }
}

void MainWindow::setSearchType(QString type) {
    if(type == NAME_STR) {
        type_ = BY_NAME;
        stacked_wgt_->setCurrentWidget (s_line_);
        s_combo_comp_->clear ();
        s_combo_comp_->addItems (COMP_SET_1);
        CheckSearchLine (s_line_->text ());
    }
    if(type == EXTENSION_STR){
        type_ = BY_EXTENSION;
        stacked_wgt_->setCurrentWidget (s_line_);
        s_combo_comp_->clear ();
        s_combo_comp_->addItems (COMP_SET_1);
        CheckSearchLine (s_line_->text ());
    }
    if(type == SIZE_STR){
        type_ = BY_SIZE;
        stacked_wgt_->setCurrentWidget (s_line_);
        s_combo_comp_->clear ();
        s_combo_comp_->addItems (COMP_SET_2);
        CheckSearchLine (s_line_->text ());
    }
    if(type == DATE_STR){
        type_ = BY_DATE;
        stacked_wgt_->setCurrentWidget (s_date_);
        s_combo_comp_->clear ();
        s_combo_comp_->addItems (COMP_SET_2);
        search_action_->setEnabled (true);
    }
}

void MainWindow::setCompareType(QString type) {
    if(type == EQUAL_STR) comp_type_ = EQUAL;
    if(type == NOT_EQUAL_STR) comp_type_ = NOT_EQUAL;
    if(type == CONTAINS_STR) comp_type_ = CONTAINS;
    if(type == LESS_STR) comp_type_ = LESS;
    if(type == GREATER_STR) comp_type_ = GREATER;
    if(type == LESS_EQUAL_STR) comp_type_ = LESS_EQUAL;
    if(type == GREATER_EQUAL_STR) comp_type_ = GREATER_EQUAL;
}

void MainWindow::CheckSearchLine(QString text) {
    if(text == "") {
        search_action_->setDisabled (true);
    }
    else {
        search_action_->setEnabled (true);
    }
}

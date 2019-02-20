#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(Indexer* indx_ref, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    indx_ptr_ = indx_ref;
    type_ = BY_NAME;

    QWidget* wgt = new QWidget(this);
    setCentralWidget (wgt);

    ptb_ = new QToolBar(this);
    s_line_ = new QLineEdit(this);
    s_date_ = new QDateEdit(this);
    s_combo_ = new QComboBox(this);
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

    start_action_ = ptb_->addAction(QPixmap(":/buttons/resources/start.png"), "Start indexing", this, SLOT(onActionStart()));
    ptb_->addSeparator ();
    pause_action_ = ptb_->addAction(QPixmap(":/buttons/resources/pause.png"), "Pause indexing", this, SLOT(onActionPause()));
    stop_action_ = ptb_->addAction(QPixmap(":/buttons/resources/stop.png"), "Stop indexing/searching", this, SLOT(onActionStop()));
    ptb_->addSeparator ();
    ptb_->addWidget (new QLabel(" Search by ", this));
    ptb_->addWidget (s_combo_);
    ptb_->addSeparator ();
    stacked_wgt_action_ = ptb_->addWidget (stacked_wgt_);
    stacked_wgt_->setCurrentWidget (s_line_);
    search_action_ = ptb_->addAction(QPixmap(":/buttons/resources/search.png"), "Search in index", this, SLOT(onActionSearch()));

    ptb_->setMovable (false);
    ptb_->setIconSize (QSize(SIZE_WID, SIZE_WID));
    addToolBar(Qt::TopToolBarArea, ptb_);

    QObject::connect (s_combo_, &QComboBox::currentTextChanged, this, &MainWindow::setSearchType, Qt::UniqueConnection);
    QObject::connect (s_line_, &QLineEdit::textChanged, this, &MainWindow::CheckSearchLine);
}

void MainWindow::DefaultTableWgtInit() {
    table_wgt_->clear ();
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
    ui->s_bar->showMessage ("Reading from index... Please wait...");

    QThread* read_indx_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (read_indx_thread);

    QObject::connect (read_indx_thread, &QThread::started, contr, &Controller::ReadIndex, Qt::UniqueConnection);
    QObject::connect (indx_ptr_, &Indexer::Message, this, &MainWindow::ShowMessage, Qt::UniqueConnection);
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
        CheckSearchLine (s_line_->text ());
        break;
    case START:
        start_action_->setDisabled (true);
        stop_action_->setEnabled (true);
        pause_action_->setEnabled (true);
        search_action_->setDisabled (true);
        stacked_wgt_->setDisabled (true);
        s_combo_->setDisabled (true);
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
        break;
    case DISABLED:
        start_action_->setDisabled (true);
        stop_action_->setDisabled (true);
        pause_action_->setDisabled (true);
        search_action_->setDisabled (true);
        stacked_wgt_->setDisabled (true);
        s_combo_->setDisabled (true);
    }
}

// public methods ------------------------------------------------------------------------------------------
void MainWindow::onActionStart() {
    SwitchButtons(START);
    DefaultTableWgtInit();
    ui->s_bar->showMessage (INDEXING);
    if (indx_ptr_->CheckState () == PAUSE){
        indx_ptr_->SetState (START);
        indx_ptr_->Resume();
    }
    else{
        QThread* start_thread = new QThread;
        Controller* contr = new Controller(indx_ptr_);
        contr->moveToThread (start_thread);

        QObject::connect (start_thread, &QThread::started, contr, &Controller::onStartButtonClick, Qt::UniqueConnection);
        QObject::connect (contr, &Controller::finished, this, &MainWindow::ActionsAfterIndexing, Qt::UniqueConnection);
        QObject::connect (indx_ptr_, &Indexer::CurrDir, this, &MainWindow::ShowCurrDir, Qt::UniqueConnection);
        start_thread->start ();
    }
}

void MainWindow::onActionStop() {
    SwitchButtons(STOP);
    ui->s_bar->showMessage("Stopped...");
    QThread* stop_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (stop_thread);

    QObject::connect(stop_thread, &QThread::started, contr, &Controller::onStopButtonClick, Qt::UniqueConnection);

    stop_thread->start ();
}

void MainWindow::onActionPause() {
    SwitchButtons(PAUSE);
    ui->s_bar->showMessage ("Paused...");

    QThread* pause_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (pause_thread);

    QObject::connect(pause_thread, &QThread::started, contr, &Controller::onPauseButtonClick, Qt::UniqueConnection);

    pause_thread->start ();
}

void MainWindow::onActionSearch() {
    QString key = type_ == BY_DATE ? s_date_->text (): s_line_->text ();
    SwitchButtons(SEARCH);
    DefaultTableWgtInit();
    ui->s_bar->showMessage ("Searching...");
    QThread* search_thread = new QThread;
    Controller* contr = new Controller(indx_ptr_);
    contr->moveToThread (search_thread);

    QObject::connect (search_thread, &QThread::started, contr, [=] { contr->onSearchButtonClick(type_, key); }, Qt::UniqueConnection);
    QObject::connect (indx_ptr_, &Indexer::MessageSearchCount, this, &MainWindow::ActionsAfterSearch, Qt::UniqueConnection);
    QObject::connect (indx_ptr_, &Indexer::SendInfoToView, this, &MainWindow::DisplayFileInfo, Qt::UniqueConnection);
    search_thread->start ();
}

void MainWindow::ActionsAfterIndexing() {
    InitReadIndex();
    ui->s_bar->showMessage ("Count of dirs: " + QString::number(indx_ptr_->GetDirCount ()) +
                            ",    Count of objects: " + QString::number (indx_ptr_->GetObjectCount ()));
}

void MainWindow::ActionsAfterSearch (unsigned count){
    SwitchButtons(DEFAULT);
    ui->s_bar->showMessage ("Find " + QString::number(count) + " objects");
}

void MainWindow::ShowMessage(QString msg){
    if (msg == INDEX_IS_EMPTY || msg == INDEX_SUCCESS) {
        SwitchButtons(DEFAULT);
    }
    ui->s_bar->showMessage (msg);
}

void MainWindow::ShowCurrDir(QString path, unsigned count) {
    ui->s_bar->showMessage (INDEXING + " | " + QString::number (count) + " objects indexed | Current dir: " + path);
}

void MainWindow::DisplayFileInfo(FileInfo info) {
    table_wgt_->insertRow(table_wgt_->rowCount());
    table_wgt_->setItem(table_wgt_->rowCount() - 1, 0, new QTableWidgetItem(info.name));
    table_wgt_->setItem(table_wgt_->rowCount() - 1, 1, new QTableWidgetItem(info.extension));
    table_wgt_->setItem(table_wgt_->rowCount() - 1, 2, new QTableWidgetItem(info.size));
    table_wgt_->setItem(table_wgt_->rowCount() - 1, 3, new QTableWidgetItem(info.date));
    table_wgt_->setItem(table_wgt_->rowCount() - 1, 4, new QTableWidgetItem(info.path));
    if (!(table_wgt_->rowCount() % 1000)) {
        ui->s_bar->showMessage("Searching... " + QString::number(table_wgt_->rowCount()) + " objects already found...");
    }
}

void MainWindow::setSearchType(QString type) {
    if(type == NAME_STR) {
        type_ = BY_NAME;
        stacked_wgt_->setCurrentWidget (s_line_);
        CheckSearchLine (s_line_->text ());
    }
    if(type == EXTENSION_STR){
        type_ = BY_EXTENSION;
        stacked_wgt_->setCurrentWidget (s_line_);
        CheckSearchLine (s_line_->text ());
    }
    if(type == SIZE_STR){
        type_ = BY_SIZE;
        stacked_wgt_->setCurrentWidget (s_line_);
        CheckSearchLine (s_line_->text ());
    }
    if(type == DATE_STR){
        type_ = BY_DATE;
        stacked_wgt_->setCurrentWidget (s_date_);
        search_action_->setEnabled (true);
    }
}

void MainWindow::CheckSearchLine(QString text){
    if(text == "") {
        search_action_->setDisabled (true);
    }
    else {
        search_action_->setEnabled (true);
    }
}

void MainWindow::ShowDir(int row, int col) {
    QDesktopServices::openUrl (QUrl::fromLocalFile (table_wgt_->item(row, 4)->text ()));
}

void MainWindow::ShowAlloc(int row, int col) {
    QModelIndex indx = f_model_->index (table_wgt_->item(row, 4)->text ());
    tree_view_->setCurrentIndex (indx);
}

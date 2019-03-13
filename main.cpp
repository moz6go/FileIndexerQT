#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<FileInfo>("FileInfo");
    QApplication a(argc, argv);
    Indexer indx;
    MainWindow w(&indx);

    w.show();

    return a.exec();
}

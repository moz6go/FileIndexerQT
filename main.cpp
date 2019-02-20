#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if defined(_WIN32)
    std::locale::global(std::locale(""));
#endif
    qRegisterMetaType<FileInfo>("FileInfo");
    QApplication a(argc, argv);
    Indexer indx;
    MainWindow w(&indx);
    w.show();

    return a.exec();
}

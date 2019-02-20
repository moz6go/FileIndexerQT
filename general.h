#ifndef GENERAL
#define GENERAL

#include <QApplication>
#include <QtDebug>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMainWindow>
#include <QToolBar>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QDateEdit>
#include <QLabel>
#include <QHeaderView>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QTreeView>
#include <QSplitter>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QDesktopServices>

#include <sys/types.h>
#include <sys/stat.h>
#include <locale>
#include <string>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#if defined(_WIN32)
typedef std::wstring string_t;
typedef std::wofstream ofstream_t;
typedef std::wifstream ifstream_t;
typedef wchar_t char_t;
const int SIZE_WID = 24;
#else
typedef std::string string_t;
typedef std::ofstream ofstream_t;
typedef  std::ifstream ifstream_t;
typedef char char_t;
const int SIZE_WID = 32;
#endif
const QString INDEX_FILE = "index.xml";
const QString DIR_EXT = "DIR";

const QString HEADER_TAG = "<?xml version = \"1.0\"?>";
const QString REM_TAG = "<!-- Filesystem index -->";
const QString FS_OPEN_TAG = "<filesystem>";
const QString FS_CLOSE_TAG = "</filesystem>";

const QString OBJECT_OPEN_TAG = "<object path =\"";
const QString OBJECT_CLOSE_TAG_ATTR = "\">";
const QString OBJECT_CLOSE_TAG = "</object>";
const int OBJECT_OPEN_TAG_SIZE = 15;

const QString NAME_OPEN_TAG = "<name>";
const QString NAME_CLOSE_TAG = "</name>";
const int NAME_OPEN_TAG_SIZE = 6;

const QString EXT_OPEN_TAG = "<extension>";
const QString EXT_CLOSE_TAG = "</extension>";
const int EXT_OPEN_TAG_SIZE = 11;

const QString SIZE_OPEN_TAG = "<size>";
const QString SIZE_CLOSE_TAG = "</size>";
const int SIZE_OPEN_TAG_SIZE = 6;

const QString DATE_OPEN_TAG = "<date>";
const QString DATE_CLOSE_TAG = "</date>";
const int DATE_OPEN_TAG_SIZE = 6;
const QString NAME_STR = "Name";
const QString EXTENSION_STR = "Extension";
const QString SIZE_STR = "Size";
const QString DATE_STR = "Date";

#define S_TYPE QStringList() << NAME_STR << EXTENSION_STR << SIZE_STR << DATE_STR
#define EXTANSION_DEF node.isFile() ? node.suffix ().isEmpty ()? "Unknown" :node.suffix () : "DIR"
#define SIZE_DEF node.isFile() ? node.size() : 0

const QString INDEX_IS_EMPTY = "Index is empty";
const QString INDEX_SUCCESS = "Index was read successful";
const QString SEARCH_IN_FS = "Searching in filesystem...";
const QString INDEXING = "Indexing... Please wait...";

enum SearchType {
    ALL,
    BY_NAME,
    BY_EXTENSION,
    BY_DATE,
    BY_SIZE
};

enum Condition {
    DEFAULT = 1,
    START,
    STOP,
    PAUSE,
    SEARCH,
    DISABLED
};

struct FileInfo {
    QString name;
    QString path;
    QString extension;
    QString size;
    QString date;
};

#endif


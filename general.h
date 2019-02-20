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
    #include <Windows.h>
#else
    #include <dirent.h>
#endif


#if defined(_WIN32)
    typedef std::wstring string_t;
    typedef std::wofstream ofstream_t;
    typedef std::wifstream ifstream_t;
    typedef wchar_t char_t;

    const int SIZE_WID = 24;
    const string_t INDEX_FILE = L"index.xml";
    const string_t DIR_EXT = L"DIR";

    const string_t HEADER_TAG = L"<?xml version = \"1.0\"?>";
    const string_t REM_TAG = L"<!-- Filesystem index -->";
    const string_t FS_OPEN_TAG = L"<filesystem>";
    const string_t FS_CLOSE_TAG = L"</filesystem>";

    const string_t OBJECT_OPEN_TAG = L"<object path =\"";
    const string_t OBJECT_CLOSE_TAG_ATTR = L"\">";
    const string_t OBJECT_CLOSE_TAG = L"</object>";
    const size_t OBJECT_OPEN_TAG_SIZE = 15;

    const string_t NAME_OPEN_TAG = L"<name>";
    const string_t NAME_CLOSE_TAG = L"</name>";
    const size_t NAME_OPEN_TAG_SIZE = 6;

    const string_t EXT_OPEN_TAG = L"<extension>";
    const string_t EXT_CLOSE_TAG = L"</extension>";
    const size_t EXT_OPEN_TAG_SIZE = 11;

    const string_t SIZE_OPEN_TAG = L"<size>";
    const string_t SIZE_CLOSE_TAG = L"</size>";
    const size_t SIZE_OPEN_TAG_SIZE = 6;

    const string_t DATE_OPEN_TAG = L"<date>";
    const string_t DATE_CLOSE_TAG = L"</date>";
    const size_t DATE_OPEN_TAG_SIZE = 6;
#else
    typedef std::string string_t;
    typedef std::ofstream ofstream_t;
    typedef  std::ifstream ifstream_t;
    typedef char char_t;

    const int SIZE_WID = 32;
    const QString INDEX_FILE = "index.xml";
    const QString DIR_EXT = "DIR";

    const QString HEADER_TAG = "<?xml version = \"1.0\"?>";
    const QString REM_TAG = "<!-- Filesystem index -->";
    const QString FS_OPEN_TAG = "<filesystem>";
    const QString FS_CLOSE_TAG = "</filesystem>";

    const QString OBJECT_OPEN_TAG = "<object path =\"";
    const QString OBJECT_CLOSE_TAG_ATTR = "\">";
    const QString OBJECT_CLOSE_TAG = "</object>";
    const size_t OBJECT_OPEN_TAG_SIZE = 15;

    const QString NAME_OPEN_TAG = "<name>";
    const QString NAME_CLOSE_TAG = "</name>";
    const size_t NAME_OPEN_TAG_SIZE = 6;

    const QString EXT_OPEN_TAG = "<extension>";
    const QString EXT_CLOSE_TAG = "</extension>";
    const size_t EXT_OPEN_TAG_SIZE = 11;

    const QString SIZE_OPEN_TAG = "<size>";
    const QString SIZE_CLOSE_TAG = "</size>";
    const size_t SIZE_OPEN_TAG_SIZE = 6;

    const QString DATE_OPEN_TAG = "<date>";
    const QString DATE_CLOSE_TAG = "</date>";
    const size_t DATE_OPEN_TAG_SIZE = 6;


#endif

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


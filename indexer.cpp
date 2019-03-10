#include "indexer.h"

Indexer::Indexer() : count_(0),
    c_dir_(1),
    search_res_count_(0),
    search_in_fs_(false),
    type_(ALL),
    indx_(INDEX_FILE) {}

Indexer::~Indexer() {}

int Indexer::SearchMain(){
    if(type_ == ALL){
        f_list_.clear ();
        xml_doc_.clear();
    }
    else {
        search_res_count_ = 0;
    }

    foreach(QFileInfo drive, QDir::drives()) {
        RecursiveSearchFiles (drive.dir());
        if(CheckState() == STOP) break;
    }
    return search_res_count_;
}

void Indexer::WriteFullIndex() {
    int progress = 0;
    count_ = f_list_.size ();
    if(indx_.open(QIODevice::WriteOnly)) {
        QTextStream fout(&indx_);
        fout << HEADER_TAG << REM_TAG << FS_OPEN_TAG;
        for(const auto& file_info : f_list_) {
            fout << OBJECT_OPEN_TAG << file_info.path << OBJECT_CLOSE_TAG_ATTR <<
                    NAME_OPEN_TAG << file_info.name << NAME_CLOSE_TAG <<
                    EXT_OPEN_TAG << file_info.extension  << EXT_CLOSE_TAG <<
                    SIZE_OPEN_TAG <<file_info.size << SIZE_CLOSE_TAG <<
                    DATE_OPEN_TAG << file_info.date << DATE_CLOSE_TAG <<
                    OBJECT_CLOSE_TAG;
            emit SendCount (++progress);
        }
        fout << FS_CLOSE_TAG;
        indx_.close ();
        fout.flush();
        f_list_.clear ();
        f_list_.squeeze ();
    }
}

void Indexer::ReadIndex() {
    if (indx_.open(QIODevice::ReadOnly)) {
        QTextStream fin(&indx_);
        xml_doc_.clear();
        xml_doc_ = fin.readAll ();
        count_ = xml_doc_.count(OBJECT_CLOSE_TAG);
        indx_.close();
    }
    else {
        count_ = 0;
    }
}

int Indexer::SearchInIndx()  {
    QString open_tag, close_tag;

    int open_tag_size = NAME_OPEN_TAG_SIZE;
    FileInfo f_info;

    switch (type_) {
    case BY_NAME:
        open_tag = NAME_OPEN_TAG;
        close_tag = NAME_CLOSE_TAG;
        open_tag_size = NAME_OPEN_TAG_SIZE;
        break;
    case BY_EXTENSION:
        open_tag = EXT_OPEN_TAG;
        close_tag = EXT_CLOSE_TAG;
        open_tag_size = EXT_OPEN_TAG_SIZE;
        break;
    case BY_DATE:
        open_tag = DATE_OPEN_TAG;
        close_tag = DATE_CLOSE_TAG;
        open_tag_size = DATE_OPEN_TAG_SIZE;
        break;
    case BY_SIZE:
        open_tag = SIZE_OPEN_TAG;
        close_tag = SIZE_CLOSE_TAG;
        open_tag_size = SIZE_OPEN_TAG_SIZE;
        break;
    case ALL:;
    }

    if(xml_doc_.size()) {
        int pos = 0;
        do {
            if ((xml_doc_.indexOf (open_tag, pos) + open_tag_size) > pos) {

                if ( Compare( key_, comp_type_, xml_doc_.mid(xml_doc_.indexOf(open_tag, pos) + open_tag_size,
                                  xml_doc_.indexOf(close_tag, pos) - (xml_doc_.indexOf(open_tag, pos) + open_tag_size)))) {
                    //store FileInfo and send signal to view

                    f_info.path = xml_doc_.mid (xml_doc_.indexOf(OBJECT_OPEN_TAG, pos) + OBJECT_OPEN_TAG_SIZE,
                                                 xml_doc_.indexOf(OBJECT_CLOSE_TAG_ATTR, pos) - (xml_doc_.indexOf(OBJECT_OPEN_TAG, pos) + OBJECT_OPEN_TAG_SIZE));

                    f_info.name = xml_doc_.mid(xml_doc_.indexOf(NAME_OPEN_TAG, pos) +  NAME_OPEN_TAG_SIZE,
                                                 xml_doc_.indexOf(NAME_CLOSE_TAG, pos) - (xml_doc_.indexOf(NAME_OPEN_TAG, pos) + NAME_OPEN_TAG_SIZE));

                    f_info.extension = xml_doc_.mid(xml_doc_.indexOf(EXT_OPEN_TAG, pos) + EXT_OPEN_TAG_SIZE,
                                                 xml_doc_.indexOf(EXT_CLOSE_TAG, pos) - (xml_doc_.indexOf(EXT_OPEN_TAG, pos) + EXT_OPEN_TAG_SIZE));

                    f_info.size = xml_doc_.mid(xml_doc_.indexOf(SIZE_OPEN_TAG, pos) + SIZE_OPEN_TAG_SIZE,
                                                 xml_doc_.indexOf(SIZE_CLOSE_TAG, pos) - (xml_doc_.indexOf(SIZE_OPEN_TAG, pos) + SIZE_OPEN_TAG_SIZE));


                    f_info.date = xml_doc_.mid(xml_doc_.indexOf(DATE_OPEN_TAG, pos) + DATE_OPEN_TAG_SIZE,
                                                 xml_doc_.indexOf(DATE_CLOSE_TAG, pos) - (xml_doc_.indexOf(DATE_OPEN_TAG, pos) + DATE_OPEN_TAG_SIZE));

                    if (isObjExist(f_info)){
                        ++search_res_count_;
                        emit SendInfoToView(f_info);
                    }
                }
                pos = xml_doc_.indexOf(OBJECT_CLOSE_TAG, pos) + 1;
            }
            else {
                pos = xml_doc_.size();
            }

            if(CheckState() == STOP) {
                return search_res_count_;
            }
        } while (pos < xml_doc_.size());
    }

    return search_res_count_;
}

bool Indexer::Compare(QString& key, CompareType& comp, QString text) {
    switch (comp) {
    case EQUAL:
        return key == text ? true : false;
    case NOT_EQUAL:
        return key != text ? true : false;
    case CONTAINS:
        return text.contains (key) ? true : false;
    case LESS:
        return type_ == BY_DATE ? QDate::fromString (text, "dd.MM.yyyy") < QDate::fromString (key, "dd.MM.yyyy") ? true : false :
                type_ == BY_SIZE ? text.toUInt() < key.toUInt() ? true : false : false;
    case GREATER:
        return type_ == BY_DATE ? QDate::fromString (text, "dd.MM.yyyy") > QDate::fromString (key, "dd.MM.yyyy") ? true : false :
                type_ == BY_SIZE ? text.toUInt() > key.toUInt() ? true : false : false;
    case LESS_EQUAL:
        return type_ == BY_DATE ? QDate::fromString (text, "dd.MM.yyyy") <= QDate::fromString (key, "dd.MM.yyyy") ? true : false :
                type_ == BY_SIZE ? text.toUInt() <= key.toUInt() ? true : false : false;
    case GREATER_EQUAL:
        return type_ == BY_DATE ? QDate::fromString (text, "dd.MM.yyyy") >= QDate::fromString (key, "dd.MM.yyyy") ? true : false :
                type_ == BY_SIZE ? text.toUInt() >= key.toUInt() ? true : false : false;
    }
    return false;
}

void Indexer::SetSearchType(SearchType type){
    type_ = type;
}

void Indexer::SetKey(QString key){
    key_ = key;
}

void Indexer::SetCompareType(CompareType comp){
    comp_type_ = comp;
}

void Indexer::SetCount(int c_dir, int c_obj, int c_seacrh) {
    c_dir_ = c_dir;
    count_ = c_obj;
    search_res_count_ = c_seacrh;
}

void Indexer::SetSerchInFs(bool search){
    search_in_fs_ = search;
}

int Indexer::GetObjectCount() const {
    return count_;
}

int Indexer::GetDirCount() const {
    return c_dir_;
}

bool Indexer::SearchInFs(){
    return search_in_fs_;
}

void Indexer::RecursiveSearchFiles(const QDir& dir) {
    if(CheckState() == STOP) return;
    CheckPause();

    if(!(c_dir_ % 128)) {
        emit (type_ == ALL ? SendCurrDir(dir.absolutePath (), f_list_.size ()) : SendCurrDir(dir.absolutePath (), search_res_count_));
    }


    QFileInfoList listDir = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (QFileInfo subdir, listDir) {
        ++c_dir_;
        RecursiveSearchFiles(QDir(dir.absoluteFilePath(subdir.absoluteFilePath ())));
    }


    foreach (QFileInfo file_info, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::System)) {
        FileInfo curr_file_info;
        curr_file_info.name = file_info.fileName ();
        curr_file_info.extension = file_info.isFile() ? file_info.suffix ().isEmpty ()? "Unknown" : file_info.suffix () : "DIR";
        curr_file_info.size = QString::number (file_info.isFile() ? file_info.size() : 0);
        curr_file_info.date = file_info.lastModified ().toString ("dd.MM.yyyy");
        curr_file_info.path = file_info.absoluteFilePath ();

        switch(type_) {
        case ALL:
            f_list_.push_back(curr_file_info);
            break;
        case BY_NAME:
            if (Compare(key_, comp_type_, curr_file_info.name)) {
                emit SendInfoToView(curr_file_info);
                ++search_res_count_;
            }
            break;
        case BY_EXTENSION:
            if (Compare(key_, comp_type_,curr_file_info.extension)){
                emit SendInfoToView(curr_file_info);
                ++search_res_count_;
            }
            break;
        case BY_SIZE:
            if (Compare(key_, comp_type_, curr_file_info.size)){
                emit SendInfoToView(curr_file_info);
                ++search_res_count_;
            }
            break;
        case BY_DATE:
            if (Compare(key_, comp_type_, curr_file_info.date)) {
                emit SendInfoToView(curr_file_info);
                ++search_res_count_;
            }
            break;
        }
    }
}


void Indexer::WriteIndexHead() {
    if(indx_.open(QIODevice::WriteOnly)){
        QTextStream fout(&indx_);
        fout << HEADER_TAG << REM_TAG << FS_OPEN_TAG;
        indx_.close ();
    }
}

void Indexer::WriteIndexTail() {
    if(indx_.open(QIODevice::WriteOnly | QIODevice::Append)){
        QTextStream fout(&indx_);
        fout << FS_CLOSE_TAG;
        indx_.close ();
    }
}

void Indexer::WriteIndexNode(QFileInfoList file_list) {
    if(indx_.open(QIODevice::WriteOnly | QIODevice::Append)){
        QTextStream fout(&indx_);
        for(const auto& file_info : file_list) {
            fout << OBJECT_OPEN_TAG << file_info.absoluteFilePath () << OBJECT_CLOSE_TAG_ATTR <<
                    NAME_OPEN_TAG << file_info.fileName () << NAME_CLOSE_TAG <<
                    EXT_OPEN_TAG << (file_info.isFile() ? file_info.suffix ().isEmpty ()? "Unknown" : file_info.suffix () : "DIR") << EXT_CLOSE_TAG <<
                    SIZE_OPEN_TAG << QString::number (file_info.isFile() ? file_info.size() : 0) << SIZE_CLOSE_TAG <<
                    DATE_OPEN_TAG << file_info.lastModified ().toString ("dd.MM.yyyy") << DATE_CLOSE_TAG <<
                    OBJECT_CLOSE_TAG;
            ++count_;
        }
        indx_.close ();
    }
}

bool Indexer::isObjExist(FileInfo& f_info) {
    return f_info.extension == DIR_EXT ? QDir(f_info.path).exists () : QFileInfo(f_info.path).exists ();
}

#include "indexer.h"

Indexer::Indexer() : count_(0),
    c_dir_(1),
    search_res_count_(0),
    type_(ALL),
    indx_(INDEX_FILE) {}

Indexer::~Indexer() {}

void Indexer::WriteIndex(){
    QTime t = QTime::currentTime ();
    type_ = ALL;
    WriteIndexHead();
    foreach(QFileInfo drive, QDir::drives()) {
        RecursiveSearchFiles (drive.dir());
        if(CheckState() == STOP) return;
    }
//    RecursiveSearchFiles (QDir("/home/myroslav/Документи/Repos/ShoesStore"));
    WriteIndexTail();
    qDebug() << t.elapsed();
}

void Indexer::ReadIndex() {
    if (indx_.open(QIODevice::ReadOnly)) {
        QTextStream fin(&indx_);
        xml_doc_.clear();
        xml_doc_ = fin.readAll ();
        if(xml_doc_.size()) {
            emit Message(INDEX_SUCCESS + ", " + QString::number(xml_doc_.count(OBJECT_CLOSE_TAG)) + " objects in index");
        }
        else {
            emit Message(INDEX_IS_EMPTY);
        }
        indx_.close();
    }
    else {
        emit Message(INDEX_IS_EMPTY);
    }
}

void Indexer::Search(SearchType type, QString key)  {
    key_ = key;
    type_ = type;
    search_res_count_ = 0;
    QString open_tag, close_tag;

    int open_tag_size = NAME_OPEN_TAG_SIZE;
    FileInfo f_info;

    switch (type) {
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
                if (xml_doc_.mid(xml_doc_.indexOf(open_tag, pos) + open_tag_size,
                                  xml_doc_.indexOf(close_tag, pos) - (xml_doc_.indexOf(open_tag, pos) + open_tag_size)) == key) {
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

            if(state_ == STOP) {
                emit MessageSearchCount(search_res_count_);
                return;
            }
        } while (pos < xml_doc_.size());

    }
    if(!search_res_count_) {
        emit Message(SEARCH_IN_FS);
        foreach(QFileInfo drive, QDir::drives()) {
            RecursiveSearchFiles (drive.dir());
            if(CheckState() == STOP) return;
        }
    }
    emit MessageSearchCount(search_res_count_);
}



unsigned Indexer::GetObjectCount() const {
    return count_;
}



unsigned Indexer::GetDirCount() const {
    return c_dir_;
}



void Indexer::SetCount(unsigned c_dir, unsigned c_obj) {
    c_dir_ = c_dir;
    count_ = c_obj;
}



// private methods-------------------------------------------------------------------------------------------
void Indexer::RecursiveSearchFiles(const QDir& dir) {
    if(!(count_ % 128)) {
        emit CurrDir(dir.absolutePath (), count_);
    }

    QFileInfoList listDir = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (QFileInfo subdir, listDir) {
        ++c_dir_;
        RecursiveSearchFiles(QDir(dir.absoluteFilePath(subdir.absoluteFilePath ())));
    }

    if(CheckState() == STOP) return;
    CheckPause();

    if (type_ == ALL) {
        WriteIndexNode(dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::System));
    }
    else {
        foreach (QFileInfo file_info, dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::System)) {
            FileInfo curr_file_info;
            curr_file_info.name = file_info.fileName ();
            curr_file_info.extension = file_info.isFile() ? file_info.suffix ().isEmpty ()? "Unknown" : file_info.suffix () : "DIR";
            curr_file_info.size = QString::number (file_info.isFile() ? file_info.size() : 0);
            curr_file_info.date = file_info.lastModified ().toString ("dd.MM.yyyy");
            curr_file_info.path = file_info.absoluteFilePath ();

            switch(type_) {
            case ALL:
            case BY_NAME:
                if (key_ == curr_file_info.name){
                    emit SendInfoToView(curr_file_info);
                    ++search_res_count_;
                }
                break;
            case BY_EXTENSION:
                if (key_ == curr_file_info.extension){
                    emit SendInfoToView(curr_file_info);
                    ++search_res_count_;
                }
                break;
            case BY_SIZE:
                if (key_ == curr_file_info.size){
                    emit SendInfoToView(curr_file_info);
                    ++search_res_count_;
                }
                break;
            case BY_DATE:
                if (key_ == curr_file_info.date) {
                    emit SendInfoToView(curr_file_info);
                    ++search_res_count_;
                }
                break;
            }
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
    if(f_info.extension == DIR_EXT){
        return QDir(f_info.path).exists ();
    }
    else {
        return QFileInfo(f_info.path).exists ();
    }
}

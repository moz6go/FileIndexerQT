#ifndef SERCHFILES_H
#define SERCHFILES_H

#include "statechecker.h"

class Indexer : public StateChecker {
    Q_OBJECT
    unsigned count_;
    unsigned c_dir_;
    unsigned search_res_count_;
    SearchType type_;
    QString key_;
    QString xml_doc_;
    QFile indx_;
    QTextStream fout_;


    void RecursiveSearchFiles(const QDir& dir);
    void WriteIndexHead();
    void WriteIndexTail();
    void WriteIndexNode(QFileInfoList file_list);
    bool isObjExist(FileInfo& f_info);
public:
    Indexer();
    ~Indexer();
    void WriteIndex();
    void ReadIndex();
    void Search(SearchType type, QString key);
    void SetCount(unsigned c_dir, unsigned c_obj);
    unsigned GetObjectCount() const;
    unsigned GetDirCount() const;

signals:
    void Message(QString mes);
    void MessageSearchCount(unsigned count);
    void CurrDir(QString path, unsigned count);
    void SendInfoToView(FileInfo f_info);
};

#endif // SERCHFILES_H

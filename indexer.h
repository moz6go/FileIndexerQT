#ifndef SERCHFILES_H
#define SERCHFILES_H

#include "statechecker.h"

class Indexer : public StateChecker {
    Q_OBJECT
    unsigned count_;
    unsigned c_dir_;
    unsigned search_res_count_;
    bool search_in_fs_;
    SearchType type_;
    CompareType comp_type_;
    QString key_;
    QString xml_doc_;
    QFile indx_;
    QFileInfoList f_list_;

    void RecursiveSearchFiles(const QDir& dir);
    void WriteIndexHead();
    void WriteIndexTail();
    void WriteIndexNode(QFileInfoList file_list);
    bool isObjExist(FileInfo& f_info);
    bool Compare(QString& key, CompareType& comp, QString text);
public:
    Indexer();
    ~Indexer();
    unsigned SearchMain();
    void WriteFullIndex();
    void ReadIndex();
    unsigned SearchInIndx();
    void SetCount(unsigned c_dir, unsigned c_obj, unsigned c_seacrh);
    void SetSerchInFs(bool search);
    void SetSearchType(SearchType type);
    void SetKey(QString key);
    void SetCompareType(CompareType comp);
    unsigned GetObjectCount() const;
    unsigned GetDirCount() const;
    bool SearchInFs();
signals:
    void SendMessage(QString mes);
    void SendReadResult(QString mes);
    void SendCurrDir(QString path, int count);
    void SendInfoToView(FileInfo f_info);
};

#endif // SERCHFILES_H

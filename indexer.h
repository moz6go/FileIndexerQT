#ifndef SERCHFILES_H
#define SERCHFILES_H

#include "statechecker.h"

class Indexer : public StateChecker {
    Q_OBJECT
    int count_;
    int c_dir_;
    int search_res_count_;
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
    int SearchMain();
    void WriteFullIndex();
    void ReadIndex();
    int SearchInIndx();

    void SetCount(int c_dir, int c_obj, int c_seacrh);
    void SetSerchInFs(bool search);
    void SetSearchType(SearchType type);
    void SetKey(QString key);
    void SetCompareType(CompareType comp);

    int GetObjectCount() const;
    int GetDirCount() const;
    bool SearchInFs();
signals:
    void SendCurrDir(QString path, int count);
    void SendInfoToView(FileInfo f_info);
    void SendCount(int count);
};

#endif // SERCHFILES_H

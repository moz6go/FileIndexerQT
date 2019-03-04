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
    void WriteFullIndex();
    bool isObjExist(FileInfo& f_info);
    bool Compare(QString& key, CompareType& comp, QString text);
public:
    Indexer();
    ~Indexer();
    void Index();
    void ReadIndex();
    void Search(SearchType type, CompareType comp, QString key);
    void SetCount(unsigned c_dir, unsigned c_obj);
    void SetSerchInFs(bool search);
    unsigned GetObjectCount() const;
    unsigned GetDirCount() const;
signals:
    void Message(QString mes);
    void MessageSearchCount(unsigned count);
    void CurrDir(QString path, int count);
    void SendInfoToView(FileInfo f_info);
    void CallMsgBox(unsigned count);
};

#endif // SERCHFILES_H

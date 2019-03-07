#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "indexer.h"

class Controller : public StateChecker {
    Q_OBJECT
    Indexer* indx_ptr_;
public:
    Controller (Indexer* indx_ptr);
public slots:
    void onStartButtonClick();
    void onStopButtonClick();
    void onPauseButtonClick();
    void onSearchButtonClick(SearchType key, CompareType comp, QString value);
    void ReadIndex ();
signals:
    void SendReadResult(QString mes);
    void SendSearchResCount(int count);
    void CallMsgBox(int count);
    void IndexWriteStarted(QString mes);
    void finished();
};

#endif // CONTROLLER_H

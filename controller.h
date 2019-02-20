#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "indexer.h"

class Controller : public QObject {
    Q_OBJECT
    Indexer* indx_ptr_;
public:
    Controller (Indexer* indx_ptr);
public slots:
    void onStartButtonClick();
    void onStopButtonClick();
    void onPauseButtonClick();
    void onSearchButtonClick(SearchType key, QString value);
    void ReadIndex ();
signals:
    void finished();
};

#endif // CONTROLLER_H

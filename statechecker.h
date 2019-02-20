#ifndef STATECHECKER_H
#define STATECHECKER_H

#include "general.h"

class StateChecker : public QObject
{
protected:
    QMutex mtx_;
    QWaitCondition condition_;
    volatile Condition state_;
public:
    StateChecker();
    Condition CheckState();
    void SetState(Condition state);
    void CheckPause();
    void Resume();
};

#endif // STATECHECKER_H

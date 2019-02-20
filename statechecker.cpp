#include "statechecker.h"

StateChecker::StateChecker(): state_(DEFAULT) {}

Condition StateChecker::CheckState() {
    QMutexLocker locker(&mtx_);
    return state_;
}

void StateChecker::SetState(Condition state) {
    QMutexLocker locker(&mtx_);
    state_ = state;
}
void StateChecker::CheckPause() {
    QMutexLocker locker(&mtx_);
    while (state_ == PAUSE){
        condition_.wait(&mtx_);
    }
}

void StateChecker::Resume() {
    condition_.wakeOne();
}

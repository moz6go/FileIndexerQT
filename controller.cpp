#include "controller.h"

Controller::Controller (Indexer* indx_ptr) : indx_ptr_(indx_ptr) {}

void Controller::onStartButtonClick() {
    indx_ptr_->SetCount(0, 0);
    indx_ptr_->SetState (START);
    indx_ptr_->WriteIndex ();
    emit finished ();
}

void Controller::onStopButtonClick() {
    indx_ptr_->SetState (STOP);
    indx_ptr_->Resume();
}

void Controller::onPauseButtonClick() {
    indx_ptr_->SetState (PAUSE);
}

void Controller::onSearchButtonClick(SearchType key, CompareType comp, QString value) {
    indx_ptr_->SetState (SEARCH);
    indx_ptr_->Search (key, comp, value);
}

void Controller::ReadIndex (){
    indx_ptr_->ReadIndex ();
}


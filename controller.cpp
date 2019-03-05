#include "controller.h"

Controller::Controller (Indexer* indx_ptr) : indx_ptr_(indx_ptr) {}

void Controller::onStartButtonClick() {
    indx_ptr_->SetCount(0, 0, 0);
    indx_ptr_->SetState (START);
    indx_ptr_->SetSearchType (ALL);

    indx_ptr_->SearchMain ();

    emit IndexWriteStarted ("2/2 Writing results of indexing to file... Please wait...");
    indx_ptr_->WriteFullIndex ();

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
    indx_ptr_->SetCount(0, 0, 0);
    indx_ptr_->SetState (SEARCH);
    indx_ptr_->SetSearchType (key);
    indx_ptr_->SetCompareType (comp);
    indx_ptr_->SetKey (value);
    unsigned res = indx_ptr_->SearchInIndx ();
    emit SendSearchResCount(res);
    emit CallMsgBox (res);
    indx_ptr_->SetState (PAUSE);
    indx_ptr_->CheckPause ();

    if(indx_ptr_->SearchInFs ()){
        res = indx_ptr_->SearchMain ();
    }
    emit SendSearchResCount(res);
}

void Controller::ReadIndex (){
    indx_ptr_->ReadIndex ();
}


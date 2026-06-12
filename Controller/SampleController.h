#pragma once
#include "../Model/ISampleModel.h"
#include "../View/ISampleView.h"

class SampleController {
public:
    SampleController(ISampleModel& model, ISampleView& view);
    void run();

private:
    void handleRegister();
    void handleListAll();
    void handleSearch();

    ISampleModel& model_;
    ISampleView&  view_;
};

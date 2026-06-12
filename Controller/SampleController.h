#pragma once
#include "ISampleController.h"
#include "../Model/ISampleModel.h"
#include "../View/ISampleView.h"

class SampleController : public ISampleController {
public:
    SampleController(ISampleModel& model, ISampleView& view);
    void run() override;

private:
    void handleRegister();
    void handleListAll();
    void handleSearch();

    ISampleModel& model_;
    ISampleView&  view_;
};

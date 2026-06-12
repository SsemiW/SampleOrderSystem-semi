#pragma once
#include "ISampleView.h"

class SampleView : public ISampleView {
public:
    void        showSampleMenu()                                   override;
    int         getMenuChoice()                                    override;
    void        showSampleList(const std::vector<Sample>& samples) override;
    Sample      promptNewSample()                                  override;
    std::string promptSearchKeyword()                              override;
    void        showMessage(const std::string& msg)                override;
};

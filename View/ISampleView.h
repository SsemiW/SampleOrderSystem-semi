#pragma once
#include <vector>
#include <string>
#include "../Model/Sample.h"

class ISampleView {
public:
    virtual ~ISampleView() = default;
    virtual void        showSampleMenu()                                  = 0;
    virtual int         getMenuChoice()                                   = 0;
    virtual void        showSampleList(const std::vector<Sample>& samples) = 0;
    virtual Sample      promptNewSample()                                 = 0;
    virtual std::string promptSearchKeyword()                             = 0;
    virtual void        showMessage(const std::string& msg)               = 0;
};

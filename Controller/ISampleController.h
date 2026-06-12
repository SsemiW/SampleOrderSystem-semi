#pragma once

class ISampleController {
public:
    virtual ~ISampleController() = default;
    virtual void run() = 0;
};

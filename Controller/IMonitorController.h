#pragma once

class IMonitorController {
public:
    virtual ~IMonitorController() = default;
    virtual void run() = 0;
};

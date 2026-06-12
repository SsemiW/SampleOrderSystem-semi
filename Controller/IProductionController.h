#pragma once

class IProductionController {
public:
    virtual ~IProductionController() = default;
    virtual void run() = 0;
};

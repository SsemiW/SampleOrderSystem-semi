#pragma once

class IOrderController {
public:
    virtual ~IOrderController() = default;
    virtual void runReserve()  = 0;
    virtual void runApproval() = 0;
};

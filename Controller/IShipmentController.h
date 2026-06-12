#pragma once

class IShipmentController {
public:
    virtual ~IShipmentController() = default;
    virtual void run() = 0;
};

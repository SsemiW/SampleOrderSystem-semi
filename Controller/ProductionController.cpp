#include "ProductionController.h"

ProductionController::ProductionController(IProductionModel& prodModel,
                                           IOrderModel&      orderModel,
                                           ISampleModel&     sampleModel,
                                           IProductionView&  view)
    : prodModel_(prodModel)
    , orderModel_(orderModel)
    , sampleModel_(sampleModel)
    , view_(view)
{}

void ProductionController::run() {
    view_.render(prodModel_.currentJob(),
                 prodModel_.currentProducedAmount(),
                 prodModel_.waitingQueue());

    if (view_.isCompleteRequested() && prodModel_.isCurrentComplete()) {
        auto job = prodModel_.dequeue();
        sampleModel_.updateStock(job.sampleId, job.requiredAmount);
        orderModel_.updateStatus(job.orderId, OrderStatus::Confirmed);
    }
}

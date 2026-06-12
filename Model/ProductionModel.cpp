#include "ProductionModel.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

ProductionModel::ProductionModel(IRepository<ProductionJob>& repo) : repo_(repo) {}

void ProductionModel::enqueue(ProductionJob job) {
    auto all = repo_.findAll();
    job.queuePosition = static_cast<int>(all.size());
    if (job.queuePosition == 0)
        job.startedAt = nowString();
    repo_.create(job);
}

std::optional<ProductionJob> ProductionModel::currentJob() const {
    for (const auto& j : repo_.findAll())
        if (j.queuePosition == 0) return j;
    return std::nullopt;
}

std::vector<ProductionJob> ProductionModel::waitingQueue() const {
    auto all = repo_.findAll();
    std::vector<ProductionJob> waiting;
    for (const auto& j : all)
        if (j.queuePosition >= 1) waiting.push_back(j);
    std::sort(waiting.begin(), waiting.end(),
              [](const ProductionJob& a, const ProductionJob& b){
                  return a.queuePosition < b.queuePosition;
              });
    return waiting;
}

int ProductionModel::currentProducedAmount() const {
    auto job = currentJob();
    if (!job || job->startedAt.empty()) return 0;
    double elapsed = elapsedMinutes(job->startedAt);
    int produced = static_cast<int>(elapsed / job->avgProdTimeMin);
    return std::min(produced, job->requiredAmount);
}

bool ProductionModel::isCurrentComplete() const {
    auto job = currentJob();
    if (!job) return false;
    return currentProducedAmount() >= job->requiredAmount;
}

ProductionJob ProductionModel::dequeue() {
    auto job = currentJob();
    repo_.remove(job->id);

    auto remaining = repo_.findAll();
    for (auto r : remaining) {
        r.queuePosition--;
        if (r.queuePosition == 0)
            r.startedAt = nowString();
        repo_.update(r);
    }
    return *job;
}

std::string ProductionModel::nowString() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

std::time_t ProductionModel::parseTime(const std::string& s) {
    std::tm tm{};
    sscanf_s(s.c_str(), "%d-%d-%d %d:%d:%d",
             &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
             &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon  -= 1;
    tm.tm_isdst = -1;
    return std::mktime(&tm);
}

double ProductionModel::elapsedMinutes(const std::string& startedAt) {
    return std::difftime(std::time(nullptr), parseTime(startedAt)) / 60.0;
}

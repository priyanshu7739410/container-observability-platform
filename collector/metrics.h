#pragma once
#include <string>
#include <filesystem>
#include <vector>

struct ContainerMetrics
{
    std::string id;
    long memory_current;
    std::string memory_limit;
    long pids;
    long cpu_usage_usec;
};

ContainerMetrics collect_metrics(const std::string& container_id);
std::vector<ContainerMetrics> collect_all_metrics(const std::vector<std::string>& containers);

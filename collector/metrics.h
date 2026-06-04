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
    double cpu_percent; // Calculated active CPU usage percentage
    long uptime_seconds; // Uptime of the container process
    std::string cpu_stat_first_line; // First line of cpu.stat
};

ContainerMetrics collect_metrics(const std::string& container_id);
std::vector<ContainerMetrics> collect_all_metrics(const std::vector<std::string>& containers);

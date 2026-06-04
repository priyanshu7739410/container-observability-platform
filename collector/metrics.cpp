#include "metrics.h"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <chrono>

namespace fs = std::filesystem;

struct CpuSample
{
    long usage_usec;
    std::chrono::steady_clock::time_point timestamp;
};

static std::unordered_map<std::string, CpuSample> g_cpu_history;

static long read_metric(const std::string& path)
{
    std::ifstream file(path);
    long value = 0;
    if (file)
        file >> value;
    return value;
}

static std::string read_text(const std::string& path)
{
    std::ifstream file(path);
    std::string value;
    if (file)
        file >> value;
    return value;
}

static long read_cpu_usage(const std::string& path)
{
    std::ifstream file(path);
    std::string key;
    long value = 0;
    if (file)
        file >> key >> value;
    return value;
}

ContainerMetrics collect_metrics(const std::string& container_id)
{
    ContainerMetrics metrics;
    metrics.id = container_id;

    fs::path base_path = fs::path("/sys/fs/cgroup") / container_id;

    metrics.memory_current = read_metric((base_path / "memory.current").string());
    metrics.memory_limit = read_text((base_path / "memory.max").string());
    metrics.pids = read_metric((base_path / "pids.current").string());
    metrics.cpu_usage_usec = read_cpu_usage((base_path / "cpu.stat").string());

    auto now = std::chrono::steady_clock::now();
    metrics.cpu_percent = 0.0;

    auto it = g_cpu_history.find(container_id);
    if (it != g_cpu_history.end())
    {
        long delta_cpu = metrics.cpu_usage_usec - it->second.usage_usec;
        auto delta_time = std::chrono::duration_cast<std::chrono::microseconds>(now - it->second.timestamp).count();
        if (delta_time > 0 && delta_cpu >= 0)
        {
            metrics.cpu_percent = (static_cast<double>(delta_cpu) / delta_time) * 100.0;
        }
    }

    g_cpu_history[container_id] = CpuSample{metrics.cpu_usage_usec, now};

    return metrics;
}

std::vector<ContainerMetrics> collect_all_metrics(const std::vector<std::string>& containers)
{
    std::vector<ContainerMetrics> all_metrics;
    all_metrics.reserve(containers.size());
    for (const auto& id : containers)
    {
        all_metrics.push_back(collect_metrics(id));
    }
    return all_metrics;
}

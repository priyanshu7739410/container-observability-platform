#include "metrics.h"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <sstream>
#include <unistd.h>

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

static std::string read_first_line(const std::string& path)
{
    std::ifstream file(path);
    std::string line;
    if (file)
        std::getline(file, line);
    return line;
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

static long get_uptime(const std::string& container_id)
{
    std::string procs_path = (fs::path("/sys/fs/cgroup") / container_id / "cgroup.procs").string();
    std::ifstream procs_file(procs_path);
    std::string pid_str;
    if (!procs_file || !(procs_file >> pid_str))
    {
        return -1;
    }

    std::string stat_path = "/proc/" + pid_str + "/stat";
    std::ifstream stat_file(stat_path);
    std::string stat_line;
    if (!stat_file || !std::getline(stat_file, stat_line))
    {
        return -1;
    }

    size_t last_paren = stat_line.rfind(')');
    if (last_paren == std::string::npos || last_paren + 2 >= stat_line.size())
    {
        return -1;
    }

    std::string remainder = stat_line.substr(last_paren + 2);
    std::stringstream ss(remainder);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token)
    {
        tokens.push_back(token);
    }

    if (tokens.size() < 20)
    {
        return -1;
    }

    long starttime = 0;
    try
    {
        starttime = std::stol(tokens[19]);
    }
    catch (...)
    {
        return -1;
    }

    std::ifstream uptime_file("/proc/uptime");
    double system_uptime = 0.0;
    if (!uptime_file || !(uptime_file >> system_uptime))
    {
        return -1;
    }

    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck <= 0)
        clk_tck = 100;

    double start_time_sec = static_cast<double>(starttime) / clk_tck;
    long uptime_seconds = static_cast<long>(system_uptime - start_time_sec);
    return uptime_seconds >= 0 ? uptime_seconds : 0;
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
    metrics.cpu_stat_first_line = read_first_line((base_path / "cpu.stat").string());
    metrics.uptime_seconds = get_uptime(container_id);

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

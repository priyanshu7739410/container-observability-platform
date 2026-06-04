#include "display.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

static double bytes_to_mb(long bytes)
{
    return bytes / (1024.0 * 1024.0);
}

static std::string format_uptime(long seconds)
{
    if (seconds < 0)
        return "-";
    if (seconds < 60)
        return std::to_string(seconds) + "s";
    if (seconds < 3600)
        return std::to_string(seconds / 60) + "m " + std::to_string(seconds % 60) + "s";
    return std::to_string(seconds / 3600) + "h " + std::to_string((seconds % 3600) / 60) + "m";
}

void print_table(const std::vector<ContainerMetrics>& metrics)
{
    // 1. Host Summary
    std::cout << "COP Monitor\n\n";
    std::cout << "Containers: " << metrics.size() << "\n";
    
    double total_mem_mb = 0.0;
    long total_pids = 0;
    for (const auto& m : metrics)
    {
        total_mem_mb += bytes_to_mb(m.memory_current);
        total_pids += m.pids;
    }
    std::cout << "Total Memory: " << total_mem_mb << " MB\n";
    std::cout << "Total PIDs:   " << total_pids << "\n";
    std::cout << "Refresh:      1s\n\n";

    // 2. Sort by memory_current descending
    std::vector<ContainerMetrics> sorted_metrics = metrics;
    std::sort(sorted_metrics.begin(), sorted_metrics.end(), [](const ContainerMetrics& a, const ContainerMetrics& b) {
        return a.memory_current > b.memory_current;
    });

    // 3. Print table header
    std::cout << std::left
              << std::setw(20) << "CONTAINER"
              << std::setw(10) << "MEM(MB)"
              << std::setw(12) << "LIMIT"
              << std::setw(8)  << "MEM%"
              << std::setw(6)  << "PIDS"
              << std::setw(8)  << "CPU%"
              << std::setw(10) << "UPTIME"
              << std::setw(25) << "CPU_STAT"
              << std::endl;

    std::cout << std::string(99, '-') << std::endl;

    // 4. Print rows
    for (const auto& m : sorted_metrics)
    {
        std::string limit_display;
        std::string mem_percent_display = "-";
        std::string color_code = "";
        bool has_color = false;

        if (m.memory_limit == "max" || m.memory_limit.empty())
        {
            limit_display = "Unlimited";
            color_code = "\033[32m"; // Green
            has_color = true;
        }
        else
        {
            try
            {
                long limit_bytes = std::stol(m.memory_limit);
                if (limit_bytes > 0)
                {
                    double pct = (static_cast<double>(m.memory_current) / limit_bytes) * 100.0;
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(1) << pct << "%";
                    mem_percent_display = ss.str();

                    if (pct > 80.0)
                    {
                        color_code = "\033[31m"; // Red
                        has_color = true;
                    }
                    else if (pct > 50.0)
                    {
                        color_code = "\033[33m"; // Yellow
                        has_color = true;
                    }
                    else
                    {
                        color_code = "\033[32m"; // Green
                        has_color = true;
                    }
                }
                limit_display = std::to_string(static_cast<int>(bytes_to_mb(limit_bytes))) + " MB";
            }
            catch (...)
            {
                limit_display = m.memory_limit;
            }
        }

        std::string cpu_percent_str;
        {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << m.cpu_percent << "%";
            cpu_percent_str = ss.str();
        }

        std::cout << std::left
                  << std::setw(20) << m.id
                  << std::setw(10) << bytes_to_mb(m.memory_current)
                  << std::setw(12) << limit_display;

        if (has_color)
            std::cout << color_code;
        std::cout << std::setw(8) << mem_percent_display;
        if (has_color)
            std::cout << "\033[0m";

        std::cout << std::setw(6)  << m.pids
                  << std::setw(8)  << cpu_percent_str
                  << std::setw(10) << format_uptime(m.uptime_seconds)
                  << std::setw(25) << m.cpu_stat_first_line
                  << std::endl;
    }
}

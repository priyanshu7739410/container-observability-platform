#include "display.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

static double bytes_to_mb(long bytes)
{
    return bytes / (1024.0 * 1024.0);
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
              << std::setw(12) << "MEM(MB)"
              << std::setw(15) << "LIMIT"
              << std::setw(10) << "MEM%"
              << std::setw(8)  << "PIDS"
              << std::setw(8)  << "CPU%"
              << std::endl;

    std::cout << std::string(75, '-') << std::endl;

    // 4. Print rows
    for (const auto& m : sorted_metrics)
    {
        std::string limit_display;
        std::string mem_percent_display = "-";

        if (m.memory_limit == "max" || m.memory_limit.empty())
        {
            limit_display = "Unlimited";
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
                }
                limit_display = std::to_string(static_cast<int>(bytes_to_mb(limit_bytes))) + " MB";
            }
            catch (...)
            {
                limit_display = m.memory_limit;
            }
        }

        std::cout << std::left
                  << std::setw(20) << m.id
                  << std::setw(12) << bytes_to_mb(m.memory_current)
                  << std::setw(15) << limit_display
                  << std::setw(10) << mem_percent_display
                  << std::setw(8)  << m.pids
                  << std::setw(8)  << m.cpu_percent
                  << std::endl;
    }
}

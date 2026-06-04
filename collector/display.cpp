#include "display.h"
#include <iostream>
#include <iomanip>

static double bytes_to_mb(long bytes)
{
    return bytes / (1024.0 * 1024.0);
}

void print_table(const std::vector<ContainerMetrics>& metrics)
{
    std::cout << std::left
              << std::setw(20) << "CONTAINER"
              << std::setw(15) << "MEM(MB)"
              << std::setw(15) << "LIMIT"
              << std::setw(10) << "PIDS"
              << std::setw(15) << "CPU(usec)"
              << std::endl;

    std::cout << std::string(75, '-') << std::endl;

    for (const auto& m : metrics)
    {
        std::string limit_display;
        if (m.memory_limit == "max")
        {
            limit_display = "Unlimited";
        }
        else
        {
            try
            {
                limit_display = std::to_string(static_cast<int>(bytes_to_mb(std::stol(m.memory_limit)))) + " MB";
            }
            catch (...)
            {
                limit_display = m.memory_limit;
            }
        }

        std::cout << std::left
                  << std::setw(20) << m.id
                  << std::setw(15) << bytes_to_mb(m.memory_current)
                  << std::setw(15) << limit_display
                  << std::setw(10) << m.pids
                  << std::setw(15) << m.cpu_usage_usec
                  << std::endl;
    }
}

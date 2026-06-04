#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

long read_metric(const std::string& path)
{
    std::ifstream file(path);

    long value = 0;

    if (file)
        file >> value;

    return value;
}

std::string read_text(const std::string& path)
{
    std::ifstream file(path);

    std::string value;

    if (file)
        file >> value;

    return value;
}

long read_cpu_usage(const std::string& path)
{
    std::ifstream file(path);

    std::string key;
    long value = 0;

    if (file)
        file >> key >> value;

    return value;
}

double bytes_to_mb(long bytes)
{
    return bytes / (1024.0 * 1024.0);
}

struct ContainerMetrics
{
    std::string id;

    long memory_current;
    std::string memory_limit;

    long pids;

    long cpu_usage_usec;
};

void print_header()
{
    std::cout << std::left
              << std::setw(20) << "CONTAINER"
              << std::setw(15) << "MEM(MB)"
              << std::setw(15) << "LIMIT"
              << std::setw(10) << "PIDS"
              << std::setw(15) << "CPU(usec)"
              << std::endl;

    std::cout << std::string(75, '-')
              << std::endl;
}

int main()
{
    while (true)
    {
        system("clear");

        std::cout << std::fixed
                  << std::setprecision(2);

        print_header();

        for (const auto& entry :
             fs::directory_iterator("/sys/fs/cgroup"))
        {
            if (!entry.is_directory())
                continue;

            std::string name =
                entry.path().filename().string();

            if (name.rfind("mycontainer-", 0) != 0)
                continue;

            ContainerMetrics metrics;

            metrics.id = name;

            metrics.memory_current =
                read_metric(
                    (entry.path() / "memory.current").string());

            metrics.memory_limit =
                read_text(
                    (entry.path() / "memory.max").string());

            metrics.pids =
                read_metric(
                    (entry.path() / "pids.current").string());

            metrics.cpu_usage_usec =
                read_cpu_usage(
                    (entry.path() / "cpu.stat").string());

            std::string limit_display;

            if (metrics.memory_limit == "max")
            {
                limit_display = "Unlimited";
            }
            else
            {
                limit_display =
                    std::to_string(
                        static_cast<int>(
                            bytes_to_mb(
                                std::stol(metrics.memory_limit))))
                    + " MB";
            }

            std::cout << std::left
                      << std::setw(20) << metrics.id
                      << std::setw(15)
                      << bytes_to_mb(metrics.memory_current)
                      << std::setw(15)
                      << limit_display
                      << std::setw(10)
                      << metrics.pids
                      << std::setw(15)
                      << metrics.cpu_usage_usec
                      << std::endl;
        }

        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }

    return 0;
}
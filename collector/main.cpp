#include "container.h"
#include "metrics.h"
#include "display.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

int main()
{
    while (true)
    {
        // Clear terminal screen
        std::system("clear");

        std::cout << std::fixed << std::setprecision(2);

        auto containers = discover_containers();
        auto metrics = collect_all_metrics(containers);
        print_table(metrics);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
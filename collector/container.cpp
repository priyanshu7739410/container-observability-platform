#include "container.h"
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> discover_containers()
{
    std::vector<std::string> containers;
    const std::string cgroup_path = "/sys/fs/cgroup";
    
    if (!fs::exists(cgroup_path) || !fs::is_directory(cgroup_path))
    {
        return containers;
    }

    for (const auto& entry : fs::directory_iterator(cgroup_path))
    {
        if (!entry.is_directory())
            continue;

        std::string name = entry.path().filename().string();
        if (name.rfind("mycontainer-", 0) == 0)
        {
            containers.push_back(name);
        }
    }
    return containers;
}

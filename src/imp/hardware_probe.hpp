#pragma once

#include <string>
#include <vector>

namespace system_info {

struct HardwareProbeItem
{
    std::string source;
    bool available = false;
};

struct HardwareProbeResult
{
    void Add(const std::string &source, const std::string &value)
    {
        items.push_back({source, !value.empty()});
        combinedProperties += value;
    }

    std::string combinedProperties;
    std::vector<HardwareProbeItem> items;
};

} // end namespace system_info

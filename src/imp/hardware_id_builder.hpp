#pragma once

#include <string>

namespace system_info {

std::string BuildHardwareIdFromProperties(const std::string &hardwareProperties,
                                          const std::string &applicationNamespace);

} // end namespace system_info

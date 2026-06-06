#include "hardware_id_builder.hpp"

#include "md5.hpp"
#include "util.hpp"

namespace system_info {

std::string BuildHardwareIdFromProperties(const std::string &hardwareProperties,
                                          const std::string &applicationNamespace)
{
    if (hardwareProperties.empty()) {
        return std::string();
    }

    std::string properties = hardwareProperties;
    if (!applicationNamespace.empty()) {
        properties += ":" + applicationNamespace;
    }

    MD5 md5(properties);
    return Util::HashToUUID(md5.HexDigest());
}

} // end namespace system_info

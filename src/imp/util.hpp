#pragma once

#include <string>

namespace system_info {

/**
 * @brief Helper class and various helper functions
 */
class Util {
 public:
    /**
     * @brief Get the current time
     * @return Time in string format like [hour:min:sec:ms]
     */
    static std::string GetTime();

    /**
     * @brief Generate UUID from 32-character string (hash)
     * @param hash 32 character string
     * @return UUID
     */
    static std::string HashToUUID(const std::string &hash);

    /**
     * @brief Converts a wide character string to its corresponding multibyte character string
     * @param ws Wide string
     * @return Corresponding narrow string
     */
    static std::string WstringToString(const std::wstring &ws);

 private:
    /**
     * @brief Swap bytes in string representation
     * @param uuid 32 character string
     * @param begin Beginning of section
     * @param end End of section
     */
    static void SwapUUIDBlocks(std::string &uuid, const int begin, const int end);
};

} // end namespace system_info

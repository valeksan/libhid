#include "util.hpp"

#include <chrono>
#include <locale>
#include <codecvt>

namespace system_info {

std::string Util::GetTime()
{
    std::string timeStr = "[";

    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration tp = now.time_since_epoch();

    tp -= std::chrono::duration_cast<std::chrono::seconds>(tp);
    const time_t tt = std::chrono::system_clock::to_time_t(now);
    const tm* currentTime = localtime(&tt);

    timeStr += std::to_string(currentTime->tm_hour) + ":";
    timeStr += std::to_string(currentTime->tm_min) + ":";
    timeStr += std::to_string(currentTime->tm_sec) + ":";
    timeStr += std::to_string(tp/std::chrono::milliseconds(1));

    timeStr += "]";
    return timeStr;
}

std::string Util::HashToUUID(const std::string &hash)
{
    std::string uuid = hash;

    SwapUUIDBlocks(uuid, 0, 8);
    SwapUUIDBlocks(uuid, 8, 12);
    SwapUUIDBlocks(uuid, 12, 16);
    SwapUUIDBlocks(uuid, 16, 20);

    int offset = 0;
    uuid.insert(8, "-");
    uuid.insert(12 + (++offset), "-");
    uuid.insert(16 + (++offset), "-");
    uuid.insert(20 + (++offset), "-");

    return uuid;
}

std::string Util::WstringToString(const std::wstring &ws)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(ws);
}

void Util::SwapUUIDBlocks(std::string &uuid, const int begin, const int end)
{
    for (int i = begin; i < (begin + end)/2 ; i += 2) {
        auto temp1 = uuid[i];
        auto temp2 = uuid[i + 1];
        uuid[i] = uuid[end + begin - i - 2];
        uuid[i + 1] = uuid[end + begin - i - 1];
        uuid[end + begin - i - 2] = temp1;
        uuid[end + begin - i - 1] = temp2;
    }
}

} // end namespace system_info

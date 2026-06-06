#include "imp/md5.hpp"
#include "imp/util.hpp"

#include <string>

namespace {

bool ExpectEqual(const std::string &actual, const std::string &expected)
{
    return actual == expected;
}

} // namespace

int main()
{
    using system_info::MD5;
    using system_info::Util;

    if (!ExpectEqual(MD5("").HexDigest(), "d41d8cd98f00b204e9800998ecf8427e")) {
        return 1;
    }

    if (!ExpectEqual(MD5("abc").HexDigest(), "900150983cd24fb0d6963f7d28e17f72")) {
        return 1;
    }

    if (!ExpectEqual(MD5("message digest").HexDigest(), "f96b697d7cb7938d525a2f31aaf161d0")) {
        return 1;
    }

    if (!ExpectEqual(Util::HashToUUID("0123456789abcdef0123456789abcdef"),
                     "67452301-ab89-efcd-2301-456789abcdef")) {
        return 1;
    }

    if (!ExpectEqual(Util::WstringToString(L"libhid"), "libhid")) {
        return 1;
    }

    return 0;
}

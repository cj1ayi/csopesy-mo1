#include "Utils.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string format_time(const std::chrono::system_clock::time_point &tp) {
    const std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif
    std::ostringstream out;
    out << std::put_time(&local, "(%m/%d/%Y %I:%M:%S%p)");
    return out.str();
}

std::string format_c(const std::string &fmt, const std::vector<uint16_t> &values) {
    std::array<int, 8> a{};
    std::copy_n(values.begin(), std::min(values.size(), a.size()), a.begin());

    const int len = std::snprintf(nullptr, 0, fmt.c_str(), a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    if (len <= 0) return "";

    std::string out(static_cast<size_t>(len), '\0');
    std::snprintf(out.data(), out.size() + 1, fmt.c_str(), a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    return out;
}

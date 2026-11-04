#include "Utils.h"

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

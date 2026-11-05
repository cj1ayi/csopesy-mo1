#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

std::string format_time(const std::chrono::system_clock::time_point &tp);

std::string format_c(const std::string &fmt, const std::vector<uint16_t> &values);

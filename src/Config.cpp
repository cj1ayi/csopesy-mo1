#include "Config.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>

namespace {
constexpr uint64_t U32_RANGE = 1ULL << 32;

std::string unquote(const std::string &s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') return s.substr(1, s.size() - 2);
    return s;
}

uint64_t parse_in_range(const std::string &key, const std::string &raw, const uint64_t lo, const uint64_t hi) {
    bool all_digits =
        !raw.empty() && std::all_of(raw.begin(), raw.end(), [](unsigned char c) { return std::isdigit(c); });
    if (!all_digits || raw.size() > 19)
        throw std::runtime_error(key + " must be a non-negative integer, got \"" + raw + "\"");

    uint64_t value = std::stoull(raw);
    if (value < lo || value > hi)
        throw std::runtime_error(key + " must be in [" + std::to_string(lo) + ", " + std::to_string(hi) + "], got " +
                                 raw);
    return value;
}
} // namespace

Config Config::load(const std::string &path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("could not open " + path);

    Config config;
    std::string key, raw;
    while (file >> key) {
        if (!(file >> raw)) throw std::runtime_error("missing value for " + key);
        raw = unquote(raw);

        if (key == "num-cpu")
            config.num_cpu = static_cast<uint32_t>(parse_in_range(key, raw, 1, 128));
        else if (key == "scheduler") {
            if (raw == "fcfs")
                config.scheduler = FCFS;
            else if (raw == "rr")
                config.scheduler = RR;
            else
                throw std::runtime_error("scheduler must be \"fcfs\" or \"rr\", got \"" + raw + "\"");
        } else if (key == "quantum-cycles")
            config.quantum_cycles = parse_in_range(key, raw, 1, U32_RANGE);
        else if (key == "batch-process-freq")
            config.batch_process_freq = parse_in_range(key, raw, 1, U32_RANGE);
        else if (key == "min-ins")
            config.min_ins = parse_in_range(key, raw, 1, U32_RANGE);
        else if (key == "max-ins")
            config.max_ins = parse_in_range(key, raw, 1, U32_RANGE);
        else if (key == "delay-per-exec" || key == "delays-per-exec")
            config.delay_per_exec = parse_in_range(key, raw, 0, U32_RANGE);
        else
            throw std::runtime_error("unknown config key \"" + key + "\"");
    }

    if (config.min_ins > config.max_ins) throw std::runtime_error("min-ins must not be greater than max-ins");
    return config;
}

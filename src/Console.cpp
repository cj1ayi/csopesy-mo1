#include "Console.h"

#include "Utils.h"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
const char *PROMPT = "root:\\> ";
const char *CONFIG_PATH = "config.txt";
const char *LOG_PATH = "csopesy-log.txt";

// nothing abt this in the spec...
constexpr auto TICK_INTERVAL = std::chrono::microseconds(100);

const char *DEVELOPERS[] = {"Billones, Francis", "Nacasabog, Joshua", "Santiago, Juan Ramon", "Tan, Roberta"};
const char *LAST_UPDATED = "11-04-2026";

std::vector<std::string> split(const std::string &line) {
    std::istringstream in(line);
    std::vector<std::string> words;
    for (std::string word; in >> word;) words.push_back(word);
    return words;
}

void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}
} // namespace

Console::~Console() {
    running = false;
    if (tick_thread.joinable()) tick_thread.join();
}

void Console::run() {
    print_header();

    std::string line;
    while (std::cout << PROMPT && std::getline(std::cin, line)) {
        auto args = split(line);
        if (args.empty()) continue;
        const auto &command = args[0];

        if (command == "exit") break;

        if (command == "initialize") {
            initialize();
            continue;
        }

        if (!process_manager) {
            std::cout << "Please run \"initialize\" first.\n";
            continue;
        }

        if (command == "screen")
            screen(args);
        else if (command == "scheduler-start") {
            batch_enabled = true;
            std::cout << "Scheduler started generating processes.\n";
        } else if (command == "scheduler-stop") {
            batch_enabled = false;
            std::cout << "Scheduler stopped generating processes.\n";
        } else if (command == "report-util")
            report_util();
        else
            std::cout << "Unknown command: " << command << "\n";
    }
}

void Console::print_header() {
    std::cout << R"ART(  ____ ____   ___  ____  _____ ______   __
 / ___/ ___| / _ \|  _ \| ____/ ___\ \ / /
| |   \___ \| | | | |_) |  _| \___ \\ V /
| |___ ___) | |_| |  __/| |___ ___) || |
 \____|____/ \___/|_|   |_____|____/ |_|
)ART";
    std::cout << "----------------------------------------\n";
    std::cout << "Welcome to CSOPESY Emulator!\n\n";
    std::cout << "Developers:\n";
    for (const char *developer : DEVELOPERS) std::cout << developer << "\n";
    std::cout << "\nLast updated: " << LAST_UPDATED << "\n";
    std::cout << "----------------------------------------\n\n";
}

void Console::initialize() {
    if (process_manager) {
        std::cout << "Already initialized.\n";
        return;
    }

    try {
        config = Config::load(CONFIG_PATH);
        process_manager = std::make_unique<ProcessManager>(config);
        scheduler = make_scheduler(config);

        running = true;
        tick_thread = std::thread(&Console::tick_loop, this);
        std::cout << "Initialized using " << CONFIG_PATH << ".\n";
    } catch (const std::exception &e) { std::cout << "Failed to initialize: " << e.what() << "\n"; }
}

void Console::screen(const std::vector<std::string> &args) {
    if (args.size() == 2 && args[1] == "-ls") {
        std::cout << status_report();
        return;
    }

    if (args.size() != 3 || (args[1] != "-s" && args[1] != "-r")) {
        std::cout << "Usage: screen -s <process name> | screen -r <process name> | screen -ls\n";
        return;
    }

    const auto &name = args[2];
    if (args[1] == "-s") {
        auto process = spawn(name);
        if (!process)
            std::cout << "Process " << name << " already exists.\n";
        else
            attach(process);
    } else {
        auto process = process_manager->find_process(name);
        if (!process)
            std::cout << "Process " << name << " not found.\n";
        else
            attach(process);
    }
}

std::shared_ptr<Process> Console::spawn(const std::string &name) {
    auto process = process_manager->create_process(name);
    if (process) scheduler->add(process);
    return process;
}

std::string Console::next_batch_name() {
    std::ostringstream name;
    name << "p" << std::setw(2) << std::setfill('0') << ++batch_count;
    return name.str();
}

void Console::tick_loop() {
    while (running) {
        scheduler->tick();
        ++ticks;
        if (batch_enabled && ticks % config.batch_process_freq == 0) {
            while (!spawn(next_batch_name())) {} // skip names already taken by "screen -s"
        }
        std::this_thread::sleep_for(TICK_INTERVAL);
    }
}

std::string Console::status_report() {
    std::vector<ProcessSnapshot> snapshot;
    {
        auto paused = scheduler->pause();
        snapshot = process_manager->snapshot();
    }
    return format_status_report(snapshot); // formatting doesn't hold up the ticks
}

std::string Console::format_status_report(const std::vector<ProcessSnapshot> &snapshot) const {
    uint32_t cores_used = 0;
    std::ostringstream running, finished;
    for (const auto &row : snapshot) {
        const auto &process = row.process;
        if (row.status == TERMINATED) {
            finished << std::left << std::setw(12) << process->name << format_time(process->created_at)
                     << "    Finished    " << row.executes << " / " << process->loc << "\n";
        } else if (row.core_id != -1) {
            ++cores_used;
            running << std::left << std::setw(12) << process->name << format_time(process->created_at)
                    << "    Core: " << std::setw(6) << row.core_id << row.executes << " / " << process->loc << "\n";
        }
    }

    std::ostringstream out;
    out << "CPU utilization: " << (cores_used * 100 / config.num_cpu) << "%\n";
    out << "Cores used: " << cores_used << "\n";
    out << "Cores available: " << (config.num_cpu - cores_used) << "\n\n";
    out << "----------------------------------------\n";
    out << "Running processes:\n" << running.str();
    out << "\nFinished processes:\n" << finished.str();
    out << "----------------------------------------\n";
    return out.str();
}

void Console::report_util() {
    std::ofstream log(LOG_PATH);
    if (!log) {
        std::cout << "Could not write " << LOG_PATH << "\n";
        return;
    }
    log << status_report();
    std::cout << "Report generated at " << std::filesystem::absolute(LOG_PATH).string() << "!\n";
}

void Console::attach(const std::shared_ptr<Process> &process) {
    clear_screen();
    print_process_smi(*process);

    std::string line;
    while (std::cout << PROMPT && std::getline(std::cin, line)) {
        auto args = split(line);
        if (args.empty()) continue;

        if (args[0] == "exit") break;

        if (args[0] == "process-smi")
            print_process_smi(*process);
        else
            std::cout << "Unknown command: " << args[0] << "\n";
    }

    clear_screen();
    print_header();
}

void Console::print_process_smi(const Process &process) {
    std::vector<std::string> print_logs;
    STATUS status;
    uint64_t executes;
    {
        std::lock_guard<std::mutex> lock(process.mtx);
        print_logs = process.print_logs;
        status = process.status;
        executes = process.executes;
    }
    std::cout << "\nProcess name: " << process.name << "\n";
    std::cout << "ID: " << process.pid << "\n";
    std::cout << "Logs:\n";
    for (const auto &log : print_logs) std::cout << log << "\n";
    std::cout << "\n";

    if (status == TERMINATED)
        std::cout << "Finished!\n\n";
    else {
        std::cout << "Current instruction line: " << executes << "\n";
        std::cout << "Lines of code: " << process.loc << "\n\n";
    }
}

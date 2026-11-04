#include "Console.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
const char *PROMPT = "root:\\> ";
const char *CONFIG_PATH = "config.txt";
const char *LOG_PATH = "csopesy-log.txt";

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
            // TODO: start generating batch processes
            std::cout << "Scheduler started generating processes.\n";
        } else if (command == "scheduler-stop") {
            // TODO: stop generating batch processes
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
        process_manager = std::make_unique<ProcessManager>(Config::load(CONFIG_PATH));
        std::cout << "Initialized using " << CONFIG_PATH << ".\n";
    } catch (const std::exception &e) { std::cout << "Failed to initialize: " << e.what() << "\n"; }
}

void Console::screen(const std::vector<std::string> &args) {
    if (args.size() == 2 && args[1] == "-ls") {
        std::cout << process_manager->status_report();
        return;
    }

    if (args.size() != 3 || (args[1] != "-s" && args[1] != "-r")) {
        std::cout << "Usage: screen -s <process name> | screen -r <process name> | screen -ls\n";
        return;
    }

    const auto &name = args[2];
    if (args[1] == "-s") {
        auto process = process_manager->create_process(name);
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

void Console::report_util() {
    std::ofstream log(LOG_PATH);
    if (!log) {
        std::cout << "Could not write " << LOG_PATH << "\n";
        return;
    }
    log << process_manager->status_report();
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
    std::cout << "\nProcess name: " << process.name << "\n";
    std::cout << "ID: " << process.pid << "\n";
    std::cout << "Logs:\n";
    for (const auto &log : process.print_logs) std::cout << log << "\n";
    std::cout << "\n";

    if (process.status == TERMINATED)
        std::cout << "Finished!\n\n";
    else {
        std::cout << "Current instruction line: " << process.executes << "\n";
        std::cout << "Lines of code: " << process.loc << "\n\n";
    }
}

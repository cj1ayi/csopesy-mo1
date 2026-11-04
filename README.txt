CSOPESY MO1 - Process Scheduler and CLI

Developers:
    Billones, Francis
    Nacasabog, Joshua
    Santiago, Juan Ramon
    Tan, Roberta

Entry point:
  src/main.cpp (the main function creates a Console and calls run())

Building (needs a C++17 compiler):
  With CMake:
    cmake -S . -B build
    cmake --build build

  Without CMake (g++ / MinGW):
    g++ -std=c++17 -O2 -pthread src/*.cpp -o csopesy

Running:
  Run the program from the folder that holds config.txt, since "initialize"
  reads config.txt from the current directory:
    ./build/csopesy        (CMake build)
    ./csopesy              (g++ build)

  Type "initialize" first. Until then only "initialize" and "exit" work.

config.txt (space-separated, one parameter per line):
  num-cpu             1 to 128
  scheduler           "fcfs" or "rr"
  quantum-cycles      1 to 2^32 (only used by rr)
  batch-process-freq  1 to 2^32
  min-ins             1 to 2^32
  max-ins             1 to 2^32
  delay-per-exec      0 to 2^32

Main menu commands:
  initialize                  read config.txt and set up the scheduler
  screen -s <process name>    create a process and attach to its screen
  screen -r <process name>    reattach to a running process
  screen -ls                  show CPU utilization and running/finished processes
  scheduler-start             start generating dummy processes
  scheduler-stop              stop generating dummy processes
  report-util                 save the screen -ls output to csopesy-log.txt
  exit                        quit

Process screen commands:
  process-smi                 show the process name, ID, logs and progress
  exit                        return to the main menu

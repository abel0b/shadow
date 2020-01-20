#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>
#include <vector>
#include <chrono>
#include <ctime>

struct Log {
    std::chrono::time_point<std::chrono::system_clock> date;
    std::string formatted_date;
    std::string message;
};

class Console {
public:
    Console(int history_size);
    void log(std::string message);
    std::vector<Log> history;

private:
    int history_size;
};

#endif

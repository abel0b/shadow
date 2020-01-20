#include "shadow/Console.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

Console::Console(int history_size) : history_size(history_size) {


}

void Console::log(std::string message) {
    Log entry;
    entry.date = std::chrono::system_clock::now();
    entry.message = message;
    
    std::time_t date_c = std::chrono::system_clock::to_time_t(entry.date);
    
    std::ostringstream stream;
    stream << std::put_time(std::localtime(&date_c), "%H:%M:%S");
    
    entry.formatted_date = stream.str();

    std::cout << "\033[1;34m[" << entry.formatted_date << "]\033[0m " << entry.message << std::endl;
    
    history.push_back(entry);
    
    if (history.size() > history_size) {
        history.erase(history.begin());
    }
}

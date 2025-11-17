#ifndef LOGGER
#define LOGGER

#include <string>

class CLogger {
public:
    void Info(std::string message);
    void Error(std::string message);
    void Warn(std::string message);
};

#endif /* LOGGER */

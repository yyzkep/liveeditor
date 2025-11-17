#include "logger.hpp"

void CLogger::Info(std::string message) {
    printf("[INFO]  %s", message.c_str());
}

void CLogger::Warn(std::string message) {
    printf("[WARN] %s", message.c_str());
}

void CLogger::Error(std::string message) {
    printf("[ERROR] %s", message.c_str());
}
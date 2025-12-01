#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace VibeReaper {

// Log levels for filtering messages
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERR
};

class Logger {
public:
    // Get singleton instance
    static Logger& GetInstance();

    // Main logging function
    void Log(LogLevel level, const std::string& message);

    // Helper methods for specific log levels
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void LogError(const std::string& message);

    // Enable/disable console output
    void SetConsoleOutput(bool enabled);

private:
    Logger();
    ~Logger();

    // Prevent copy and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string GetTimestamp() const;
    std::string LevelToString(LogLevel level) const;
    std::string GetColorCode(LogLevel level) const;

    bool m_consoleOutput;
    std::ofstream m_logFile;
};

} // namespace VibeReaper

// Convenience macros for easy logging
#define LOG_DEBUG(msg) VibeReaper::Logger::GetInstance().Debug(msg)
#define LOG_INFO(msg) VibeReaper::Logger::GetInstance().Info(msg)
#define LOG_WARNING(msg) VibeReaper::Logger::GetInstance().Warning(msg)
#define LOG_ERROR(msg) VibeReaper::Logger::GetInstance().LogError(msg)

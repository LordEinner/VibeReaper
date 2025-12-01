#include "Logger.h"

namespace VibeReaper {

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : m_consoleOutput(true) {
    // Constructor - logger starts with console output enabled
    // Open log file for writing
    m_logFile.open("VibeReaper.log", std::ios::out | std::ios::trunc);
    if (m_logFile.is_open()) {
        m_logFile << "=== VibeReaper Log Started ===" << std::endl;
    }
}

Logger::~Logger() {
    // Destructor
    if (m_logFile.is_open()) {
        m_logFile << "=== VibeReaper Log Ended ===" << std::endl;
        m_logFile.close();
    }
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::string timestamp = GetTimestamp();
    std::string levelStr = LevelToString(level);
    std::string colorCode = GetColorCode(level);

    // Format: [TIMESTAMP] [LEVEL] Message
    std::string formattedMessage = "[" + timestamp + "] [" + levelStr + "] " + message;

    if (m_consoleOutput) {
        // Output to console with color (Windows console supports ANSI colors in Win10+)
        std::cout << colorCode << formattedMessage << "\033[0m" << std::endl;
    }

    // Always write to log file
    if (m_logFile.is_open()) {
        m_logFile << formattedMessage << std::endl;
        m_logFile.flush(); // Flush immediately so logs are written even if crash occurs
    }
}

void Logger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void Logger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void Logger::Warning(const std::string& message) {
    Log(LogLevel::WARNING, message);
}

void Logger::LogError(const std::string& message) {
    Log(LogLevel::ERR, message);
}

void Logger::SetConsoleOutput(bool enabled) {
    m_consoleOutput = enabled;
}

std::string Logger::GetTimestamp() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::LevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

std::string Logger::GetColorCode(LogLevel level) const {
    // ANSI color codes for terminal output
    switch (level) {
        case LogLevel::DEBUG:   return "\033[36m"; // Cyan
        case LogLevel::INFO:    return "\033[32m"; // Green
        case LogLevel::WARNING: return "\033[33m"; // Yellow
        case LogLevel::ERR:   return "\033[31m"; // Red
        default:                return "\033[0m";  // Reset
    }
}

} // namespace VibeReaper

#include <Windows.h>
#include <fstream>
#include <string>

#define DEFAULT_LOG_FILE "application_log.txt"

// Assuming LOGFILE and DEBUGSTRING are defined as 0 or 1 somewhere in your build configuration
// For example, you could define them in your project settings or before including this header

// Global log file stream initialization could be done in a similar way or through a function call
std::ofstream logFileStream;

// This is a simplified approach; consider more sophisticated log initialization and checking
inline void InitializeLogFile() {
#ifdef LOGFILE
    if (!logFileStream.is_open()) {
        logFileStream.open(DEFAULT_LOG_FILE, std::ios::app);
    }
#endif
}

inline void LogDebugStringW(const std::wstring& message) {
#ifdef DEBUGSTRING
    OutputDebugStringW(message.c_str());
#endif
}

inline void LogFileW(const std::wstring& message) {
#ifdef LOGFILE
    if (logFileStream.is_open()) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::string narrow = converter.to_bytes(message);
        logFileStream << narrow;
}
#endif
}

inline void LogDebugString(const std::string& message) {
#ifdef DEBUGSTRING
    OutputDebugStringA(message.c_str());
#endif
}

inline void LogFile(const std::string& message) {
#ifdef LOGFILE
    if (logFileStream.is_open()) {
        logFileStream << message;
    }
#endif
}

#define WLOG_HELPER(level, message) \
    do { \
        const wchar_t* file = _CRT_WIDE(__FILE__); \
        int line = __LINE__; \
        std::wstring wmessage = std::wstring(L"[") + level + L"] (" + file + L":" + std::to_wstring(line) + L") " + message + L"\n"; \
        LogDebugStringW(wmessage); \
        LogFileW(wmessage); \
    } while (0)

#define LOG_HELPER(level, message) \
    do { \
        const char* file = __FILE__; \
        int line = __LINE__; \
        std::string formattedMessage = std::string("[") + level + "] (" + file + ":" + std::to_string(line) + ") " + message + "\n"; \
        LogDebugString(formattedMessage); \
        LogFile(formattedMessage); \
    } while (0)

#define LOGINFOW(message)    WLOG_HELPER(L"INFO", message)
#define LOGDEBUGW(message)   WLOG_HELPER(L"DEBUG", message)
#define LOGWARNINGW(message) WLOG_HELPER(L"WARNING", message)
#define LOGERRORW(message)   WLOG_HELPER(L"ERROR", message)

#define LOGINFO(message)    LOG_HELPER("INFO", message)
#define LOGDEBUG(message)   LOG_HELPER("DEBUG", message)
#define LOGWARNING(message) LOG_HELPER("WARNING", message)
#define LOGERROR(message)   LOG_HELPER("ERROR", message)

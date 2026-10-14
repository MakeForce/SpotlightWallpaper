#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <string>
#include <memory>
#include <mutex>
#include <filesystem>
#include <windows.h>
#include <codecvt>

class Logger
{
public:
    // 获取全局单例
    static Logger &GetInstance();

    // 初始化日志系统（线程安全，仅执行一次）
    // @param log_dir 日志根目录
    // @param logger_name 日志记录器名称（也是文件名前缀）
    void Init(const std::string &log_dir, const std::string &logger_name = "app");

    // 动态设置日志等级 (枚举方式)
    void SetLevel(spdlog::level::level_enum level);

    // 动态设置日志等级 (字符串方式，方便从配置文件读取)
    // 支持: "trace", "debug", "info", "warn", "err", "critical", "off"
    void SetLevel(const std::string &level_str);

    // 获取底层 logger 指针（用于高级用法或直接调用 spdlog 宏）
    std::shared_ptr<spdlog::logger> GetLogger() const { return m_logger; }

    // ==================== 宽字符转换工具 ====================
    // wstring -> string
    static std::string WstrToStr(const std::wstring &wstr);

    // string -> wstring
    static std::wstring StrToWstr(const std::string &str);

    // wchar_t* -> string
    static std::string WstrToStr(const wchar_t *wstr);

private:
    Logger() = default;
    ~Logger() = default;

    // 禁用拷贝和赋值
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    std::shared_ptr<spdlog::logger> m_logger;
    std::once_flag m_init_flag; // 用于保证 Init 只执行一次
};

// ==================== 字符串转换实现（头文件内联，方便使用） ====================

inline std::string Logger::WstrToStr(const std::wstring &wstr)
{
    if (wstr.empty())
        return {};

    int len = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.data(), (int)wstr.size(),
        nullptr, 0,
        nullptr, nullptr);

    std::string str(len, 0);
    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.data(), (int)wstr.size(),
        &str[0], len,
        nullptr, nullptr);
    return str;
}

inline std::wstring Logger::StrToWstr(const std::string &str)
{
    if (str.empty())
        return {};

    int len = MultiByteToWideChar(
        CP_UTF8, 0,
        str.data(), (int)str.size(),
        nullptr, 0);

    std::wstring wstr(len, 0);
    MultiByteToWideChar(
        CP_UTF8, 0,
        str.data(), (int)str.size(),
        &wstr[0], len);
    return wstr;
}

inline std::string Logger::WstrToStr(const wchar_t *wstr)
{
    if (!wstr)
        return {};
    return WstrToStr(std::wstring(wstr));
}

// 包装：自动把 wstring / const wchar_t* 转成 string 再打日志
#define LOG_WRAP(level, ...) \
    Logger::GetInstance().GetLogger()->level(__VA_ARGS__)

#define LOG_TRACE(...) LOG_WRAP(trace, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_WRAP(debug, __VA_ARGS__)
#define LOG_INFO(...) LOG_WRAP(info, __VA_ARGS__)
#define LOG_WARN(...) LOG_WRAP(warn, __VA_ARGS__)
#define LOG_ERROR(...) LOG_WRAP(error, __VA_ARGS__)
#define LOG_CRITICAL(...) LOG_WRAP(critical, __VA_ARGS__)
#include "Logger.h"
#include <iostream>

Logger &Logger::GetInstance()
{
    // Meyer's Singleton，C++11 保证线程安全
    static Logger instance;
    return instance;
}

void Logger::Init(const std::string &log_dir, const std::string &logger_name)
{
    // std::call_once 确保在多线程环境下，Init 也只会执行一次
    std::call_once(m_init_flag, [this, &log_dir, &logger_name]()
                   {
        // 1. 确保日志目录存在 (C++17 特性)
        std::error_code ec;
        std::filesystem::create_directories(log_dir, ec);
        if (ec) {
            std::cerr << "[Logger] Failed to create log directory: " << log_dir << std::endl;
        }

        // 2. 构建日志文件完整路径
        // spdlog 会自动在文件名后追加日期，例如：logs/app_2023-10-27.log
        std::string full_path = log_dir + "/" + logger_name;

        // 3. 创建每日滚动 Sink (mt 代表多线程安全)
        // 参数：文件名前缀, 每天几点滚动(0-23), 每天几分滚动(0-59)
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            full_path, 0, 0); // 每天 00:00 滚动

        // 4. 创建 Logger 并绑定 Sink
        m_logger = std::make_shared<spdlog::logger>(logger_name, daily_sink);

        // 5. 设置日志格式
        // 格式：[2023-10-27 14:30:00.123] [info] [app] User logged in
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");

        // 6. 设置默认日志等级 (默认只输出 info 及以上)
        m_logger->set_level(spdlog::level::info);

        // 7. 设置自动 flush 策略 (遇到 error 级别时自动 flush 到磁盘，防止崩溃丢失日志)
        m_logger->flush_on(spdlog::level::info);

        // 8. (可选) 注册为 spdlog 的默认 logger
        spdlog::set_default_logger(m_logger);
        
        LOG_INFO("Logger initialized successfully. Log dir: {}", log_dir); });
}

void Logger::SetLevel(spdlog::level::level_enum level)
{
    if (m_logger)
    {
        m_logger->set_level(level);
        // 注意：如果之前有 flush_on 设置，修改 level 后可能需要重新设置，这里保持默认即可
    }
}

void Logger::SetLevel(const std::string &level_str)
{
    if (!m_logger)
        return;

    // 将字符串转换为 spdlog 的 level_enum
    auto level = spdlog::level::from_str(level_str);
    if (level != spdlog::level::off || level_str == "off")
    {
        m_logger->set_level(level);
    }
    else
    {
        LOG_WARN("Invalid log level string: {}. Supported: trace, debug, info, warn, err, critical, off", level_str);
    }
}
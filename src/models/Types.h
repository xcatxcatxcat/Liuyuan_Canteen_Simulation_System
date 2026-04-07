#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QStringList>

namespace Models {

// ============================================================
//  枚举类型定义 — 仿真世界中的状态与策略
// ============================================================

/// 学生当前状态
enum class StudentState {
    Moving,         // 移动中（前往食堂/窗口/座位）
    Queuing,        // 排队中（等待打饭）
    BeingServed,    // 正在被服务（打饭中）
    FindingSeat,    // 寻找座位
    Dining,         // 就餐中
    Leaving         // 离开食堂
};

/// 窗口服务状态
enum class WindowState {
    Idle,           // 空闲
    Serving         // 服务中
};

/// 窗口选择偏好策略
enum class WindowPreference {
    ShortestQueue,      // 最短队列优先
    Random,             // 随机选择
    FastestService,     // 最快服务速度优先
    Fixed               // 固定偏好
};

/// 座位选择策略
enum class SeatPreference {
    MostEmptyTable,     // 选择空位最多的桌子
    Random,             // 随机选择
    Nearest             // 选择最近的空位
};

/// 日志记录级别
enum class LogLevel {
    Minimal,        // 仅关键指标
    Normal,         // 常规事件
    Detailed        // 详细事件日志
};

/// 运行模式
enum class RunMode {
    Visual,         // 可视化模式
    Headless        // 无头模式（纯计算）
};

// ============================================================
//  辅助函数 — 枚举与字符串的互相转换
// ============================================================

inline QString studentStateToString(StudentState s) {
    switch (s) {
    case StudentState::Moving:      return QStringLiteral("移动中");
    case StudentState::Queuing:     return QStringLiteral("排队中");
    case StudentState::BeingServed: return QStringLiteral("服务中");
    case StudentState::FindingSeat: return QStringLiteral("找座位");
    case StudentState::Dining:      return QStringLiteral("就餐中");
    case StudentState::Leaving:     return QStringLiteral("离开");
    }
    return {};
}

inline QString windowStateToString(WindowState s) {
    return s == WindowState::Serving ? QStringLiteral("服务中") : QStringLiteral("空闲");
}

inline QString windowPreferenceToString(WindowPreference p) {
    switch (p) {
    case WindowPreference::ShortestQueue:  return QStringLiteral("最短队列优先");
    case WindowPreference::Random:         return QStringLiteral("随机选择");
    case WindowPreference::FastestService: return QStringLiteral("最快服务优先");
    case WindowPreference::Fixed:          return QStringLiteral("固定偏好");
    }
    return {};
}

inline QStringList windowPreferenceOptions() {
    return {
        windowPreferenceToString(WindowPreference::ShortestQueue),
        windowPreferenceToString(WindowPreference::Random),
        windowPreferenceToString(WindowPreference::FastestService),
        windowPreferenceToString(WindowPreference::Fixed)
    };
}

inline WindowPreference stringToWindowPreference(const QString &s) {
    if (s == windowPreferenceToString(WindowPreference::Random))         return WindowPreference::Random;
    if (s == windowPreferenceToString(WindowPreference::FastestService)) return WindowPreference::FastestService;
    if (s == windowPreferenceToString(WindowPreference::Fixed))          return WindowPreference::Fixed;
    return WindowPreference::ShortestQueue;
}

inline QString seatPreferenceToString(SeatPreference p) {
    switch (p) {
    case SeatPreference::MostEmptyTable: return QStringLiteral("空位最多优先");
    case SeatPreference::Random:         return QStringLiteral("随机选择");
    case SeatPreference::Nearest:        return QStringLiteral("最近优先");
    }
    return {};
}

inline QStringList seatPreferenceOptions() {
    return {
        seatPreferenceToString(SeatPreference::MostEmptyTable),
        seatPreferenceToString(SeatPreference::Random),
        seatPreferenceToString(SeatPreference::Nearest)
    };
}

inline SeatPreference stringToSeatPreference(const QString &s) {
    if (s == seatPreferenceToString(SeatPreference::Random))  return SeatPreference::Random;
    if (s == seatPreferenceToString(SeatPreference::Nearest)) return SeatPreference::Nearest;
    return SeatPreference::MostEmptyTable;
}

inline QString logLevelToString(LogLevel l) {
    switch (l) {
    case LogLevel::Minimal:  return QStringLiteral("仅关键指标");
    case LogLevel::Normal:   return QStringLiteral("常规事件");
    case LogLevel::Detailed: return QStringLiteral("详细日志");
    }
    return {};
}

inline QStringList logLevelOptions() {
    return {
        logLevelToString(LogLevel::Minimal),
        logLevelToString(LogLevel::Normal),
        logLevelToString(LogLevel::Detailed)
    };
}

inline LogLevel stringToLogLevel(const QString &s) {
    if (s == logLevelToString(LogLevel::Normal))   return LogLevel::Normal;
    if (s == logLevelToString(LogLevel::Detailed)) return LogLevel::Detailed;
    return LogLevel::Minimal;
}

} // namespace Models

#endif // TYPES_H

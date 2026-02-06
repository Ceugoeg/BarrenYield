//
// Created by ceugo-eg on 2026/1/31.
//

#ifndef BARREN_YIELD_Logger_H
#define BARREN_YIELD_Logger_H


#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

class GameLogger : public Node{
    GDCLASS(GameLogger, Node)

public:
    enum LogType {
        INFO,       // 白色
        WARNING,    // 黄色
        ERROR,      // 红色
        CHEAT,      // 紫色
        SYSTEM,     // 绿色
    };

private:
    static GameLogger* singleton;
    String log_file_path;

    // 文本颜色设置
    String _apply_bbcode_color(const String& p_msg);

protected:
    static void _bind_methods();

public:
    GameLogger();
    ~GameLogger();
    static GameLogger* get_singleton();

    // 发送日志
    void log(const String& p_msg, LogType p_type = INFO);

    // 读取最近100行日志并发送给 UI
    void replay_history(int max_lines = 100);

    // 存档接口预留
    void save_log_to_disk(const String& line);
};

// 把 ENUM 注册给 Godot
VARIANT_ENUM_CAST(GameLogger::LogType);

#endif //BARREN_YIELD_Logger_H
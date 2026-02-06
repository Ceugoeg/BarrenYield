//
// Created by ceugo-eg on 2026/1/31.
//

#include "../../include/core/GameLogger.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/time.hpp>

using namespace godot;

GameLogger* GameLogger::singleton = nullptr;

String GameLogger::_apply_bbcode_color(const String& p_msg) {
    String color_code = "white";

    if (p_msg.begins_with("[WARNING]"))         { color_code = "yellow"; }
    else if (p_msg.begins_with("[ERROR]"))      { color_code = "red"; }
    else if (p_msg.begins_with("[CHEAT]"))      { color_code = "violet"; }
    else if (p_msg.begins_with("[SYSTEM]"))     { color_code = "green"; }
    else if (p_msg.begins_with(">"))            { color_code = "grey"; }

    return "[color=" + color_code + "]" + p_msg + "[/color]";
}

GameLogger::GameLogger() {
    singleton = this;
    // user:// 是 Godot 规定的用户存档目录，保证跨平台安全
    log_file_path = "user://game_logs.txt";

    // 每次游戏启动保留追加日志内容
    Ref<FileAccess> file = FileAccess::open(log_file_path, FileAccess::WRITE_READ);
}

GameLogger::~GameLogger() {
    if (singleton == this) { singleton = nullptr; }
}

GameLogger *GameLogger::get_singleton() {
    return singleton;
}

void GameLogger::_bind_methods() {
    ClassDB::bind_method(D_METHOD("log", "message", "type"), &GameLogger::log);
    ClassDB::bind_method(D_METHOD("replay_history", "max_lines"), &GameLogger::replay_history, DEFVAL(100));

    // 注册信号：当有新日志时，通知 UI
    ADD_SIGNAL(MethodInfo("log_updated", PropertyInfo(Variant::STRING, "bbcode_text")));

    // 绑定枚举
    BIND_ENUM_CONSTANT(INFO);
    BIND_ENUM_CONSTANT(WARNING);
    BIND_ENUM_CONSTANT(ERROR);
    BIND_ENUM_CONSTANT(CHEAT);
    BIND_ENUM_CONSTANT(SYSTEM);
}

void GameLogger::log(const String& p_msg, LogType p_type) {
    String prefix = "";
    // String color_code = "white";    // BBCode 颜色

    switch (p_type) {
        case INFO:      prefix = "[INFO] ";     break;
        case WARNING:   prefix = "[WARNING] ";  break;
        case ERROR:     prefix = "[ERROR] ";    break;
        case CHEAT:     prefix = "[CHEAT] ";    break;
        case SYSTEM:    prefix = "[SYSTEM] ";   break;
    }

    // 组装纯文本
    String raw_text = prefix + p_msg;

    // 组装给 UI 显示的文本：[color=xxx][prefix] msg[/color]
    String ui_text = _apply_bbcode_color(raw_text);

    // 控制台打印一份
    UtilityFunctions::print(raw_text);

    // 发送信号给 UI
    emit_signal("log_updated", ui_text);

    // 写入文件
    save_log_to_disk(raw_text);

}

void GameLogger::save_log_to_disk(const String& line) {
    Ref<FileAccess> file = FileAccess::open(log_file_path, FileAccess::READ_WRITE);
    if (file.is_valid()) {
        file->seek_end();
        file->store_line(line);
    }
}

void GameLogger::replay_history(int max_lines) {
    if (!FileAccess::file_exists(log_file_path)) { return; }

    Ref<FileAccess> file = FileAccess::open(log_file_path, FileAccess::READ);
    if (file.is_null()) { return; }

    uint64_t file_len = file->get_length();
    if (file_len == 0) { return; }

    // ====== 1. 倒序查找起始位置 ======
    const int CHUNK_SIZE = 4096;
    uint64_t cursor = file_len;
    int newlines_found = 0;
    uint64_t start_pos = 0;

    while (cursor > 0 && newlines_found <= max_lines) {
        uint64_t bytes_to_read = MIN((uint64_t)CHUNK_SIZE, cursor);
        cursor -= bytes_to_read;

        file->seek(cursor);
        // 获取二进制 buffer，避免字符串转换开销
        PackedByteArray chunk = file->get_buffer(bytes_to_read);
        const uint8_t *ptr = chunk.ptr();   // 原始指针进一步提速

        // 倒序扫描
        for (int64_t i = bytes_to_read; i >= 0; --i) {
            if (ptr[i] == '\n') {
                newlines_found++;
                if (newlines_found > max_lines) {
                    start_pos = cursor + i + 1;
                    goto FOUND_START;
                }
            }
        }
    }

    // ====== 2. 从 start_pos 正向读取 ======
    FOUND_START:

    file->seek(start_pos);

    while (!file->eof_reached()) {
        String line = file->get_line();
        if (line.is_empty() && file->eof_reached()) { break; }

        // 给 raw_text 套上颜色：
        String ui_text = _apply_bbcode_color(line);

        emit_signal("log_updated", ui_text);
    }
}

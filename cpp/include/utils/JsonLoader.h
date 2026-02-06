//
// Created by ceugo-eg on 2026/1/27.
//

#ifndef BARREN_YIELD_JSONLOADER_H
#define BARREN_YIELD_JSONLOADER_H

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

class JsonLoader {
public:
    // 静态工具函数：输入文件路径，返回解析好的 Variant
    static Variant load_file(const String& p_path);
};


#endif //BARREN_YIELD_JSONLOADER_H
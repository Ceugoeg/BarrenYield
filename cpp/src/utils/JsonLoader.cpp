//
// Created by ceugo-eg on 2026/1/27.
//

#include "../../include/utils/JsonLoader.h"

Variant JsonLoader::load_file(const String& p_path) {
    // 1. 检查文件存在性
    if (!FileAccess::file_exists(p_path)) {
        UtilityFunctions::push_error("JsonLoader: File not found -> " + p_path);
        return Variant();   // 返回空的 Variant
    }

    // 2. 读取文本内容
    Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);
    String content = file->get_as_text();
    file->close();

    // 3. 解析文本内容
    Ref<JSON> json = memnew(JSON);
    Error err = json->parse(content);
    if (err != OK) {
        UtilityFunctions::push_error("JsonLoader: Parse error in " + p_path +
            " at line " + String::num_int64(json->get_error_line()) +
            ": " + json->get_error_message());
        return Variant();
    }

    // 4. 返回数据
    return json->get_data();
}
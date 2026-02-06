//
// Created by ceugo-eg on 2026/1/27.
//

#include "../../include/core/ResourceRegistry.h"
#include "utils/JsonLoader.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// 初始化静态指针为空
ResourceRegistry* ResourceRegistry::singleton = nullptr;

// 构造函数
ResourceRegistry::ResourceRegistry() {
    // 鉴于 Godot 的特殊节点机制，直接把最新创建的设为单例就好了
    singleton = this;
}

// 析构函数
ResourceRegistry::~ResourceRegistry() {
    // 如果死的是当前单例，就把指针清空
    if (singleton == this) { singleton = nullptr; }
}

// 静态函数：让外界获取单例
ResourceRegistry* ResourceRegistry::get_singleton() {
    return singleton;
}

// 绑定方法：将方法暴露出去，确保 GDScript 能看到它们
void ResourceRegistry::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_data"), &ResourceRegistry::load_data);
    ClassDB::bind_method(D_METHOD("get_item_data", "id"), &ResourceRegistry::get_item_data);
    ClassDB::bind_method(D_METHOD("has_item", "id"), &ResourceRegistry::has_item);
}

// 读取 JSON 数据
void ResourceRegistry::load_data() {
    // // ====== 临时代码：手动定义“煤炭”的mock数据 ======
    // Dictionary coal_data;
    // coal_data["id"] = "coal";
    // coal_data["name"] = "Coal";
    // coal_data["base_value"] = 10.0;
    // coal_data["is_fuel"] = true;
    // coal_data["fuel_energy"] = 50.0;
    // // 存入大字典
    // items["coal"] = coal_data;
    // UtilityFunctions::print("ResourceRegistry: Data Loaded! \nItems registered: ", items.size());
    // // ====== 临时代码结束 ======

    items.clear();

    // 1. 定义 JSON 文件路径
    String path = "res://data/registry/resources.json";

    // 2. 调用工具类读取
    Variant data = JsonLoader::load_file(path);

    //3. 检查数据类型（必须为数组 [...]）
    if (data.get_type() != Variant::ARRAY) {
        UtilityFunctions::push_error("ResourceRegistry: JSON root must be an Array!");
        return;
    }

    Array list = data;      // 转换为 Array

    // 4. 遍历数组，构建数据
    for (int i = 0; i < list.size(); i++) {
        Dictionary item_dict = list[i];
        if (!item_dict.has("id")) { continue; }     // 简单防守性：必须有id字段
        String id = item_dict["id"];    // 获取 item id
        items[id] = item_dict;      // 存入核心大字典
    }

    UtilityFunctions::print("ResourceRegistry: JSON Loaded! \nTotal items: ", items.size());
}

Dictionary ResourceRegistry::get_item_data(const String& p_id) {
    // 检查字典里是否有这个 key
    if (items.has(p_id)) { return items[p_id]; }
    // 若没有：报错并返回空字典
    UtilityFunctions::push_error("ResourceRegistry: Item not found -> " + p_id);
    return Dictionary();
}

bool ResourceRegistry::has_item(const String& p_id) {
    return items.has(p_id);
}

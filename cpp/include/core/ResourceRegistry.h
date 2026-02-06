//
// Created by ceugo-eg on 2026/1/27.
//

#ifndef BARREN_YIELD_RESOURCEREGISTRY_H
#define BARREN_YIELD_RESOURCEREGISTRY_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

/** 定义物品的数据结构体
 * 不继承 Godot 的任何东西，仅仅充当数据容器
 */
struct ItemDef {
    String id;              // 控制台中、源代码中的名字 "iron_ore"
    String name;            // 显示给玩家看的名字 "Iron ore"
    float base_value;       // 基础售价
    bool is_fuel;           // 是燃料吗？
    float fuel_energy;      // 能量值

    // 构造函数：初始化所有变量
    ItemDef() : base_value(0.0), is_fuel(false), fuel_energy(0.0) {}
};

/** 注册表类
 * 继承自 Node，由此挂到 Godot的场景树上
 */
class ResourceRegistry : public Node {
    GDCLASS(ResourceRegistry, Node)     // 宏：告诉 Godot 自己的身份和继承自谁

private:
    // 单例指针，为了在任何地方都能 RegistryResource::get_singleton() 找到它
    static ResourceRegistry* singleton;

    /** 核心存储：
     * 用 Godot 的 Dictionary 暂时存好
     * Key = 物品ID（String），Value = 物品数据（Dictionary）
     */
    Dictionary items;

protected:
    // 绑定函数，用于暴露方法给 Godot
    static void _bind_methods();

public:
    ResourceRegistry();
    ~ResourceRegistry();

    // 静态方法获取单例
    static ResourceRegistry* get_singleton();

    // 核心功能：读取 json 数据
    void load_data();

    // 提供给外部查询的接口
    Dictionary get_item_data(const String& p_id);

    // 检查某个物品是否存在
    bool has_item(const String& p_id);
};


#endif //BARREN_YIELD_RESOURCEREGISTRY_H
//
// Created by ceugo-eg on 2026/1/31.
//

#ifndef BARREN_YIELD_INVENTORY_H
#define BARREN_YIELD_INVENTORY_H


#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

class Inventory : public Node {
    GDCLASS(Inventory, Node)

public:
    enum ActionState {
        SUCCESS,                // 成功
        ERR_INVALID_ITEM,       // 不在注册表中的物品
        ERR_NOT_IN_INVENTORY,   // 背包里不存在这个 key
        ERR_NOT_ENOUGH          // 数量不足
    };

private:
    // 核心存储：Key = 物品ID，Value = 数量（float）
    Dictionary storage;

protected:
    static void _bind_methods();

public:
    Inventory();
    ~Inventory();

    // 增
    ActionState add_item(const String& p_id, float p_amount);
    // 删
    ActionState remove_item(const String& p_id, float p_amount);
    // 查
    float get_item_amount(const String& p_id) const;

    // 获取整个库存字典（ UI 遍历用）
    Dictionary get_all_items() const;

    // 信号发射函数：库存变动时通知 UI 刷新，避免帧轮询
    void emit_inventory_updated();

    // 存档
    Dictionary save_data() const;

    // 读档
    void load_data(const Dictionary& data);
};

VARIANT_ENUM_CAST(Inventory::ActionState);


#endif //BARREN_YIELD_INVENTORY_H
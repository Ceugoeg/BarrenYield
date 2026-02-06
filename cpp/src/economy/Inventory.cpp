//
// Created by ceugo-eg on 2026/1/31.
//

#include "../../include/economy/Inventory.h"
#include "core/ResourceRegistry.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Inventory::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add_item", "id", "amount"), &Inventory::add_item);
    ClassDB::bind_method(D_METHOD("remove_item", "id", "amount"), &Inventory::remove_item);
    ClassDB::bind_method(D_METHOD("get_item_amount", "id"), &Inventory::get_item_amount);
    ClassDB::bind_method(D_METHOD("get_all_items"), &Inventory::get_all_items);

    BIND_ENUM_CONSTANT(SUCCESS);
    BIND_ENUM_CONSTANT(ERR_INVALID_ITEM);
    BIND_ENUM_CONSTANT(ERR_NOT_IN_INVENTORY);
    BIND_ENUM_CONSTANT(ERR_NOT_ENOUGH);

    // 注册信号：让 Godot 知道这个节点会发出 "inventory_updated"
    ADD_SIGNAL(MethodInfo("inventory_updated"));
}

Inventory::Inventory() {}

Inventory::~Inventory() {}

Inventory::ActionState Inventory::add_item(const String &p_id, float p_amount) {
    // 1. 安全检查：物品是否存在
    if (!ResourceRegistry::get_singleton()->has_item(p_id)) {
        UtilityFunctions::push_warning("Inventory: Try to add unknown item -> " + p_id);
        return ERR_INVALID_ITEM;
    }

    // 2. 增加数量
    float current = 0.0;
    if (storage.has(p_id)) {
        current = storage[p_id];
    }
    storage[p_id] = current + p_amount;

    // 3. 通知 UI 更新
    emit_signal("inventory_updated");
    // 调试：
    UtilityFunctions::print("Inventory: Added ", p_amount, " of ", p_id, ". New Total: ", storage[p_id]);
    return SUCCESS;
}

Inventory::ActionState Inventory::remove_item(const String &p_id, float p_amount) {
    // 1. 安全检查
    if (!ResourceRegistry::get_singleton()->has_item(p_id)) { return ERR_INVALID_ITEM; }
    if (!storage.has(p_id)) { return ERR_NOT_IN_INVENTORY; }

    // 2. 减去数量
    float current = storage[p_id];
    if (current < p_amount) { return ERR_NOT_ENOUGH; }       // 余额不足
    storage[p_id] = current - p_amount;

    // 3. 通知 UI 更新
    emit_signal("inventory_updated");
    // 注：此处保留数量为 0 的 key 在字典中
    return SUCCESS;
}

float Inventory::get_item_amount(const String &p_id) const {
    // 安全检查
    if (!storage.has(p_id)) { return -1.0f; }
    return storage[p_id];
}

Dictionary Inventory::get_all_items() const {
    return storage;
}

void Inventory::emit_inventory_updated() {
    emit_signal("inventory_updated");
}

// 存读档：待完成，现在是简单逻辑
Dictionary Inventory::save_data() const {
    Dictionary data;
    data["storage"] = storage;
    return data;
}

void Inventory::load_data(const Dictionary& data) {
    if (data.has("storage")) {
        storage = data["storage"];
        emit_signal("inventory_updated");
    }
}

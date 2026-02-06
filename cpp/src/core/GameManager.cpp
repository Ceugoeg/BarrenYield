//
// Created by ceugo-eg on 2026/1/27.
//

#include "core/GameManager.h"
#include "core/GameLogger.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "core/ResourceRegistry.h"

using namespace godot;

void GameManager::_bind_methods() {
    // 暴露给 UI 调用
    ClassDB::bind_method(D_METHOD("execute_command", "input"), &GameManager::execute_command);
    ClassDB::bind_method(D_METHOD("get_inventory"), &GameManager::get_inventory);
}

GameManager::GameManager() {
    time_passed = 0.0;
    // UtilityFunctions::print("GameManager Initialized via C++!");

    // 自动创建一个库存对象，挂在 GameManager 下
    player_inventory = memnew(Inventory);   // 不在构造函数里 add_child，放在 _ready() 里
}

GameManager::~GameManager() {
}

void GameManager::_process(double delta) {
    // time_passed += delta;
    // if (time_passed > 5.0) {
    //     UtilityFunctions::print("C++ Core is running... Tick.");
    //     time_passed = 0.0;
    // }
}

void GameManager::_ready() {
    if (player_inventory) {
        // 检查是否已经添加，防止重复添加
        if (player_inventory->get_parent() == nullptr) {
            add_child(player_inventory);
        }
    }
}

Inventory* GameManager::get_inventory() const {
    return player_inventory;
}

void GameManager::execute_command(const String& input) {
    // 安全检查 + 是否为命令处理
    if (input.is_empty() || !input.begins_with("/")) { return; }

    // 命令解析：按空格分割
    PackedStringArray parts = input.split(" ", false);
    if (parts.size() == 0) { return; }

    String cmd = parts[0];

    // ====== /add <item_id> [amount=1] ======
    if (cmd == "/add") {
        // 至少要有 /add 和 id 两部分
        if (parts.size() < 2) {
            // UtilityFunctions::print("[Cheat] Usage: /add <item_id> [amount=1]");
            GameLogger::get_singleton()->log("Usage: /add <item_id> [amount=1]", GameLogger::WARNING);
            return;
        }

        String item_id = parts[1];
        float amount = 1.0;     // 默认值为1

        // 若输入了第三部分，则覆盖数量的默认值
        if (parts.size() >= 3) {
            amount = parts[2].to_float();
        }

        if (player_inventory) {
            Inventory::ActionState action_state = player_inventory->add_item(item_id, amount);
            // UtilityFunctions::print("[Cheat] Added ", amount, " ", item_id);
            switch (action_state) {
                case Inventory::SUCCESS:
                    GameLogger::get_singleton()->log("Added "+ String::num(amount) + " " + item_id, GameLogger::CHEAT);
                    break;
                case Inventory::ERR_INVALID_ITEM:
                    GameLogger::get_singleton()->log("\'" + item_id + "\' is not a valid item", GameLogger::WARNING);
                    break;
                default:
                    GameLogger::get_singleton()->log("Failed to add item", GameLogger::ERROR);
                    break;
            }
        } else {
            // UtilityFunctions::print("Error: Player Inventory not initialized!");
            GameLogger::get_singleton()->log("Player Inventory not initialized!", GameLogger::ERROR);
        }
    }

    // ====== /remove <item_id> [amount=1] ======
    else if (cmd == "/remove") {
        if (parts.size() < 2) {
            // UtilityFunctions::print("[Cheat] Usage: /remove <item_id> [amount=1]");
            GameLogger::get_singleton()->log("Usage: /remove <item_id> [amount=1]", GameLogger::WARNING);
            return;
        }

        String item_id = parts[1];
        float amount = 1.0;

        if (parts.size() >= 3) {
            amount = parts[2].to_float();
        }

        if (player_inventory) {
            Inventory::ActionState action_state = player_inventory->remove_item(item_id, amount);
            // UtilityFunctions::print("[Cheat] Removed ", amount, " ", item_id);
            switch (action_state) {
                case Inventory::SUCCESS:
                    GameLogger::get_singleton()->log("Removed " + String::num(amount) + " " + item_id, GameLogger::CHEAT);
                    break;
                case Inventory::ERR_INVALID_ITEM:
                    GameLogger::get_singleton()->log("\'" + item_id + "\' is not a valid item", GameLogger::WARNING);
                    break;
                case Inventory::ERR_NOT_IN_INVENTORY:
                    GameLogger::get_singleton()->log("\'" + item_id + "\' is not in the inventory", GameLogger::WARNING);
                    break;
                case Inventory::ERR_NOT_ENOUGH:
                    GameLogger::get_singleton()->log("Not enough '" + item_id + "' in the inventory", GameLogger::WARNING);
                    break;
                default:
                    GameLogger::get_singleton()->log("Failed to remove item", GameLogger::ERROR);
                    break;
            }
        } else {
            // UtilityFunctions::print("[Cheat] Error: Player Inventory not initialized!");
            GameLogger::get_singleton()->log("Player Inventory not initialized!", GameLogger::ERROR);
        }
    }

    // ====== /help ======
    else if (cmd == "/help" || cmd == "/?") {
        // UtilityFunctions::print("======== Commands ========");
        // UtilityFunctions::print("/add <item_id> [amount=1] - Add item");
        // UtilityFunctions::print("/remove <item_id> [amount=1] - Remove item");
        // UtilityFunctions::print("/help - List All Commands");
        GameLogger::get_singleton()->log("\n======= Commands =======\n"
                                     "/add <item_id> [amount=1] - Add item\n"
                                     "/remove <item_id> [amount=1] - Remove item\n"
                                     "/help or /? - List All Commands", GameLogger::SYSTEM);
    }

    // ====== 未知指令 ======
    else {
        // UtilityFunctions::print("Unknown Command: ", cmd);
        // UtilityFunctions::print("[Cheat] Enter \"/help\" to list all commands");
        GameLogger::get_singleton()->log("Unknown command\n"
                                     "Enter \"/help\" to list all commands", GameLogger::SYSTEM);
    }
}

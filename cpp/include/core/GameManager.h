//
// Created by ceugo-eg on 2026/1/27.
//

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <godot_cpp/classes/node.hpp>
#include "economy/Inventory.h"

namespace godot {

class GameManager : public Node {
    GDCLASS(GameManager, Node)

private:
    double time_passed;
    Inventory* player_inventory;    // 玩家库存指针

protected:
    static void _bind_methods();

public:
    GameManager();
    ~GameManager();

    void _process(double delta) override;
    void _ready() override;

    // 提供给 GDScript 获取库存的接口
    Inventory* get_inventory() const;
    // 处理指令输入逻辑
    void execute_command(const String& input);
};

}

#endif
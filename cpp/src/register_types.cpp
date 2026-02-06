//
// Created by ceugo-eg on 2026/1/27.
//

#include "register_types.h"
#include "core/GameManager.h"       // 引用要注册的类
#include "core/ResourceRegistry.h"
#include "economy/Inventory.h"
#include "core/GameLogger.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_barren_yield_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    // 在这里注册类
    ClassDB::register_class<GameManager>();
    ClassDB::register_class<ResourceRegistry>();
    ClassDB::register_class<Inventory>();
    ClassDB::register_class<GameLogger>();
}

void uninitialize_barren_yield_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
    // GDExtension 初始化入口
    GDExtensionBool GDE_EXPORT barren_yield_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_barren_yield_module);
        init_obj.register_terminator(uninitialize_barren_yield_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}

# Barren Yield: The Wasteland Industrialist

# 最终开发实施白皮书 (Implementation Bible)

> **Version:** 1.0-FINAL
> **Engine:** Godot 4.x + C++ GDExtension + **DeepSeek API**
> **Genre:** Hardcore Industrial Survival / Resource Management Strategy
> **Core Philosophy:** "Boredom is Failure." (No meaningless clicking, no offline waiting, constant irreversible decisions).

---

## 0. 设计哲学与核心约束 (Design Philosophy)

### 0.1 三大铁律

1. **成本线性，需求指数 (Linear Cost, Exponential Demand):**
* **绝对禁止**建筑造价随数量递增（如：第100个水泵的造价 = 第1个水泵）。
* **压力来源**：完全来自外部（游商订单、里程碑），而非内部通胀。玩家的挑战在于如何在有限的土地和电力瓶颈下，堆叠出满足指数级订单的产能。


2. **数据驱动 (Data-Driven):**
* 所有资源、机器参数、配方必须定义在 External Registry (JSON/Struct) 中，严禁 Hardcode。
* 这为后续通过 LLM 动态调整平衡性预留接口。


3. **决策透明 (Decision Transparency):**
* UI 必须直观展示瓶颈（Tech Flowchart）。
* 没有隐性概率（除了游商的博弈），只有显性的效率计算。



### 0.2 游戏循环 (The Loop)

1. **土地 (Land):** 种植作物  维持生存 / 产出基础能源。
2. **工业 (Industry):** 消耗电力与原料  产出高级物资。
3. **博弈 (The Gamble):** 与 **DeepSeek 驱动的游商** 谈判  投入物资作为“期货”  获取/损失蓝图。
4. **声望 (Reputation):** 完成订单  提升声望  解锁声望墙后的科技。
5. **终局 (The Singularity):** 合成 ATM9 级别的复杂产物  通关。

---

## 1. 核心系统架构 (Core Systems)

### 1.1 土地与设施二元论

* **种植区 (The Grid):**
* **硬限制:** 固定 **100个地块 (10x10)**。不可扩展。
* **功能:** 仅用于生物质生产（食物、燃料作物）。
* **污染挂钩:** 工业污染度直接百分比降低地块的 `GrowRate`。


* **设施区 (Facilities):**
* **无空间限制:** 机器不占用 Grid，独立存在于列表中。
* **软限制:** 受限于 **电力负载**、**库存容量** 和 **原料输入**。
* **建造:** 只要资源够，点击即造，耗时固定且线性。



### 1.2 游商与订单系统 (Trader & Orders) —— *重构核心*

游商不是商店，而是**情报掮客**与**期货交易员**。

* **交互接口:** 集成 DeepSeek API。
* **行为模式 (Futures Trading):**
1. **Pitch (画饼):** 游商根据当前 Phase 和玩家资源，生成一段叙事：“我知道北方废墟有一批 *量子处理单元*，但我需要 *2000吨乙醇* 打通关节。”
2. **Invest (下注):** 玩家决定是否投资。
3. **Outcome (结算):**
* *High Roll:* 游商带回蓝图或稀有资源（回报率 > 300%）。
* *Low Roll:* 游商只带回垃圾。
* *Scam:* 游商跑路，声望下降。




* **常态化订单:**
* 由固定NPC（军阀、科学家）发布。
* **限制:** 具有严格倒计时。
* **奖励:** **声望 (Reputation)** 和 **糖 (Sugar)**。



### 1.3 科技与声望墙 (Tech & Reputation Wall)

* **蓝图覆盖 (Blueprint Override):**
* 获得 Mk.2 蓝图后，UI 自动替换 Mk.1 建造按钮。
* 提供全局 **Retrofit** 按钮，一键升级旧设施。


* **声望锁:**
* 科技树不仅需要资源解锁，还需要声望达标（e.g., "聚变堆 - 需声望 5000"）。
* 逼迫玩家必须参与订单循环，不能闭门造车。



### 1.4 全局资源池 (Global Pools)

* **Tick 逻辑:** `Input -> Consume -> Produce -> Grow -> Pollution -> Order Check` (严禁 Timer，统一在 `_physics_process` 处理)。
* **电力:** `Gen < Load` 时，全网机器效率 = `Gen / Load`。
* **水:** `Stored <= 0` 时，全网作物停止生长，甚至枯死。
* **污染:** 燃煤/化工产生污染值，不可逆（除非后期科技清洗），永久削弱农业。

---

## 2. 完整资源数据库 (Master Resource Registry)

### 2.1 基础采集 (Raw & Mining)

| ID | 名称 | 来源 | 用途 |
| --- | --- | --- | --- |
| `worm` | 虫子 | 手动扒土 | 初级肥料/饲料 |
| `stone` | 石头 | 挖掘 | 建筑基石 |
| `coal` | 煤炭 | 挖掘 | 肮脏燃料 (Energy: 10) |
| `iron_ore` | 铁矿 | 挖掘 | 基础结构金属 |
| `copper_ore` | 铜矿 | 挖掘 | 导电金属 |
| `silicon` | 硅石 | 自动钻机 Mk.3 | 电子工业基石 |
| `gold` | 金矿 | 自动钻机 Mk.3 | 精密电路/硬通货 |

### 2.2 农业与生物 (Agriculture)

| ID | 名称 | 来源 | 用途 |
| --- | --- | --- | --- |
| `wheat` | 小麦 | 种植 (10s) | 主粮，副产麦秆 |
| `sugar` | **糖** | 甘蔗压榨 | **唯一通用货币 (Hard Currency)** |
| `melon` | 西瓜 | 种植 | 压榨 -> 乙醇 (清洁能源) |
| `cotton` | 棉花 | 种植 | 纤维 (绝缘/包装) |
| `potato` | 土豆 | 种植 | 猪饲料 |
| `pork` | 猪肉 | 养猪 | 游商高价收购品 |
| `lard` | 猪油 | 养猪 | **润滑油**原料 |
| `methane` | 甲烷 | 养牛 | 气体燃料 (Energy: 50) |

### 2.3 工业中间件 (Industry)

| ID | 名称 | 配方 | 用途 |
| --- | --- | --- | --- |
| `brick` | 石砖 | Stone x2 | 仓库/一级机器原料 |
| `iron_ingot` | 铁锭 | Iron Ore + Fuel | 机器骨架 |
| `copper_ingot` | 铜锭 | Copper Ore + Fuel | 线缆 |
| `concrete` | 混凝土 | Stone + Water | 高级建筑原料 |
| `plastic` | 塑料 | Ethanol + Starch | 机器外壳 |
| `lubricant` | 润滑油 | Lard x1 | 机器维护/运作消耗 |

### 2.4 高精尖与终局 (High-Tech & Endgame)

| ID | 名称 | 配方 | 用途 |
| --- | --- | --- | --- |
| `circuit` | 电路板 | Copper + Plastic | 自动化核心 |
| `processor` | 处理器 | Silicon + Gold + Circuit | 智能核心 |
| `fusion_cell` | 聚变电池 | Deuterium (Water) + High-Tech | 终局能源组件 |
| `super_gene` | 超变基因 | 极低概率农业突变 | 终局生物组件 |
| `essence` | **生命精华** | **ATM9 级合成** | **通关道具** |

---

## 3. 机器设施列表 (Facilities Registry)

所有机器建造时间线性固定（e.g., 5秒）。

| 分类 | 名称 | 等级 (Tier) | 功能描述 |
| --- | --- | --- | --- |
| **Infra** | 粮仓 (Silo) | Mk.1 / Mk.2 | 增加 Food/Seed 上限。 |
| **Infra** | 堆料场 (Stockpile) | Mk.1 / Mk.2 | 增加 Mineral/Ore 上限。 |
| **Infra** | 储液罐 (Tank) | Mk.1 / Mk.2 | 增加 Water/Oil 上限。 |
| **Infra** | 物资库 (Warehouse) | Mk.1 | 增加 Product/Tech 上限。 |
| **Prod** | 水泵 (Pump) | Mk.1-3 | 产出水。Mk.3 效率极高但耗电巨大。 |
| **Prod** | 脱粒机 (Thresher) | Mk.1-3 | 小麦 -> 种子+麦秆。Mk.3 双倍产种。 |
| **Prod** | 榨糖机 (Presser) | Mk.1-2 | 甘蔗 -> 糖。 |
| **Prod** | 提炼厂 (Refinery) | Mk.1-3 | 西瓜->乙醇 / 猪油->润滑油。 |
| **Prod** | 熔炉 (Smelter) | Mk.1-3 | 矿 -> 锭。Mk.1 烧煤(高污)，Mk.3 等离子(极快)。 |
| **Prod** | 组装机 (Assembler) | Mk.1-3 | 零件 -> 成品。Mk.3 支持纳米工艺。 |
| **Prod** | 自动钻机 (Drill) | Mk.1-3 | 自动产矿。Mk.3 可挖金/硅。 |
| **Power** | 燃煤机 | T1 | 早期主力，高污染，低维护。 |
| **Power** | 生物发电机 | T2 | 烧乙醇，清洁，中等效率。 |
| **Power** | 燃气轮机 | T3 | 烧甲烷，高效，需畜牧业支持。 |
| **Power** | 聚变堆 | T4 | 烧水(氘)，终极能源，无污染。 |

---

## 4. 游戏阶段链 (Phase Chain)

游戏进程由**游商事件**和**声望门槛**驱动。

### Phase 1: 原始积累 (The Scavenger)

* **状态:** 手动扒土，手动种地。
* **目标:** 积攒 **糖 (Sugar)**。
* **游商交互:** 仅提供基础生存物资交换，无期货博弈。
* **解锁:** 购买 [石工技术] & [基础农业]。

### Phase 2: 蒸汽与污染 (The Smoke)

* **事件:** [寒潮危机]。订单系统上线，索要大量燃料。
* **核心冲突:** 烧煤发电  污染上升  农作物减产。
* **博弈:** 玩家必须在“牺牲环境换产能”和“苟且偷生”中选择。
* **转折点:** 积累声望，解锁 [化工图纸]，开启 **西瓜->乙醇** 清洁能源路线。

### Phase 3: 战争与期货 (The War)

* **事件:** [全面战争]。军阀索要海量 铁锭、塑料、润滑油。
* **核心机制:** **期货博弈 (Futures)**。
* 常规产能无法满足指数级订单。
* 玩家必须把资源借给游商，赌他带回 **Mk.3 蓝图** 或 **自动化核心**。


* **压力:** 100块地资源分配瓶颈（发电用西瓜 vs 饲料用土豆 vs 经济用甘蔗）。

### Phase 4: 奇点 (The Singularity)

* **事件:** [方舟计划]。
* **终局挑战 (ATM9 Style):** 合成 `生命精华`。
* **需求:** 9999x 量子处理器 + 9999x 聚变电池 + 9999x 超变基因。
* **状态:** 全图机器 Mk.3 + 超频，电力负载达到吉瓦级 (GW)。


* **胜利:** 完成合成，发射方舟。

---

## 5. 工程实现与代码结构 (Engineering & Code)

### 5.1 目录结构

```text
BarrenYield/
├── cpp/                            # [C++ Workspace]
│   ├── SConstruct                  # SCons 构建脚本
│   ├── CMakeLists.txt              # CLion 索引配置
│   ├── godot-cpp/                  # (Submodule) GDExtension 核心库
│   │
│   ├── include/                    # [Header Files]
│   │   ├── register_types.h        # 模块注册入口声明
│   │   │
│   │   ├── core/
│   │   │   ├── GameManager.h       # 全局状态管理
│   │   │   └── ResourceRegistry.h  # 资源数据定义 (Structs)
│   │   │
│   │   ├── economy/
│   │   │   ├── Inventory.h         # 库存系统
│   │   │   ├── OrderSystem.h       # 订单生成逻辑
│   │   │   ├── TraderAI.h          # DeepSeek API 接口
│   │   │   └── Reputation.h        # 声望门槛逻辑
│   │   │
│   │   ├── production/
│   │   │   ├── Machine.h           # 机器基类与逻辑
│   │   │   └── PowerGrid.h         # 电力负载计算
│   │   │
│   │   ├── land/
│   │   │   └── Plot.h              # 地块生长与污染
│   │   │
│   │   ├── tech/
│   │   │   ├── TechTree.h          # 科技树 DAG
│   │   │   └── BlueprintManager.h  # 蓝图解锁管理
│   │   │
│   │   └── utils/
│   │       ├── JsonLoader.h        # JSON 解析器封装
│   │       └── MathHelpers.h       # 通用数学库
│   │
│   └── src/                        # [Source Files]
│       ├── register_types.cpp      # 模块注册入口实现
│       ├── core/
│       │   ├── GameManager.cpp
│       │   └── ResourceRegistry.cpp
│       ├── economy/
│       │   ├── Inventory.cpp
│       │   ├── OrderSystem.cpp
│       │   ├── TraderAI.cpp
│       │   └── Reputation.cpp
│       ├── production/
│       │   ├── Machine.cpp
│       │   └── PowerGrid.cpp
│       ├── land/
│       │   └── Plot.cpp
│       ├── tech/
│       │   ├── TechTree.cpp
│       │   └── BlueprintManager.cpp
│       └── utils/
│           └── JsonLoader.cpp
│
└── godot/                          # [Godot Workspace]
    ├── bin/                        # (自动生成) .so 动态库输出位置
    ├── barren_yield.gdextension    # 库连接文件
    ├── project.godot               # Godot 项目主文件
    ├── icon.svg
    │
    ├── assets/                     # 静态资源
    │   ├── icons/
    │   └── theme/
    │
    ├── data/                       # [Data Driven Core]
    │   └── registry/
    │       ├── resources.json      # 原始资源数据
    │       ├── machines.json       # 机器参数数据
    │       └── tech_tree.json      # 科技树配置
    │
    └── scenes/                     # 场景文件
        ├── main.tscn
        └── ui/
            ├── hud.tscn
            └── terminal.tscn

```

### 5.2 关键数据结构 (C++)

```cpp
// 游商报价结构 (AI 生成)
struct TraderOffer {
    String narrative_text; // "I need fuel to cross the dead zone..."
    Dictionary cost;       // { "ethanol": 5000 }
    Dictionary potential_reward; // { "blueprint_fusion": 1 }
    float risk_factor;     // 0.0 - 1.0 (Hidden from player)
    double return_time;    // Seconds until result
};

// 订单结构
struct Order {
    StringName issuer;     // "Warlord_Krieg"
    Dictionary requirements; // { "circuit": 1000, "pork": 500 }
    double time_limit;
    int reputation_reward;
    int currency_reward;
};

// 机器定义 (Data Driven)
struct MachineDef {
    StringName id;
    int tier;
    double power_load;
    Dictionary inputs;  // per second
    Dictionary outputs; // per second
    int reputation_required; // Tech Gate
};

```

---

## 6. UI/UX 规范

1. **Tech Flowchart (DAG):**
* 实时显示生产链路。
* **红色连线:** 阻塞/缺料。
* **绿色连线:** 正常运转 (显示 items/min)。


2. **Comms Terminal (通讯终端):**
* 聊天窗口风格。
* 显示 DeepSeek 生成的文本。
* 操作: [Invest] / [Reject] / [Accept Order]。


3. **主界面:**
* **左:** Log (仅重要事件)。
* **中:** 上部 10x10 Grid，下部 设施列表 (Tab页签)。
* **右:** 资源表 (Current/Cap) + 全局仪表 (Power/Water/Pollution)。



---

## 7. 结语

**"Barren Yield"** 是关于在匮乏中寻找效率，在崩溃边缘进行豪赌。代码必须严谨，数值必须残酷，但 UI 必须诚实。

**执行标准:** 2周内完成 Phase 1 MVP，包含基础循环与 DeepSeek 接口联调。

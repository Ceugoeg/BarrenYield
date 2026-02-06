extends GameManager

# 获取 UI 节点引用
@onready var item_list = $CanvasLayer/HUD/HBoxContainer/ItemList
@onready var input_box = $CanvasLayer/HUD/HBoxContainer/VBoxContainer/LineEdit
@onready var log_box = $CanvasLayer/HUD/HBoxContainer/VBoxContainer/LogBox

var inventory_ref

func _ready():
	# [LogBox 设置] 开启 BBCode 以支持颜色，确保滚动条激活
	if log_box is RichTextLabel:
		log_box.bbcode_enabled = true
		log_box.scroll_active = true

	# 连接库存系统
	inventory_ref = get_inventory()
	if inventory_ref:
		print("UI: Connected to Inventory successfully!")
		inventory_ref.inventory_updated.connect(_on_inventory_updated)
		refresh_list() # 初始化时刷新一次列表
	else:
		printerr("UI: Error - Cannot find Inventory!")

	# [输入框设置] 使用 gui_input 处理特殊按键（如 ESC 取消焦点）
	input_box.gui_input.connect(_on_input_box_gui_input)

	# [Logger 初始化]
	# 注意：根据之前的修改，这里使用的是重命名后的 GameLogger
	var logger = GameLogger.new()
	add_child(logger)

	# 连接日志更新信号
	logger.log_updated.connect(_on_log_updated)

	# 播放历史记录
	logger.replay_history(100)

	# [临时] 资源注册加载
	var registry = ResourceRegistry.new()
	registry.load_data()

# --- 日志系统核心逻辑 ---

# 1. 处理来自 C++ 后端的日志更新
func _on_log_updated(bbcode_text: String):
	if not log_box: return

	# [优化] 追加文本 + 双换行 (增加空行以防拥挤)
	log_box.append_text(bbcode_text + "\n\n")

	# [优化] 自动滚动到底部
	_scroll_to_bottom()

# 2. 处理玩家的命令输入 (关键逻辑)
func _handle_command_execution(text_input: String):
	if text_input.is_empty():
		return

	# [新增] 手动回显玩家的输入 (灰色文本)
	# 这一步在 C++ 执行前做，确保玩家立刻看到自己的输入
	log_box.append_text("[color=gray]> " + text_input + "[/color]\n\n")

	# 因为玩家输入不会触发 C++ 的 log_updated 信号，所以这里也要手动触发一次滚动
	_scroll_to_bottom()

	# 发送给 C++ GameManager 执行
	execute_command(text_input)

# [辅助函数] 滚动到底部 (必须使用 call_deferred)
func _scroll_to_bottom():
	var scroll_bar = log_box.get_v_scroll_bar()
	if scroll_bar:
		# 等待当前帧 UI 渲染完毕（计算出新高度）后再设置滚动条
		scroll_bar.call_deferred("set_value", scroll_bar.max_value)


# --- 输入与交互逻辑 ---

# 1. 处理输入框 *获取焦点时* 的按键
func _on_input_box_gui_input(event: InputEvent):
	if event is InputEventKey and event.pressed:

		# 情况1：按下回车 -> 提交
		if event.keycode == KEY_ENTER or event.keycode == KEY_KP_ENTER:
			var text = input_box.text
			if not text.is_empty():
				_handle_command_execution(text)

			input_box.clear()
			input_box.accept_event() # 阻止默认事件（防止失焦）

		# 情况2：按下 ESC -> 主动失焦
		elif event.keycode == KEY_ESCAPE:
			input_box.release_focus()
			input_box.accept_event()

# 2. 处理全局输入 (当输入框没有焦点时)
func _unhandled_input(event: InputEvent):
	# 处理鼠标点击
	if event is InputEventMouseButton and event.pressed:
		# [修复] 必须检查 button_index == MOUSE_BUTTON_LEFT
		# 否则滚动鼠标滚轮也会被判定为 MouseButton pressed，导致输入框意外失焦
		if event.button_index == MOUSE_BUTTON_LEFT:
			if input_box.has_focus():
				input_box.release_focus()

	# 处理键盘快捷键唤起输入框
	if event is InputEventKey and event.pressed:
		if input_box.has_focus():
			return

		# 回车 -> 唤起
		if event.keycode == KEY_ENTER or event.keycode == KEY_KP_ENTER:
			input_box.grab_focus()
			get_viewport().set_input_as_handled()

		# 斜杠 / -> 唤起并预填
		elif event.keycode == KEY_SLASH:
			input_box.grab_focus()
			input_box.text = "/"
			input_box.caret_column = 1
			get_viewport().set_input_as_handled()

# --- 库存列表逻辑 ---

func _on_inventory_updated():
	refresh_list()

func refresh_list():
	item_list.clear()
	if not inventory_ref: return

	var all_items = inventory_ref.get_all_items()

	# 遍历字典
	var keys = all_items.keys()
	for item_id in keys:
		var amount = all_items[item_id]
		var text = item_id + ": " + str(amount)
		item_list.add_item(text)
# 第 11 课：Makefile ⚙️

Makefile 自动化编译过程，让构建项目变得简单。

---

## 11.1 为什么需要 Makefile？

每次修改代码都要输入长长的编译命令：

```bash
# ❌ 每次都输入
gcc -Wall -g src/main.c src/snake.c src/food.c src/ui.c src/input.c src/score.c src/utils.c -o snake_game -lncurses
```

**Makefile 的好处：**
- ⚡ **一键编译** - `make` 即可
- 🎯 **智能编译** - 只编译修改过的文件
- 🧹 **清理方便** - `make clean`
- 📋 **标准化** - 团队使用相同的构建方式

---

## 11.2 Makefile 基础

```makefile
# 变量定义
CC = gcc
CFLAGS = -Wall -g -Isrc
LDFLAGS = -lncurses

# 目标文件列表
OBJS = obj/main.o obj/snake.o obj/food.o obj/ui.o obj/input.o obj/score.o obj/utils.o

# 最终目标
snake_game: $(OBJS)
	$(CC) $(OBJS) -o snake_game $(LDFLAGS)

# 编译规则
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf obj/*.o snake_game

# 声明伪目标
.PHONY: clean
```

---

## 11.3 Makefile 语法

### 变量

```makefile
# 定义变量
CC = gcc
CFLAGS = -Wall -g

# 使用变量
program: main.c
	$(CC) $(CFLAGS) main.c -o program
```

### 目标规则

```makefile
# 格式
target: dependencies
	command
```

```makefile
# 示例
snake_game: main.o snake.o
	gcc main.o snake.o -o snake_game
```

### 自动变量

| 变量 | 含义 |
|------|------|
| `$@` | 目标文件名 |
| `$<` | 第一个依赖文件 |
| `$^` | 所有依赖文件 |

```makefile
obj/main.o: src/main.c
	$(CC) -c $< -o $@
```

---

## 11.4 完整的贪吃蛇 Makefile

```makefile
# 编译器和标志
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc -O2
LDFLAGS = -lncurses

# 目录
SRC_DIR = src
OBJ_DIR = obj

# 源文件和目标文件
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# 目标可执行文件
TARGET = snake_game

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(OBJS)
	@echo "链接 $(TARGET)..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# 编译
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "编译 $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# 创建 obj 目录
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# 清理
clean:
	@echo "清理..."
	rm -rf $(OBJ_DIR) $(TARGET)

# 重新编译
rebuild: clean all

# 运行
run: $(TARGET)
	./$(TARGET)

# 调试版本
debug: CFLAGS += -g -DDEBUG
debug: clean all

# 帮助
help:
	@echo "可用目标:"
	@echo "  all      - 编译项目（默认）"
	@echo "  clean    - 清理编译产物"
	@echo "  rebuild  - 重新编译"
	@echo "  run      - 编译并运行"
	@echo "  debug    - 编译调试版本"
	@echo "  help     - 显示帮助"

.PHONY: all clean rebuild run debug help
```

---

## 11.5 使用 Makefile

```bash
# 编译项目
make

# 清理
make clean

# 重新编译
make rebuild

# 编译并运行
make run

# 编译调试版本
make debug

# 查看帮助
make help
```

---

## 11.6 Makefile 工作流程

```
执行 make
    ↓
检查目标 snake_game
    ↓
检查依赖 obj/*.o
    ↓
如果 .o 文件不存在或 .c 文件更新
    ↓
执行编译命令
    ↓
所有 .o 文件准备好
    ↓
执行链接命令
    ↓
生成 snake_game
```

---

## 11.7 增量编译

Make 的聪明之处在于只编译修改过的文件：

```bash
# 第一次编译
$ make
编译 src/main.c...
编译 src/snake.c...
编译 src/food.c...
链接 snake_game...

# 只修改了 snake.c
$ make
编译 src/snake.c...    # 只编译这个！
链接 snake_game...

# 没有修改
$ make
make: 'snake_game' is up to date.
```

---

## 11.8 调试 Makefile

```bash
# 显示执行的命令（不实际执行）
make -n

# 显示详细过程
make -d

# 只显示错误
make -s
```

---

## 11.9 常见错误

### 错误 1：使用空格而不是 Tab

```makefile
# ❌ 错误：使用空格
target:
    command  # 空格！

# ✅ 正确：使用 Tab
target:
	command  # Tab！
```

### 错误 2：缺少依赖

```makefile
# ❌ 可能编译顺序错误
snake_game: main.o
	gcc main.o -o snake_game

# ✅ 正确
snake_game: main.o snake.o
	gcc main.o snake.o -o snake_game
```

---

## ✅ 本课检查清单

- [ ] 理解 Makefile 的作用
- [ ] 会编写基本 Makefile
- [ ] 理解变量和自动变量
- [ ] 会使用 make 命令
- [ ] 知道如何调试 Makefile

---

## 📝 作业

1. 为你的贪吃蛇项目创建 Makefile

2. 添加以下目标：
   - `all` - 默认编译
   - `clean` - 清理
   - `run` - 运行
   - `debug` - 调试版本

3. 测试增量编译：修改一个文件，观察只重新编译该文件

---

下一课：[游戏循环](12-游戏循环.md)

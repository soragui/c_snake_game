# 第 13 课：ncurses 库 📺

ncurses 库让我们能在终端中绘制图形界面。

---

## 13.1 什么是 ncurses？

**ncurses** = "new curses"，是一个终端图形库。

**功能：**
- 📍 光标定位
- 🎨 颜色和样式
- ⌨️ 键盘输入
- 🖼️ 绘制边框和图形

---

## 13.2 安装 ncurses

```bash
# Ubuntu/Debian
sudo apt install libncurses5-dev

# Fedora/RHEL
sudo dnf install ncurses-devel

# macOS
brew install ncurses
```

---

## 13.3 基本使用

### 初始化和清理

```c
#include <ncurses.h>

int main(void) {
    // 初始化
    initscr();      // 启动 ncurses 模式
    cbreak();       // 禁用行缓冲
    noecho();       // 不显示输入字符
    keypad(stdscr, TRUE);  // 启用方向键
    
    // ... 你的代码
    
    // 清理
    endwin();       // 恢复终端
    return 0;
}
```

---

## 13.4 光标定位

```c
#include <ncurses.h>

int main(void) {
    initscr();
    
    // 方法 1：move + addch
    move(5, 10);        // 移动到第 5 行，第 10 列
    addch('X');         // 在当前位置画字符
    
    // 方法 2：mvaddch（组合）
    mvaddch(10, 20, 'O');
    
    // 方法 3：printw（格式化）
    mvprintw(15, 5, "Score: %d", 100);
    
    refresh();          // 刷新屏幕
    getch();            // 等待按键
    
    endwin();
    return 0;
}
```

---

## 13.5 键盘输入

```c
#include <ncurses.h>

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    
    int key;
    while ((key = getch()) != 'q') {
        clear();
        mvprintw(0, 0, "Key: %d", key);
        
        if (key == KEY_UP) mvprintw(2, 0, "UP");
        else if (key == KEY_DOWN) mvprintw(2, 0, "DOWN");
        else if (key == KEY_LEFT) mvprintw(2, 0, "LEFT");
        else if (key == KEY_RIGHT) mvprintw(2, 0, "RIGHT");
        
        refresh();
    }
    
    endwin();
    return 0;
}
```

---

## 13.6 贪吃蛇中的应用

```c
// 初始化
void ui_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
}

// 绘制蛇
void draw_snake(Snake* s) {
    Segment* cur = s->head;
    while (cur) {
        attron(COLOR_PAIR(1));
        mvaddch(cur->y, cur->x, 'O');
        attroff(COLOR_PAIR(1));
        cur = cur->next;
    }
}

// 清理
void ui_cleanup(void) {
    endwin();
}
```

---

## ✅ 本课检查清单

- [ ] 会初始化 ncurses
- [ ] 理解坐标系统
- [ ] 会处理键盘输入
- [ ] 能用 ncurses 绘制图形

---

下一课：[状态机](14-状态机.md)

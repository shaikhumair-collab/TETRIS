# 🎮 Tetris Game in C

A classic Tetris game developed in C as a Programming Fundamentals project and later upgraded from a console application to a graphical game using raylib.

## ✨ Versions

- `project.c` — original console version created during Semester 1
- `tetris_gui.c` — updated graphical version built with raylib

## 📌 Features

- Classic Tetris gameplay
- Seven different tetrominoes
- Colored graphical blocks
- Keyboard movement and rotation
- Soft drop and hard drop
- Line clearing
- Score tracking
- Saved high-score system
- Pause, restart, and game-over screens
- Clickable graphical main menu
- Neon arcade visual theme

## 🎮 Controls

| Key | Action |
| --- | --- |
| `A` or Left Arrow | Move left |
| `D` or Right Arrow | Move right |
| `W` or Up Arrow | Rotate piece |
| `S` or Down Arrow | Soft drop |
| `Space` | Hard drop |
| `P` | Pause or resume |
| `Esc` | Return to the main menu |

## 🛠️ Technologies Used

- C programming language
- raylib graphics library
- GCC/MinGW compiler
- File handling for saved scores

## 📋 Requirements

Before compiling the graphical version, install raylib for Windows from the [official raylib website](https://www.raylib.com/).

## ▶️ Run the Graphical Version

If you installed the preconfigured raylib Windows package:

1. Open `tetris_gui.c` in **Notepad++ for raylib**.
2. Press **F6**.
3. Select `raylib_compile_execute`.
4. Click **OK**.

You can also compile it from a configured MinGW terminal:

```bash
gcc tetris_gui.c -o tetris_gui.exe -std=c99 -Wall -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
```

Then run:

```bash
./tetris_gui.exe
```

## ▶️ Run the Original Console Version

```bash
gcc project.c -o project.exe
./project.exe
```

> The original console version uses Windows-specific headers such as `conio.h` and `windows.h`.

## 💾 High Scores

The graphical version stores scores in `highscore.txt` beside the executable. This file is created automatically after a completed game.

## 🚀 Future Improvements

- Next-piece preview
- Increasing difficulty levels
- Background music and sound effects
- Hold-piece feature
- Improved animations

## 👤 Author

**Muhammad Umair**

Programming Fundamentals — Semester 1 Project

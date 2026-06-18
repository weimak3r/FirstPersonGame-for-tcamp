# FirstPersonGame for tcamp

## 🎮 UE5 Multiplayer Combat Demo — First-Person Game | UE5 多人对战Demo

**腾讯青科训练营 · 客户端方向 · 开局一课大作业**

---

### 📋 项目简介 | About

基于 UE5 First Person 模板开发的多人对战游戏 Demo，实现敌人 AI、计分系统、胜利机制和网络多人联机。

A multiplayer combat game demo built on UE5's First Person template, featuring enemy AI, scoring, win conditions, and networked multiplayer.

### ✨ 功能特性 | Features

- 🎯 **敌人 AI** — 敌人会自动索敌并攻击玩家，玩家可开枪击败敌人
- 📊 **计分系统** — 击败敌人获得分数，实时 HUD 显示
- 🏆 **胜利机制** — 达到目标分数即获胜
- 🌐 **多人联机** — 支持网络多人对战
- 🎨 **蓝图资源** — BP_FPCharacter / BP_FPEnemy / BP_FPGameMode / WBP_GameHUD

### 🛠 技术实现 | Tech Stack

| 模块 | 实现方式 |
|------|---------|
| 玩家角色 | C++ `AFPCharacter` + 蓝图 BP_FPCharacter |
| 敌人 AI | C++ `AFPEnemy` + AI Controller 行为树 |
| 游戏模式 | C++ `AFPGameMode` + 蓝图 BP_FPGameMode |
| 计分系统 | C++ `AFPGameState`（网络同步） |
| HUD | C++ `UFPGameHUDWidget` + 蓝图 WBP_GameHUD |
| 玩家控制器 | C++ `AFPPlayerController` |

### 🚀 运行方式 | How to Run

1. 使用 **Unreal Engine 5** 打开 `FirstPersonGame.uproject`
2. 编译 C++ 项目（如提示）
3. 点击 Play 运行单人模式
4. 多人模式：设置 Net Mode → Play as Listen Server / Client

### 📁 项目结构 | Structure

```
FirstPersonGame/
├── Source/              # C++ 源码
├── Content/             # 蓝图、地图、UI 资源
├── Config/              # 项目配置
├── 提交材料/             # Demo视频脚本 + PDF
├── FirstPersonGame.uproject
└── README.md
```

### 👤 作者 | Author

**weimak3r**

---

> 提交时间：2026年6月21日前 | 腾讯青科训练营客户端大作业

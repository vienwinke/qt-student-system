# C++ 学生成绩管理系统 (Qt 桌面版)

基于 **Qt Widgets** 的学生成绩管理桌面应用，复用原生 MySQL C API 实现数据库操作。

## 功能
- 学生：添加、删除、查询（表格展示列表）
- 成绩：添加、删除（自动按权重计算总分）
- 权重：从 `subject_weight` 表读取（自动计算）
- 数据实时刷新，图形界面操作

## 技术栈
- C++17 + Qt Widgets
- MySQL C API (libmysqlclient)
- qmake / CMake 构建

## 运行环境
### Linux (WSL / Ubuntu)
```bash
# 1. 安装依赖
sudo apt install -y g++ make qtbase5-dev libqt5widgets5 libmysqlclient-dev

# 2. 编译
qmake6 StudentSystem.pro && make
# 或 cmake 方式
```

### Windows (MSVC + MySQL Connector)
1. 安装 Qt（官网下载 Qt Creator，含 MSVC 工具链）
2. 安装 MySQL Connector/C++
3. 修改 `StudentSystem.pro` 中的 INCLUDEPATH 和 LIBS 为实际路径
4. Qt Creator 打开 `StudentSystem.pro` 编译运行

## 配置数据库
```bash
# 1. 启动 MySQL，执行初始化脚本
mysql -u root -p < ../c-main/c-main/sql/student_sys.sql

# 2. 设置密码环境变量
export DB_PASSWORD=123456

# 3. 运行程序
./StudentSystem
```

## 数据库结构
- `student(sid, sname)` — 学生表
- `score(sid, subject, usual_score, end_score, final_score)` — 成绩表
- `subject_weight(subject_name, weight_ratio)` — 权重表

## 总分计算公式
```
总分 = 权重 × 平时分 + (1 - 权重) × 期末分
```

## 文件说明
| 文件 | 说明 |
|------|------|
| `StudentSystem.pro` | qmake 项目文件 |
| `MainWindow.h/cpp` | 主窗口界面与业务逻辑 |
| `Student.h/cpp` | 学生数据模型 + MySQL CRUD |
| `Score.h/cpp` | 成绩数据模型 + MySQL CRUD |
| `Calculator.h/cpp` | 总分计算工具 |
| `main.cpp` | 程序入口 |

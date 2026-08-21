#!/bin/bash
# Qt 学生管理系统启动脚本（带中文输入法支持）

# 设置中文输入法
export GTK_IM_MODULE=fcitx
export QT_IM_MODULE=fcitx
export XMODIFIERS=@im=fcitx

# 设置图形显示
export DISPLAY=:0
export XDG_RUNTIME_DIR=/mnt/wslg

# 设置数据库密码
export DB_PASSWORD=${DB_PASSWORD:-123456}

# 启动 fcitx（如果未运行）
if ! pgrep -x fcitx > /dev/null; then
    echo "启动 fcitx 输入法..."
    fcitx &
    sleep 2
fi

# 进入程序目录
cd "$(dirname "$0")"

# 运行程序
echo "启动学生成绩管理系统..."
./StudentSystem

#include <QApplication>
#include <QFont>
#include "MainWindow.h"
#include "Student.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
    // 数据库密码从环境变量读取（Windows/Linux 通用）
    const char *pwd = getenv("DB_PASSWORD");
    if (!pwd || !*pwd) {
        std::cerr << "[警告] 未设置 DB_PASSWORD 环境变量，使用默认值 123456\n";
        setenv("DB_PASSWORD", "123456", 1);
    }

    QApplication app(argc, argv);
    
    // 设置全局中文字体（包括标题栏）
    QFont font("WenQuanYi Zen Hei", 10);
    font.setStyleHint(QFont::SansSerif);
    app.setFont(font);
    
    MainWindow w;
    w.show();
    return app.exec();
}

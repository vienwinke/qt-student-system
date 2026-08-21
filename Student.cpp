#include "Student.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

static MYSQL* g_conn = nullptr;

MYSQL* getDBConn() {
    if (g_conn) {
        // 添加重连检测
        if (mysql_ping(g_conn) != 0) {
            mysql_close(g_conn);
            g_conn = nullptr;
        } else {
            return g_conn;
        }
    }
    g_conn = mysql_init(NULL);
    if (!g_conn) return nullptr;
    const char* dbPwd = getenv("DB_PASSWORD");
    if (!dbPwd) dbPwd = "123456";
    if (!mysql_real_connect(g_conn, "127.0.0.1", "root", dbPwd, "student_sys", 3306, NULL, 0)) {
        std::cerr << "连接 MySQL 失败：" << mysql_error(g_conn) << std::endl;
        mysql_close(g_conn); g_conn = nullptr; return nullptr;
    }
    if (mysql_set_character_set(g_conn, "utf8mb4") != 0) {
        mysql_close(g_conn); g_conn = nullptr; return nullptr;
    }
    return g_conn;
}

void closeDBConn() {
    if (g_conn) { mysql_close(g_conn); g_conn = nullptr; }
}

void Student::setInfo(const std::string& id, const std::string& n) { sid = id; sname = n; }
void Student::show() { std::cout << "学号:" << sid << " 姓名:" << sname << std::endl; }
std::string Student::getSid() { return sid; }
std::string Student::getSname() { return sname; }
void Student::setSid(const std::string& id) { sid = id; }
void Student::setSname(const std::string& n) { sname = n; }

bool AddStudent(Student stu) {
    MYSQL* conn = getDBConn();
    if (!conn) return false;
    size_t sid_len = stu.getSid().length();
    size_t name_len = stu.getSname().length();
    std::vector<char> esc_sid(sid_len * 2 + 1);
    std::vector<char> esc_name(name_len * 2 + 1);
    mysql_real_escape_string(conn, esc_sid.data(), stu.getSid().c_str(), sid_len);
    mysql_real_escape_string(conn, esc_name.data(), stu.getSname().c_str(), name_len);
    std::string q = "INSERT INTO student(sid, sname) VALUES('" + std::string(esc_sid.data()) + "', '" + std::string(esc_name.data()) + "')";
    if (mysql_query(conn, q.c_str()) != 0) {
        std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

Student QueryStudentBySid(const std::string& targetSid) {
    Student res;
    MYSQL* conn = getDBConn();
    if (!conn) return res;
    size_t sid_len = targetSid.length();
    std::vector<char> esc(sid_len * 2 + 1);
    mysql_real_escape_string(conn, esc.data(), targetSid.c_str(), sid_len);
    std::string q = "SELECT sid,sname FROM student WHERE sid='" + std::string(esc.data()) + "'";
    if (mysql_query(conn, q.c_str()) == 0) {
        MYSQL_RES* result = mysql_store_result(conn);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) res.setInfo(row[0] ? row[0] : "", row[1] ? row[1] : "");
            mysql_free_result(result);
        }
    }
    return res;
}

bool UpdateStudentName(const std::string& targetSid, const std::string& newName) {
    MYSQL* conn = getDBConn();
    if (!conn) return false;
    size_t sid_len = targetSid.length();
    size_t name_len = newName.length();
    std::vector<char> esc_sid(sid_len * 2 + 1);
    std::vector<char> esc_name(name_len * 2 + 1);
    mysql_real_escape_string(conn, esc_sid.data(), targetSid.c_str(), sid_len);
    mysql_real_escape_string(conn, esc_name.data(), newName.c_str(), name_len);
    std::string q = "UPDATE student SET sname='" + std::string(esc_name.data()) + "' WHERE sid='" + std::string(esc_sid.data()) + "'";
    if (mysql_query(conn, q.c_str()) != 0) {
        std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

bool DeleteStudent(const std::string& targetSid) {
    MYSQL* conn = getDBConn();
    if (!conn) return false;
    size_t sid_len = targetSid.length();
    std::vector<char> esc(sid_len * 2 + 1);
    mysql_real_escape_string(conn, esc.data(), targetSid.c_str(), sid_len);
    std::string q1 = "DELETE FROM score WHERE sid='" + std::string(esc.data()) + "'";
    std::string q2 = "DELETE FROM student WHERE sid='" + std::string(esc.data()) + "'";
    if (mysql_query(conn, q1.c_str()) != 0 || mysql_query(conn, q2.c_str()) != 0) {
        std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

#include "Score.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

void Score::setAll(const std::string& id, const std::string& sub, int usual, int end, int final)
{
    sid = id; subject = sub; usual_score = usual; end_score = end; final_score = final;
}

void Score::showScore()
{
    std::cout << "学号:" << sid << " 科目:" << subject
        << " 平时:" << usual_score << " 期末:" << end_score
        << " 成绩:" << final_score << std::endl;
}

std::string Score::getSid() { return sid; }
std::string Score::getSubject() { return subject; }
int Score::getUsual() { return usual_score; }
int Score::getEnd() { return end_score; }
int Score::getFinal() { return final_score; }
void Score::setSid(const std::string& id) { sid = id; }
void Score::setSubject(const std::string& sub) { subject = sub; }
void Score::setUsual(int s) { usual_score = s; }
void Score::setEnd(int s) { end_score = s; }
void Score::setFinal(int s) { final_score = s; }

bool AddScore(Score sc)
{
    MYSQL* conn = getDBConn();
    if (!conn) return false;
    size_t sid_len = sc.getSid().length();
    size_t sub_len = sc.getSubject().length();
    std::vector<char> esc_sid(sid_len * 2 + 1);
    std::vector<char> esc_sub(sub_len * 2 + 1);
    mysql_real_escape_string(conn, esc_sid.data(), sc.getSid().c_str(), sid_len);
    mysql_real_escape_string(conn, esc_sub.data(), sc.getSubject().c_str(), sub_len);
    std::string q = "INSERT INTO score(sid,subject,usual_score,end_score,final_score) VALUES('" + 
                    std::string(esc_sid.data()) + "','" + std::string(esc_sub.data()) + "'," +
                    std::to_string(sc.getUsual()) + "," + std::to_string(sc.getEnd()) + "," + 
                    std::to_string(sc.getFinal()) + ")";
    if (mysql_query(conn, q.c_str()) != 0) {
        std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

Score QueryScore(const std::string& sid, const std::string& sub)
{
    Score res;
    MYSQL* conn = getDBConn();
    if (!conn) return res;
    size_t sid_len = sid.length();
    size_t sub_len = sub.length();
    std::vector<char> esc_sid(sid_len * 2 + 1);
    std::vector<char> esc_sub(sub_len * 2 + 1);
    mysql_real_escape_string(conn, esc_sid.data(), sid.c_str(), sid_len);
    mysql_real_escape_string(conn, esc_sub.data(), sub.c_str(), sub_len);
    std::string q = "SELECT sid,subject,usual_score,end_score,final_score FROM score WHERE sid='" + 
                    std::string(esc_sid.data()) + "' AND subject='" + std::string(esc_sub.data()) + "'";
    if (mysql_query(conn, q.c_str()) == 0) {
        MYSQL_RES* result = mysql_store_result(conn);
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) res.setAll(row[0]?row[0]:"", row[1]?row[1]:"", row[2]?atoi(row[2]):0, row[3]?atoi(row[3]):0, row[4]?atoi(row[4]):0);
            mysql_free_result(result);
        }
    }
    return res;
}

bool DeleteScore(const std::string& sid, const std::string& sub)
{
    MYSQL* conn = getDBConn();
    if (!conn) return false;
    size_t sid_len = sid.length();
    size_t sub_len = sub.length();
    std::vector<char> esc_sid(sid_len * 2 + 1);
    std::vector<char> esc_sub(sub_len * 2 + 1);
    mysql_real_escape_string(conn, esc_sid.data(), sid.c_str(), sid_len);
    mysql_real_escape_string(conn, esc_sub.data(), sub.c_str(), sub_len);
    std::string q = "DELETE FROM score WHERE sid='" + std::string(esc_sid.data()) + "' AND subject='" + std::string(esc_sub.data()) + "'";
    if (mysql_query(conn, q.c_str()) != 0) {
        std::cerr << "SQL error: " << mysql_error(conn) << std::endl;
        return false;
    }
    return true;
}

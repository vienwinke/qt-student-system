#pragma once

#include <string>
#include <mysql.h>

class Score
{
private:
    std::string sid;
    std::string subject;
    int usual_score;
    int end_score;
    int final_score;
public:
    void setAll(const std::string& id, const std::string& sub, int usual, int end, int final);
    void showScore();
    std::string getSid();
    std::string getSubject();
    int getUsual();
    int getEnd();
    int getFinal();
    void setSid(const std::string& id);
    void setSubject(const std::string& sub);
    void setUsual(int s);
    void setEnd(int s);
    void setFinal(int s);
};

extern MYSQL* getDBConn();
bool AddScore(Score sc);
Score QueryScore(const std::string& sid, const std::string& sub);
bool DeleteScore(const std::string& sid, const std::string& sub);

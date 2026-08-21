#include "MainWindow.h"
#include "Student.h"
#include "Score.h"
#include "Calculator.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QTabWidget>
#include <QGroupBox>
#include <mysql.h>

#include <string>

static std::string qs2s(const QString &s) { return s.toUtf8().toStdString(); }

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Student Management System");
    resize(900, 600);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // 顶部按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAddStudent = new QPushButton("添加学生", this);
    QPushButton *btnDelStudent = new QPushButton("删除学生", this);
    QPushButton *btnQueryStudent = new QPushButton("查询学生", this);
    QPushButton *btnAddScore = new QPushButton("添加成绩", this);
    QPushButton *btnDelScore = new QPushButton("删除成绩", this);
    QPushButton *btnRefresh = new QPushButton("刷新", this);

    connect(btnAddStudent, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(btnDelStudent, &QPushButton::clicked, this, &MainWindow::deleteStudent);
    connect(btnQueryStudent, &QPushButton::clicked, this, &MainWindow::queryStudent);
    connect(btnAddScore, &QPushButton::clicked, this, &MainWindow::addScore);
    connect(btnDelScore, &QPushButton::clicked, this, &MainWindow::deleteScore);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshStudents);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshScores);

    btnLayout->addWidget(btnAddStudent);
    btnLayout->addWidget(btnDelStudent);
    btnLayout->addWidget(btnQueryStudent);
    btnLayout->addWidget(btnAddScore);
    btnLayout->addWidget(btnDelScore);
    btnLayout->addWidget(btnRefresh);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // 学生表格
    QGroupBox *studentBox = new QGroupBox("学生列表", this);
    QVBoxLayout *sl = new QVBoxLayout(studentBox);
    studentTable = new QTableWidget(this);
    studentTable->setColumnCount(4);
    studentTable->setHorizontalHeaderLabels({"学号", "姓名", "课程数", "平均分"});
    studentTable->horizontalHeader()->setStretchLastSection(true);
    sl->addWidget(studentTable);
    mainLayout->addWidget(studentBox, 1);

    // 成绩表格
    QGroupBox *scoreBox = new QGroupBox("成绩列表", this);
    QVBoxLayout *scl = new QVBoxLayout(scoreBox);
    scoreTable = new QTableWidget(this);
    scoreTable->setColumnCount(6);
    scoreTable->setHorizontalHeaderLabels({"学号", "姓名", "科目", "平时", "期末", "总分"});
    scoreTable->horizontalHeader()->setStretchLastSection(true);
    scl->addWidget(scoreTable);
    mainLayout->addWidget(scoreBox, 1);

    setCentralWidget(central);

    refreshStudents();
    refreshScores();
}

MainWindow::~MainWindow() {
    closeDBConn();
}

void MainWindow::refreshStudents() {
    MYSQL *conn = getDBConn();
    if (!conn) {
        QMessageBox::warning(this, "错误", "无法连接数据库！请检查 DB_PASSWORD 环境变量和 MySQL 服务。");
        return;
    }
    if (mysql_query(conn, "SELECT s.sid, s.sname, COUNT(sc.sid), COALESCE(ROUND(AVG(sc.final_score),1),0) "
                          "FROM student s LEFT JOIN score sc ON sc.sid = s.sid "
                          "GROUP BY s.sid, s.sname ORDER BY s.sid") == 0) {
        MYSQL_RES *res = mysql_store_result(conn);
        if (res) {
            int n = (int)mysql_num_rows(res);
            studentTable->setRowCount(n);
            MYSQL_ROW row;
            int i = 0;
            while ((row = mysql_fetch_row(res))) {
                studentTable->setItem(i, 0, new QTableWidgetItem(row[0] ? row[0] : ""));
                studentTable->setItem(i, 1, new QTableWidgetItem(row[1] ? row[1] : ""));
                studentTable->setItem(i, 2, new QTableWidgetItem(row[2] ? row[2] : "0"));
                studentTable->setItem(i, 3, new QTableWidgetItem(row[3] ? row[3] : "0"));
                i++;
            }
            mysql_free_result(res);
        }
    }
}

void MainWindow::refreshScores() {
    MYSQL *conn = getDBConn();
    if (!conn) return;
    if (mysql_query(conn, "SELECT sc.sid, s.sname, sc.subject, sc.usual_score, sc.end_score, sc.final_score "
                          "FROM score sc JOIN student s ON s.sid = sc.sid "
                          "ORDER BY sc.sid, sc.subject") == 0) {
        MYSQL_RES *res = mysql_store_result(conn);
        if (res) {
            int n = (int)mysql_num_rows(res);
            scoreTable->setRowCount(n);
            MYSQL_ROW row;
            int i = 0;
            while ((row = mysql_fetch_row(res))) {
                scoreTable->setItem(i, 0, new QTableWidgetItem(row[0] ? row[0] : ""));
                scoreTable->setItem(i, 1, new QTableWidgetItem(row[1] ? row[1] : ""));
                scoreTable->setItem(i, 2, new QTableWidgetItem(row[2] ? row[2] : ""));
                scoreTable->setItem(i, 3, new QTableWidgetItem(row[3] ? row[3] : "0"));
                scoreTable->setItem(i, 4, new QTableWidgetItem(row[4] ? row[4] : "0"));
                scoreTable->setItem(i, 5, new QTableWidgetItem(row[5] ? row[5] : "0"));
                i++;
            }
            mysql_free_result(res);
        }
    }
}

void MainWindow::addStudent() {
    bool ok1 = false, ok2 = false;
    QString sid = QInputDialog::getText(this, "添加学生", "学号:", QLineEdit::Normal, "", &ok1);
    if (!ok1 || sid.isEmpty()) return;
    QString name = QInputDialog::getText(this, "添加学生", "姓名:", QLineEdit::Normal, "", &ok2);
    if (!ok2 || name.isEmpty()) return;

    Student s;
    s.setInfo(qs2s(sid), qs2s(name));
    if (AddStudent(s)) {
        QMessageBox::information(this, "成功", "学生添加成功");
        refreshStudents();
    } else {
        QMessageBox::warning(this, "错误", "学生添加失败，学号可能已存在");
    }
}

void MainWindow::deleteStudent() {
    int row = studentTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选中一个学生"); return; }
    QString sid = studentTable->item(row, 0)->text();
    if (QMessageBox::question(this, "确认", "确定删除学生 " + sid + " 及其全部成绩？") != QMessageBox::Yes) return;
    if (DeleteStudent(qs2s(sid))) {
        refreshStudents();
        refreshScores();
    } else {
        QMessageBox::warning(this, "错误", "删除失败");
    }
}

void MainWindow::queryStudent() {
    bool ok = false;
    QString sid = QInputDialog::getText(this, "查询学生", "学号:", QLineEdit::Normal, "", &ok);
    if (!ok || sid.isEmpty()) return;
    Student s = QueryStudentBySid(qs2s(sid));
    if (s.getSid().empty()) {
        QMessageBox::information(this, "结果", "未找到学号为 " + sid + " 的学生");
    } else {
        QMessageBox::information(this, "结果",
            "学号：" + QString::fromUtf8(s.getSid().c_str()) + "\n姓名：" + QString::fromUtf8(s.getSname().c_str()));
    }
}

void MainWindow::addScore() {
    bool ok = false;
    QString sid = QInputDialog::getText(this, "添加成绩", "学号:", QLineEdit::Normal, "", &ok);
    if (!ok || sid.isEmpty()) return;
    
    // 验证学号是否存在
    Student stu = QueryStudentBySid(qs2s(sid));
    if (stu.getSid().empty()) {
        QMessageBox::warning(this, "错误", "学号不存在，请先添加学生");
        return;
    }
    
    QString subject = QInputDialog::getText(this, "添加成绩", "科目:", QLineEdit::Normal, "", &ok);
    if (!ok || subject.isEmpty()) return;
    int usual = QInputDialog::getInt(this, "添加成绩", "平时分 (0-100):", 80, 0, 100);
    int end = QInputDialog::getInt(this, "添加成绩", "期末分 (0-100):", 80, 0, 100);

    // 查询权重
    double w = 0.5;
    MYSQL *conn = getDBConn();
    if (conn) {
        size_t sub_len = qs2s(subject).length();
        std::vector<char> esc(sub_len * 2 + 1);
        mysql_real_escape_string(conn, esc.data(), qs2s(subject).c_str(), sub_len);
        std::string q = "SELECT weight_ratio FROM subject_weight WHERE subject_name='" + std::string(esc.data()) + "'";
        if (mysql_query(conn, q.c_str()) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            if (res && mysql_num_rows(res) > 0) {
                MYSQL_ROW r = mysql_fetch_row(res);
                if (r && r[0]) w = atof(r[0]);
            }
            if (res) mysql_free_result(res);
        }
    }

    int final_score = CalculateFinalScore(usual, end, w);
    Score sc;
    sc.setAll(qs2s(sid), qs2s(subject), usual, end, final_score);
    if (AddScore(sc)) {
        QMessageBox::information(this, "成功", "成绩添加成功！总分=" + QString::number(final_score));
        refreshScores();
        refreshStudents();
    } else {
        QMessageBox::warning(this, "错误", "成绩添加失败，可能已存在");
    }
}

void MainWindow::deleteScore() {
    int row = scoreTable->currentRow();
    if (row < 0) { QMessageBox::warning(this, "提示", "请先选中一条成绩"); return; }
    QString sid = scoreTable->item(row, 0)->text();
    QString subject = scoreTable->item(row, 2)->text();
    if (QMessageBox::question(this, "确认", "确定删除 " + sid + " 的 " + subject + " 成绩？") != QMessageBox::Yes) return;
    if (DeleteScore(qs2s(sid), qs2s(subject))) {
        refreshScores();
        refreshStudents();
    } else {
        QMessageBox::warning(this, "错误", "删除失败");
    }
}

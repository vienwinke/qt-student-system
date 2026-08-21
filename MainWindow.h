#pragma once

#include <QMainWindow>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshStudents();
    void refreshScores();
    void addStudent();
    void deleteStudent();
    void addScore();
    void deleteScore();
    void queryStudent();

private:
    QTableWidget *studentTable;
    QTableWidget *scoreTable;
};

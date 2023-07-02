#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPixmap>
#include <QDate>
#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include "multithread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //Set everything properly to start program
    void Starter();


    //Detect all cams found in system
    void detect_cams();

    //Show in QWidgetTable P or A, wich cam got selected
    void setCam(QString type, int row);

    //Enable right click in Table
    void RightClick();

    //Create TXT file with baby data
    void createTxt();

    //Change test button color to green
    void testCompleted(int test, QString color);

    //To pause without crashing gui
    void delay(int n);

    //If any user input error, return true
    bool checkErrors();

    //Create baby folder
    void makeFolder();
    //Check video file creation
    void checkFileCreation();


private slots:

    //User Texts data
    void on_babyName_editingFinished();//Baby name
    void on_dateEdit_editingFinished();// Birth date
    void on_momName_editingFinished();//Mom Name
    void on_lineEdit_editingFinished();// Baby week
    //Test buttons
    void on_test_1_clicked();
    void on_test_2_clicked();
    void on_test_3_clicked();
    void on_test_4_clicked();

    //Left click in table
    void on_TableWidget_Cams_cellClicked(int row, int column);

    //Reset button
    void on_Reset_all_clicked(){ Starter(); }

    // Re-scan cameras
    void button_rescan();

    //Record clicked
    void on_record_stop_clicked(bool checked);

    //Get thread frame
    void getFrame(std::string winName, cv::Mat frame);




private:
    Ui::MainWindow *ui;

    QString sysCommand;//Printar comando no sistema

    QVector<int> cameras; //all(max 10)  cameras find in computer
    int principalCam;
    int auxiliarCam;
    int currentTest;//Current tast selected

    QString babyName;
    QString momName;
    QString idadeCronologica;
    QString idadeCorrigida;

    int m_semanas;

    QDate Birth;

    QVector<bool> tests; //each test made
    std::atomic<int> waitSync;//Wait for threads to sync
    std::atomic<bool> recordChecked;

    MultiThread *M_principal;
    MultiThread *M_auxiliar;

    QDir P_folder;
    QDir A_folder;
    QDir atualFolder;

};
#endif // MAINWINDOW_H

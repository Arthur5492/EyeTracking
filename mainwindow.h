#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <opencv2/opencv.hpp>
#include <QProcess>
#include <QPixmap>
#include <QDate>
#include <QFont>
//#include <QDir>
//#include <string>
//#include <QThread>
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

    //Record cam //cvWindowTitle false = principal, true = auxiliar
    QDir record_cam(QString filename, std::string winName);

    //Show in QWidgetTable wich cam got selected
    void setCam(QString type, int row);

    //Enable right click in Table
    void RightClick();

    //Create TXT file with baby data
    void createTxt();

    //Change test button color to green
    void testCompleted(int test, QString color);

    void delay(int n);

private slots:

    //User Texts data
    void on_babyName_editingFinished();//Baby name
    void on_dateEdit_editingFinished();// Birth date
    void on_momName_editingFinished();//Mom Name

    //Test buttons
    void on_test_1_clicked();
    void on_test_2_clicked();
    void on_test_3_clicked();
    void on_test_4_clicked();

    //Left click in table
    void on_TableWidget_Cams_cellClicked(int row, int column);

    //Record clicked
//    void on_record_stop_clicked();

    //Reset button
    void on_Reset_all_clicked(){ Starter(); }

    // Re-scan cameras
    void button_rescan();



    void on_record_stop_clicked(bool checked);

private:
    Ui::MainWindow *ui;

    QString sysCommand;//Printar comando no sistema

    QVector<int> cameras;
    int principalCam;
    int auxiliarCam;
    int currentTest;//Current tast selected

    QString babyName;
    QString momName;
    QString idadeCronologica;
    QString idadeCorrigida;

    QDate Birth;

    QVector<bool> tests; //each test made
    bool isRecordClicked; //If record already clicked one time
    std::atomic<bool> recordChecked;

    MultiThread *M_principal;
    MultiThread *M_auxiliar;

    QDir P_folder;
    QDir A_folder;

};
#endif // MAINWINDOW_H
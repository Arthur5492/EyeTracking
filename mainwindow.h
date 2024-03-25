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
#include <QMessageBox>
#include "multithread.h"
#include "processconfig.h"


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

    //Change test button color
    void testCompleted(int test, QString color);

    //To pause without crashing gui
    void delay(int n);

    //If any user input error, return true
    bool checkErrors();

    //Create baby folder
    void makeFolder();

    //Check video file creation
    void checkFileCreation();

    //Make baby age in Months
    QString makeMonth(double days);

private slots:

    //User Texts data
    void on_babyName_editingFinished();//Baby name
    void on_dateEdit_editingFinished();// Birth date
    void on_momName_editingFinished();//Mom Name
    void on_gravidez_editingFinished();// Baby week

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
    void useFrame(std::string winName, const cv::Mat frame);

    //Nao esta funcionando ainda
    void setWindow();

    //Sinal de que video esta processando(nao funciona ainda)
    void getFinishRecord();

    //Botao para configurar e iniciar reconhecimento facial
    void on_ConfigEyeTracker_clicked();

    //Sinal de que alguma camera parou de funcionar
    void getIfCamStopped();

private:
    Ui::MainWindow *ui;
    ProcessConfig *uiProcess;

    QString sysCommand;//Printar comando no sistema
    QString babyName;
    QString momName;
    QString idadeCronologica;
    QString idadeCorrigida;

    QString filename_princ;//Principal Cam file
    QString filename_aux;//Auxiliar Cam file
    QDir babyFolder;//Folder baby

    QDate Birth;// born date

    QVector<int> cameras; //all(max 10)  cameras found on computer
    int principalCam;//Princiapal cam int number
    int auxiliarCam;//Auxiliar Cam int number
    int currentTest;//Current tast selected
    int semanasGravidez;//Weeks born
    std::atomic<int> onlyOnce;
    std::atomic<int> waitSync;//Wait for threads to sync
    int correctDays; // dias que bebe tem com a idade corrigida

    QVector<bool> tests; //each test made
    std::atomic<bool> recordChecked; //If record button was clicked, to stop threads
    bool once_princ;//Change namedWindow values only one time per thread
    bool once_aux;//Change namedWindow values only one time per thread
    bool dateChecked;//Para nao chamar o calculo das idades em semanasGravidez, antes de colocar a data

    MultiThread *M_principal;//Thread 1
    MultiThread *M_auxiliar;//Thread 2
};
#endif // MAINWINDOW_H

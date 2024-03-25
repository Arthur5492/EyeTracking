#ifndef PROCESSCONFIG_H
#define PROCESSCONFIG_H

#include "QCheckBox"
#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <cstring>
#include<QIODevice>
#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QByteArray>
#include <QProcess>//ESSEEEEEEEEEEEE HAHAHAHAA CMD

//Colocar explorador de arquivos essse ainda
//Enviar os videos selecionados pra pasta raw ..TO NESSEEEEEEEEEEEEEEEEEEEEEEE
//Executar no cmd o processo com QProcess e esperar acabar
//Apagar do vds/raw joia
//Criar um botão pra acessar a pasta com os videos editados

//Dps de tudo colocar a api em python

namespace Ui {
class ProcessConfig;
}

class ProcessConfig : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessConfig(QWidget *parent = nullptr);
    ~ProcessConfig();

    //Create txt or set the default settings
    void createUserConfig();

    //Set user config variables
    void manageUserConfig();

    void Save();

    void checkMarks();

    void selectLine(int line);

    struct Parameter
    {
        QString name;
        char value;
    };

    void runCommand();


private slots:
    void on_Cancel_clicked();

    void on_Start_clicked();

    void on_pushButton_clicked();

    void on_chVideos_clicked();

private:
    Ui::ProcessConfig *ui;

    //txt with user settings
    QFile txtFile;

    //Parameters array
    std::array<Parameter,10> parameters;
    QString path;

    //ui checkboxes array
    std::array<QCheckBox*,9> checkboxes;

    //Vector of video files selected
    QString babyFolder;
    QStringList testFolders;

    QString atualFilename;
    //Local executable path
    QString executablePath;

    //Show how many videos are selected
    QString videoNames;

    QProcess *EyeTracker;



};

#endif // PROCESSCONFIG_H

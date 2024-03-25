#include "processconfig.h"
#include "ui_processconfig.h"
#include <iostream>

ProcessConfig::ProcessConfig(QWidget *parent):
    QDialog(parent),ui(new Ui::ProcessConfig){ ui->setupUi(this);

    executablePath = QCoreApplication::applicationDirPath();

    txtFile.setFileName("userconfig.txt");

    checkboxes = {this->ui->showprocess, this->ui->drawbb, this->ui->drawir, this->ui->drawpu,
                  this->ui->drawpp, this->ui->drawmp, this->ui->drawgz,
                  this->ui->multicore, this->ui->overwrite};

    manageUserConfig();

    videoNames = "";
    //change if name for the api changes
    atualFilename = "record.avi";
}

ProcessConfig::~ProcessConfig(){ delete ui; }
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::on_Cancel_clicked() { hide(); }
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::manageUserConfig()
{

    QTextStream stream(&txtFile);


    if(!txtFile.exists())
    {
        createUserConfig();
    }

    txtFile.open(QIODevice::ReadOnly | QIODevice::Text);

    for(size_t i = 0; i<parameters.size(); i++)
    {
        QString line = stream.readLine();
        int seeknum = line.indexOf(' ');
        QChar Qvalue = line.at(seeknum+1);
        char value = Qvalue.unicode();

        //variables array
        parameters[i].name = line;
        parameters[i].value = value;
    }

    txtFile.close();

    checkMarks();

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::createUserConfig()
{
    txtFile.resize(0);
    txtFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&txtFile);

    stream<<"-showprocess n\n";
    stream<<"-drawbb s\n";
    stream<<"-drawir s\n";
    stream<<"-drawpu s\n";
    stream<<"-drawpp s\n";
    stream<<"-drawmp s\n";
    stream<<"-drawgz s\n";
    stream<<"-multicore n\n";
    stream<<"-overwrite s\n";
    stream<<"-showwarn s\n";

    txtFile.close();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::checkMarks()
{
    for(size_t i = 0; i<checkboxes.size();i++)
    {
        if(parameters[i].value == 's')
            checkboxes[i]->setChecked(true);
        else
            checkboxes[i]->setChecked(false);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::Save()
{

    for(size_t i=0; i<checkboxes.size();i++)
    {
        //Atualiza valor da variavel
        checkboxes[i]->isChecked() ?
            parameters[i].value = 's'    :
            parameters[i].value = 'n';

        int space = parameters[i].name.indexOf(" ");
        //Replace the value to the QString
        parameters[i].name.replace(space+1, 1 ,parameters[i].value);
    }

    txtFile.resize(0);
    txtFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&txtFile);

    if(!txtFile.isOpen())
    {
        std::cerr<<"Error opening File";
        return;
    }

    for(size_t i=0;i<parameters.size();i++)
    {
        stream<<parameters[i].name+"\n";
    }

    txtFile.close();
    std::cerr<<"Values Stored"<<std::endl;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::on_Start_clicked()
{
    Save();

    //Install dependencies
//    std::system("cd tracker && pipenv install");


    //if no folder or folder empty
    if(babyFolder.isEmpty() || testFolders.isEmpty())
    {
        this->ui->chVideos->setStyleSheet("background-color:#d0312d;");
        this->ui->chVideos->setText("Selecione uma pasta correta");
        return;
    }

    //Just for general usage
    QDir c1;

    //Start command and create processed folder
    for(int i =0;i<testFolders.size();i++)
    {
        int index = babyFolder.lastIndexOf("/");
        QString babyName = babyFolder.mid(index+1)+"/";
        path = "-path \"" + babyFolder+"/"+ testFolders.at(i)+"\"";

        //Folder with the processed Folder
        QString processFolder = babyFolder+"/"+testFolders.at(i)+"/processed/";

        runCommand();

        //Insert in processed videos folder
        QString processed(executablePath+"/videos/processados/"+babyName+testFolders.at(i) );
        c1.mkpath(executablePath+"/videos/processados/"+babyName+testFolders.at(i) );

        QFile::copy(processFolder+"positions.csv", processed+"/positions.csv") ?
            std::cerr<<"positions.csv copied sucefully":
            std::cerr<<"Error copying positions.csv";

        QFile::copy(processFolder+"video.avi", processed+"/video.avi") ?
            std::cerr<<"video.avi copied sucefully":
            std::cerr<<"Error copying video.avi";

        //Delete process folder
        c1.cd(processFolder);
        if(c1.exists())
        {
//            c1.setFilter(QDir::NoDotAndDotDot);
//            c1.removeRecursively();
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::on_pushButton_clicked()
{
    createUserConfig();
    manageUserConfig();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::on_chVideos_clicked()
{
    QString path = executablePath + "/videos/normal";
    testFolders.clear();

    babyFolder = QFileDialog::getExistingDirectory(this,
    "clique na pasta nome e selecione as pastas dos testes",path);

    for(size_t i=0;i<4;i++)
    {
        QFile check = babyFolder+"/"+QString::number(i+1)+"/"+atualFilename;
        if(check.exists())
            testFolders.append(QString::number(i+1));
    }

    if(!babyFolder.isEmpty())
    {
        int index = babyFolder.lastIndexOf("/");

        if(testFolders.empty())
        {
            this->ui->chVideos->setStyleSheet("background-color:#d0312d;");
            this->ui->chVideos->setText(babyFolder.mid(index+1)+" Está vazio.");
            return;
        }

        this->ui->chVideos->setStyleSheet("background-color:rgb(144, 238, 144);");
        QString textToInsert = babyFolder.mid(index+1)+"| Testes com video: "+testFolders.join("/");
        this->ui->chVideos->setText(textToInsert);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ProcessConfig::runCommand()
{

    QStringList parameter;//Parameters stored

    //Insert all QStringList parameters in a QString
    for(size_t i=0;i<parameters.size();i++)
        parameter.append(parameters[i].name);

    //console command

    QString command = "cd tracker && pipenv run python main.py "+ parameter.join(" ") + " " + path;

    QByteArray byteArray = command.toUtf8();

    const char* syscommand = byteArray.constData();

    std::system(syscommand);
}

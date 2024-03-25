#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Starter(); //Set all variables

    //Enable on/off in button
    ui->record_stop->setCheckable(true);

   //To rescan
   connect(ui->button_rescan,SIGNAL(clicked()),this,SLOT(button_rescan()));

}
//////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    if(M_principal)
        delete M_principal;
    if(M_auxiliar)
        delete M_auxiliar;
    delete ui;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::detect_cams()
{
    cameras.clear();
    ui->TableWidget_Cams->setRowCount(0);
    int row_number = 0;


    for(int i = 0; i<10; i++)
    {
        cv::VideoCapture camera(i);
        cv::Mat frame;


        if(camera.isOpened())
        {

            QTableWidgetItem *imageItem= new QTableWidgetItem;
            QPixmap img_pxmap;
            camera>>frame;
            cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
            QImage image((uchar*)frame.data, frame.cols, frame.rows,frame.step, QImage::Format_RGB888);
            img_pxmap = QPixmap::fromImage(image);

            ui->TableWidget_Cams->insertRow(row_number);
            imageItem->setIcon(QIcon(img_pxmap));
            imageItem->setData(Qt::DecorationRole, img_pxmap.scaled(210,210));

            ui->TableWidget_Cams->setItem(row_number,0, imageItem);
            ui->TableWidget_Cams->verticalHeader()->setDefaultSectionSize(30);

            cameras.append(i);
            row_number++;
        }
        ui->TableWidget_Cams->resizeRowsToContents();


        camera.release();
    }

    if(cameras.size()==0)
        sysCommand = "Please insert a camera";
    else if(cameras.size()==1)
        sysCommand = "1 Camera found!!";
    else
        sysCommand = QString::number(cameras.size()) + " Cameras found!!";

    ui->command->setText(sysCommand);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::button_rescan()
{
    QPushButton *clone = new QPushButton("Re-scaning...",this);
    clone->setGeometry(ui->button_rescan->geometry());
    clone->show();
    ui->button_rescan->setHidden(true);
    detect_cams();
    delete clone;
    ui->button_rescan->setHidden(false);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_babyName_editingFinished(){babyName = ui->babyName->text();   }
void MainWindow::on_momName_editingFinished() { momName = ui->momName->text() ;   }
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_dateEdit_editingFinished()
{
    QDate ActualDate = QDate::currentDate();
    int daysRemaining;

    Birth = ui->dateEdit->date() ;

    int babyDays = Birth.daysTo(ActualDate);
    int semanas, dias;
    semanas = std::floor(babyDays/7);
    dias = babyDays%7;

    //If days != 0 print dias too
    dias == 0 ?
        idadeCronologica = QString::number(semanas)+" Semanas."   :
        idadeCronologica = QString::number(semanas)+" Semanas e " + QString::number(dias) + " dias.";

    //Insert
    ui->cronologicAge->setText(idadeCronologica);

              //36 semanas
    if(babyDays>=252)
    {
        ui->correctAge->setText("Idade esta certa");
        return;
    }

                              //40 semanas
    daysRemaining = babyDays - (280 -(semanasGravidez*7));

    QDate dateCorreta = Birth.addDays(daysRemaining);

    correctDays = dateCorreta.daysTo(ActualDate);
    dateCorreta = Birth.addDays(correctDays);
    correctDays = dateCorreta.daysTo(ActualDate);

    //Same with Correct Age
    semanas = std::floor(correctDays/7);
    dias = correctDays%7;

    dias == 0 ?
        idadeCorrigida = QString::number(semanas)+" Semanas."   :
        idadeCorrigida = QString::number(semanas)+" Semanas e " + QString::number(dias) + " dias.";
    //Insert

    ui->correctAge->setText(idadeCorrigida);

    dateChecked = true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_gravidez_editingFinished()
{
    semanasGravidez= ui->gravidez->text().toInt();

    if(dateChecked)
        on_dateEdit_editingFinished();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_test_1_clicked(){
    currentTest = 0;
    this->ui->command->setText("Test 1 Selected!");
}
void MainWindow::on_test_2_clicked(){
    currentTest = 1;
    this->ui->command->setText("Test 2 Selected!");
}
void MainWindow::on_test_3_clicked(){
    currentTest = 2;
    this->ui->command->setText("Test 3 Selected!");
}
void MainWindow::on_test_4_clicked(){
    currentTest = 3;
    this->ui->command->setText("Test 4 Selected!");
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_TableWidget_Cams_cellClicked(int row, int column)
{
    //If row is = auxiliarCam
    if(cameras.at(row) == auxiliarCam)
       auxiliarCam = -1;

    //If principalCam already set
    if(principalCam > -1)
    {
        //if equals row
        if(principalCam==cameras.at(row))
        {
            setCam(" ", row);
            principalCam = -1;
            return;
        }
        //if set in another row
        else
            setCam(" ", cameras.indexOf(principalCam));
    }

        setCam("P",row);
        principalCam = cameras.at(row);
        std::cerr<<"Principal: " <<principalCam<<std::endl;
        std::cerr<<"Auxiliar: "<<auxiliarCam<<std::endl;

}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::setCam(QString type, int row_number)
{
    QFont font;
    font.setPointSize(80);

    QTableWidgetItem* item = new QTableWidgetItem(type);

    item->setFont(font);
    item->setTextAlignment(Qt::AlignCenter);

    ui->TableWidget_Cams->setItem(row_number,1,item);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::RightClick()
{
    ui->TableWidget_Cams->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->TableWidget_Cams, &QWidget::customContextMenuRequested,this,[this](const QPoint& pos)
        {
            QModelIndex index = ui->TableWidget_Cams->indexAt(pos);
            int row = index.row(); // Get the row from the index

            //If row is = principalCam
            if(cameras.at(row) == principalCam)
                principalCam = -1;

            //If principalCam already set
            if(auxiliarCam > -1)
            {
                //if equals row
                if(auxiliarCam==cameras.at(row))
                {
                    setCam(" ", row);
                    auxiliarCam = -1;
                    return;
                }
                //if set in another row
                else
                    setCam(" ", cameras.indexOf(auxiliarCam));
            }

            setCam("A",row);
            auxiliarCam = cameras.at(row);
            std::cerr<<"Principal: "<<principalCam<<std::endl;
            std::cerr<<"Auxiliar: "<<auxiliarCam<<std::endl;
        });
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::testCompleted(int test, QString color)
{
    if(test==0)
        MainWindow::ui->test_1->setStyleSheet("background-color:" + color);
    else if(test==1)
        MainWindow::ui->test_2->setStyleSheet("background-color:" + color);
    else if(test==2)
       MainWindow:: ui->test_3->setStyleSheet("background-color:" + color);
    else if(test ==3)
        MainWindow::ui->test_4->setStyleSheet("background-color:" + color);

}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::Starter()
{
/////////////////Variables_Reset//////////////////////////////////////
    //Clear QString
    idadeCronologica =
    sysCommand       =
    babyName         =
    momName          =
    filename_aux     =
    filename_princ   =
    idadeCorrigida   = "";
    //

    //Clear QDir
    QDir empty;
    babyFolder = empty;
    //


    //Clear QDate
    QDate clearDate;
    Birth = clearDate;
    //

    //Clear QVector<int> and int
    cameras.clear();
    principalCam =
    auxiliarCam  =
    currentTest  = -1;

    semanasGravidez =
    correctDays     =
    waitSync        = 0;

    onlyOnce.store(0);
    //

    //Clear QVector<bool> and bool
    tests = QVector<bool>(3,false);
    recordChecked =
    dateChecked   =
    waitSync      =
    once_princ    =
    once_aux      = false;
    //
///////////////////////////////////////////////////////
    //Return old test_button color
    testCompleted(0,""); testCompleted(1,"");
    testCompleted(2,""); testCompleted(3,"");
    //
    //Functions
    RightClick();
    detect_cams();
    //
    //Re-format texts
    QDate reset(2023,01,01);
    ui->dateEdit->setDate(reset);
    ui->babyName->clear();
    ui->momName->clear();
    ui->correctAge->clear();
    ui->cronologicAge->clear();
    ui->gravidez->clear();
    //
    //Start Message
    ui->command->setText("Welcome to BabyTracker!!");
    //Threads
    M_auxiliar = nullptr;
    M_principal = nullptr;
    //To get only numbers in week QLine
    QIntValidator *validator = new QIntValidator(this);
    ui->gravidez->setValidator(validator);
    //
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::createTxt()
{
    QString executablePath = QCoreApplication::applicationDirPath() + "/videos/normal/"+babyName+"_"+momName+"/data.txt";
    QFile file(executablePath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream<<"//////////////////////////////"+babyName+"//////////////////////////////";
        stream<<"\n \n";
        stream<<"Nome do bebe: " + babyName + "\n";
        stream<<"Nome da mae: " + momName + "\n";
        stream<<"Nascimento: " + Birth.toString("dd/MM/yyyy")+ "\n";
        stream<<"Idade Cronologica: " + idadeCronologica + "\n";
        if(idadeCorrigida!="")
            stream<<"Idade Corrigida: "+idadeCorrigida + "\n";
        stream<<"Idade Cronologica em MESES: " + makeMonth(double(semanasGravidez*7)) + "\n";
        if(idadeCorrigida!="")
            stream<<"Idade Corrigida em MESES: "+ makeMonth(double(correctDays))+ "\n";
    }
    else
        std::cerr<<"Could not create/open data.txt"<<std::endl;
    file.close();
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::delay(int n)
{
    QTime dieTime= QTime::currentTime().addMSecs(n);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_record_stop_clicked(bool checked)
{

    recordChecked.store(checked); //Atomic<bool> variable for threads stop management


    if(!checked)
    {

        if(M_auxiliar)
            while(M_auxiliar->isRunning() && M_principal->isRunning())
                ;
        else
            while(M_principal->isRunning())
                ;

        ui->record_stop->setText("Gravar");

        M_principal->exit();
        if(M_auxiliar)
            M_auxiliar->exit();

        checkFileCreation();

        waitSync = 0;//Semaphor
        once_princ = false;
        once_aux = false;

        cv::destroyAllWindows();
    }
    //Button Clicked
    else if(checked)
    {
        onlyOnce = 0;//removing useless processing of setWindow

        //Return true if error found
        if(checkErrors()==true) return;

//        if(!M_auxiliar )
        makeFolder();
        createTxt();
        this->ui->command->setText("Aguarde as câmeras aparecerem!");
        this->ui->record_stop->setText("Iniciando...");

        cv::Mat P_frame;
        cv::Mat A_frame;

        if(principalCam != -1 && !M_principal)
        {
            M_principal = new MultiThread("Principal",principalCam, &recordChecked, &waitSync, P_frame);
            //Emit frame from threads
            connect(M_principal, &MultiThread::sendFrame,this, &MainWindow::useFrame, Qt::QueuedConnection);
            connect(M_principal, &MultiThread::sendWindow,this,&MainWindow::setWindow, Qt::QueuedConnection);
            connect(M_principal, &MultiThread::sendFinishRecord, this,&MainWindow::getFinishRecord, Qt::QueuedConnection);
            connect(M_principal, &MultiThread::returnIfNoCam,this,&MainWindow::getIfCamStopped, Qt::QueuedConnection);
        }

        if(auxiliarCam != -1 && !M_auxiliar)
        {
            M_auxiliar = new MultiThread("Auxiliar",auxiliarCam, &recordChecked, &waitSync, A_frame);
            //Emit frame from threads
            connect(M_auxiliar, &MultiThread::sendFrame, this,&MainWindow::useFrame, Qt::QueuedConnection);
            connect(M_auxiliar, &MultiThread::sendWindow,this,&MainWindow::setWindow, Qt::QueuedConnection);
            connect(M_auxiliar, &MultiThread::sendFinishRecord,this,&MainWindow::getFinishRecord, Qt::QueuedConnection);
            connect(M_auxiliar, &MultiThread::returnIfNoCam,this,&MainWindow::getIfCamStopped, Qt::QueuedConnection);
        }

        if(M_principal)
        {
            M_principal->setCam(principalCam);
            M_principal->setFileName(filename_princ);
            M_principal->start();
        }

        if(M_auxiliar)
        {

            M_auxiliar->setCam(auxiliarCam);
            M_auxiliar->setFileName(filename_aux);
            M_auxiliar->start();
        }
        else
            waitSync++;

    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
bool MainWindow::checkErrors()
{
    //Verify User Variables
    if(principalCam == -1)
    {
      ui->command->setText("Please select a camera first!");
      return true;
    }
    else if(currentTest == -1)
    {
      ui->command->setText("Please select a test first!");
      return true;
    }
    else if(babyName =="" || momName =="")
    {
      ui->command->setText("Please insert baby and mom name");
      return true;
    }
    else
      return false;

}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::makeFolder()
{

    filename_princ =QString::number(currentTest+1)+"/record.avi";
    filename_aux   =QString::number(currentTest+1)+"/auxiliary.avi";

    babyFolder = QCoreApplication::applicationDirPath();

    babyFolder.mkpath("videos/normal");
    babyFolder.cd("videos/normal");

    QString babyFolder2 = babyName+"_"+momName;//Create babyFolder tests
    babyFolder.mkpath(babyFolder2);
    babyFolder.cd(babyFolder2);

    //Create the test respective folder
      babyFolder.mkdir(QString::number(currentTest+1));

    filename_princ = babyFolder.filePath(filename_princ);
    filename_aux = babyFolder.filePath(filename_aux);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::checkFileCreation()
{
    QFile principalFile(filename_princ);
    //Check if file was created
    if(principalFile.exists())
    {
      MainWindow::ui->command->setText("Test "+QString::number(currentTest+1)+" suceffuly recorded!");
      testCompleted(currentTest,"rgb(144, 238, 144);");
    }
    else
    {
      //Remover arquivos temporarios criados pelo VideoWriter
      //principal
      if(!filename_princ.isEmpty())
      {
            QString rmPrincipalstr = filename_princ;
            QFile rmPrincipal = rmPrincipalstr;
            int index = rmPrincipalstr.lastIndexOf("/");
            rmPrincipalstr = filename_princ.first(index) + rmPrincipalstr.mid(index)+"temp.avi";
            if(M_principal)
                while(M_principal->isRunning())
                    ;
            QFile::remove(rmPrincipalstr);
      }
      //aux
      if(!filename_aux.isEmpty())
      {
        QString rmAuxiliarstr = filename_aux;
        QFile rmAuxiliar = rmAuxiliarstr;
        int index = rmAuxiliarstr.lastIndexOf("/");
        rmAuxiliarstr = filename_aux.first(index) + rmAuxiliarstr.mid(index)+"temp.avi";
        if(M_auxiliar)
            while(M_auxiliar->isRunning())
                ;
        QFile::remove(rmAuxiliarstr);
      }

      MainWindow::ui->command->setText("Cannot create test "+QString::number(currentTest+1)+" file.");
      testCompleted(currentTest,"#d0312d");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::useFrame(std::string winName, const cv::Mat frame)
{

    cv::imshow(winName, frame);

    if(!recordChecked)
        cv::destroyAllWindows();
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::setWindow()
{
    ui->command->setText("Clique no botão \"gravando\" para parar");
    ui->record_stop->setText("Gravando...");
}
//////////////////////////////////////////////////////////////////////////////////////////////
QString MainWindow::makeMonth(double value)//232
{
    value = value/30;// 7,733333333333333
    double months;
    double restDays = modf(value, &months); //restDays = 0.733333333333333 : months = 7

    double days = ceil(restDays*30);//22
    double weeks = floor(days/7);//3 semanas

    days = days - (weeks*2);//1 dia

    QString Months = QString::number(months) +" Meses ";
    if(weeks>0)
    {
            Months +=QString::number(weeks)+ " semanas ";
            if(days>0)
            Months+=" e " + QString::number(days) + " dias";
    }
    return Months;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::getFinishRecord()
{
    ui->command->setText("Processando video, pode demorar um pouco, aguarde sem clicar por favor :) ");
    ui->record_stop->setText("Salvando...");
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_ConfigEyeTracker_clicked()
{
    uiProcess = new ProcessConfig(this);
    uiProcess->show();
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::getIfCamStopped()
{
    QMessageBox::critical(nullptr, "Erro na camera", "Verifique a conexao das cameras selecionadas, e clique em rescan");

    on_record_stop_clicked(false);
}

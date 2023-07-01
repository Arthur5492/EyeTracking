#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   Starter(); //Set all variables

   //To rescan
   connect(ui->button_rescan,SIGNAL(clicked()),this,SLOT(button_rescan()));

   //Enable on/off in button
   ui->record_stop->setCheckable(true);
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
    RightClick();
    cameras.clear();
    ui->TableWidget_Cams->setRowCount(0);
    int row_number = 0;


    for(int i = 0; i<10; i++)
    {
        cv::VideoCapture test(i);
        cv::Mat frame;

        if(test.isOpened())
        {
            QTableWidgetItem *imageItem= new QTableWidgetItem;
            QPixmap img_pxmap;
            test>>frame;
            cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
            QImage image((uchar*)frame.data, frame.cols, frame.rows,frame.step, QImage::Format_RGB888);
            img_pxmap = QPixmap::fromImage(image);

            ui->TableWidget_Cams->insertRow(row_number);
            imageItem->setIcon(QIcon(img_pxmap));
            imageItem->setData(Qt::DecorationRole, img_pxmap.scaled(250,250));

            ui->TableWidget_Cams->setItem(row_number,0, imageItem);
            ui->TableWidget_Cams->verticalHeader()->setDefaultSectionSize(30);

            cameras.append(i);
            row_number++;
        }
        ui->TableWidget_Cams->resizeRowsToContents();
        test.release();
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
//    ui->button_rescan->setDisabled(true);
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

    int semanas,dias;
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
    daysRemaining = babyDays - (280 -(m_semanas*7));

    QDate dateCorreta = Birth.addDays(daysRemaining);

//    int correctDays = Birth.daysTo(dateCorreta);
//    correctDays = Birth.addDays(correctDays);
    int correctDays = dateCorreta.daysTo(ActualDate);
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
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::on_lineEdit_editingFinished()
{
    m_semanas = ui->lineEdit->text().toInt();
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
    //Clear QString
    sysCommand=babyName=momName= "";
    idadeCorrigida=idadeCronologica="";

    //Clear QVector<int> and int
    cameras.clear();
    currentTest=principalCam=auxiliarCam = -1;

    m_semanas = 0;

    //Clear QVector<bool> and bool
    tests = QVector<bool>(3,false);
    startRecord = false;

    //Clear QDate
    QDate clear;
    Birth = clear;


    //Return old test_button color
    testCompleted(0,""); testCompleted(1,"");
    testCompleted(2,""); testCompleted(3,"");
    //

    //Functions
    detect_cams();
    //

    //Re-format texts
    QDate reset(2023,01,01);
    ui->dateEdit->setDate(reset);
    ui->babyName->clear();
    ui->momName->clear();
    ui->correctAge->clear();
    ui->cronologicAge->clear();

    //Start Message
    ui->command->setText("Welcome to BabyTracker!!");

    //Threads
    M_auxiliar = nullptr;
    M_principal = nullptr;

    //Folders
    QDir empty;
    P_folder = A_folder = empty;
    atualFolder = atualFolder.filePath(QCoreApplication::applicationDirPath());

    //To get only numbers in week QLine
    QIntValidator *validator = new QIntValidator(this);
    ui->lineEdit->setValidator(validator);

}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::createTxt()
{
    QString executablePath = QCoreApplication::applicationDirPath() + "/Files/"+babyName+"_"+momName+"/data.txt";
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
            stream<<"Idade Corrigida: "+idadeCorrigida;
    }
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

        MainWindow::ui->record_stop->setText("Gravar");

        M_principal->exit();
        if(M_auxiliar)
            M_auxiliar->exit();

        checkFileCreation();

        cv::destroyAllWindows();

//        bool completed = std::all_of(tests.begin(), tests.end(),[](bool v){return v;});

//        if(completed)
//        {
//            this->ui->command->setText("All tests has been recorded!");
//        }


    }
    //Button Clicked
    else if(checked)
    {
        //Return true if error found
        if(checkErrors()==true) return;

//        if(!M_auxiliar )

        createTxt();
        makeFolder();
        this->ui->command->setText("Aguarde um momento!!");
        this->ui->record_stop->setText("Gravando...");

        if(principalCam != -1 && !M_principal)
            M_principal = new MultiThread("Principal",principalCam, &recordChecked, &startRecord);

        if(auxiliarCam != -1 && !M_auxiliar)
            M_auxiliar = new MultiThread("Auxiliar",auxiliarCam, &recordChecked, &startRecord);



        if(M_principal)
        {
            M_principal->setCam(principalCam);
            M_principal->setFileName(P_folder);
            M_principal->start();
        }

        if(M_auxiliar)
        {

            M_auxiliar->setCam(auxiliarCam);
            M_auxiliar->setFileName(A_folder);
            M_auxiliar->start();
        }


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
    QString filename_cam1;
    QString filename_cam2;

    filename_cam1 ="test_"+QString::number(currentTest+1)+"_Principal_cam.avi";
    filename_cam2 ="test_"+QString::number(currentTest+1)+"_Auxiliar_cam.avi";


    P_folder = atualFolder; //Executable folder
    P_folder.mkdir("Files");
    P_folder.cd("Files");

    //Create folder tests
    P_folder.mkpath(babyName+"_"+momName);
    P_folder.cd(babyName+"_"+momName);

    //Auxiliar Cam file
    A_folder = P_folder.filePath(filename_cam2);
    //Principal cam file
    P_folder = P_folder.filePath(filename_cam1);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::checkFileCreation()
{
    //Check if file was created
    if(!P_folder.exists())
    {
      MainWindow::ui->command->setText("Test "+QString::number(currentTest+1)+" suceffuly recorded!");
      testCompleted(currentTest,"rgb(144, 238, 144);");
//      tests[currentTest] = true;
    }
    else
      MainWindow::ui->command->setText("Cannot create test "+QString::number(currentTest+1)+" file.");
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::getFrame(std::string winName, cv::Mat frame)
{

}


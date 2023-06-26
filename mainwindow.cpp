#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   Starter();

   connect(ui->button_rescan,SIGNAL(clicked()),this,SLOT(button_rescan()));

   ui->record_stop->setCheckable(true);
}
//////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
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
        sysCommand = QString::number(cameras.size()) + " Camera found!!";

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

    Birth = ui->dateEdit->date()  ;

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
    daysRemaining = 280-babyDays;

    QDate dateCorreta = Birth.addDays(daysRemaining);

    int correctDays = Birth.daysTo(dateCorreta);

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
QDir MainWindow::record_cam(QString filename, std::string winName)
{

    int wichCam =0;
    winName=="Principal" ? wichCam = principalCam : wichCam = auxiliarCam;

        cv::VideoCapture cam(wichCam); //Open cam

    cv::VideoWriter video; //Class to write the frames

    int fourcc = cv::VideoWriter::fourcc('M','J','P','G'); //Set forcc variable

    QDir folder(QCoreApplication::applicationDirPath()); //Executable folder
    QDir babyFolder, nullFolder;//Baby path and a null folder for return

    cv::Mat frame; //Frame of video
    int fps = 0; //Fps


    folder.mkdir("Files");
    folder.cd("Files");
    //Check if cam open
    if(!cam.isOpened()){
        std::cerr<<"Cannot open VideoCapture cam "+ m_cam;
        QDir null;
        return null;
      }

    //Create folder tests
    if(folder.mkpath(babyName+"_"+momName) && !isRecordClicked)
        isRecordClicked = true;
    else
    {
        if(!isRecordClicked)
        {
//            MainWindow::ui->command->setText("Cannot create folder");

            return nullFolder;
        }
    }

    babyFolder = folder.filePath(babyName+"_"+momName);
    babyFolder = babyFolder.filePath(filename); //Add test(1,2,..) in babyFolder

    filename = babyFolder.absolutePath();

    cam>>frame; //To get fps value
    fps = cam.get(cv::CAP_PROP_FPS);
    cv::namedWindow(winName,cv::WINDOW_NORMAL); //Window

    winName=="Principal" ?

        cv::resizeWindow(winName, cv::Size(640,480));/*Principal windowSize */:
        cv::resizeWindow(winName, cv::Size(640,480));//Auxiliar windowSize

    video.open(filename.toStdString(), fourcc, fps, frame.size(), true);

    while(cam.read(frame))
    {
        video.write(frame);

        cv::imshow(winName,frame);

        if(cv::waitKey(1)==32 || recordChecked == true)
        {
            MainWindow::ui->record_stop->setText("Gravar");
            cv::destroyAllWindows();
            recordChecked = true;//turn off button
            break;
        }
    }

    cam.release();
    video.release();

    return babyFolder;
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

    //Clear QVector<bool> and bool
    tests = QVector<bool>(3,false);
    isRecordClicked = false;
    recordChecked = true;

    //Clear QDate
    QDate clear;
    Birth = clear;


    //Return old test_button color
    testCompleted(0,""); testCompleted(1,"");
    testCompleted(2,""); testCompleted(3,"");
    //

    //Functions
    detect_cams();
    RightClick();
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
        {
            stream<<"Idade Corrigida: 40 semanas \n";
            stream<<"Restante para idade correta: " + idadeCorrigida + "\n";
        }
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

    if(!checked)
    {
        recordChecked = true;

        if(M_auxiliar)
        {
            while( M_principal->isRunning() && M_auxiliar->isRunning() )
                ;
        }
        else
        {
            while( M_principal->isRunning())
                ;
        }

        delay(3000);

        cv::destroyAllWindows();
        MainWindow::ui->record_stop->setText("Gravar");

        delete M_principal;

        if(M_auxiliar)
            delete M_auxiliar;

        //Check if file was created
        if(!P_folder.exists())
        {
            MainWindow::ui->command->setText("Test "+QString::number(currentTest+1)+" suceffuly recorded!");
            testCompleted(currentTest,"rgb(144, 238, 144);");
        }
        else
            MainWindow::ui->command->setText("Cannot create test "+QString::number(currentTest+1)+" file.");
    }

    else if(checked)
    {
        recordChecked = false;

        //Verify User Variables
        if(principalCam == -1)
        {
            ui->command->setText("Please select a camera first!");
            return;
        }
        else if(currentTest == -1)
        {
            ui->command->setText("Please select a test first!");
            return;
        }
        else if(babyName =="" || momName =="")
        {
            ui->command->setText("Please insert baby and mom name");
            return;
        }

        createTxt();
        QString filename_cam1;
        QString filename_cam2;


        filename_cam1 ="test_"+QString::number(currentTest+1)+"_Principal_cam.avi";
        filename_cam2 ="test_"+QString::number(currentTest+1)+"_Auxiliar_cam.avi";


        P_folder = P_folder.filePath(QCoreApplication::applicationDirPath()); //Executable folder
        P_folder.mkdir("Files");
        P_folder.cd("Files");

        //Create folder tests
        P_folder.mkpath(babyName+"_"+momName);
        P_folder.cd(babyName+"_"+momName);

        //Auxiliar Cam file
        A_folder = P_folder.filePath(filename_cam2);
        //Principal cam file
        P_folder = P_folder.filePath(filename_cam1);

        ui->record_stop->setText("Gravando..");

        M_auxiliar = nullptr;

        M_principal = new MultiThread(P_folder,"Principal",principalCam, &recordChecked);
                if(auxiliarCam!=-1)
                    M_auxiliar = new MultiThread(A_folder,"Auxiliar",auxiliarCam, &recordChecked);


        M_principal->start();

                if(M_auxiliar) //if is opened
                    M_auxiliar->start();

        delay(100);
    }
}


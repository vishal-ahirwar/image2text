#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QClipboard>
#include <fmt/base.h>
#include<tesseract/baseapi.h>
#include<QMessageBox>
#include<QFileDevice>
#include <QFileDialog>
#include<QTimer>
#include <qwindow.h>
#include"screencapture.h"
#include<fstream>

#define TESSDATA_PREFIX "tessdata"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumHeight(670);
    setMinimumWidth(400);
    setMaximumHeight(minimumHeight());
    setMaximumWidth(minimumWidth());
    connect(ui->btnEnableEdit,&QPushButton::clicked,this,&MainWindow::onBtnEnableEditClicked);
    connect(ui->btnLoadImage,&QPushButton::clicked,this,&MainWindow::onBtnLoadImageClicked);
    connect(ui->btnSaveAs,&QPushButton::clicked,this,&MainWindow::onBtnSaveAsClicked);
    connect(ui->btnCaptureScreen,&QPushButton::clicked,this,&MainWindow::onBtnCaptureScreenClicked);
    imageScene=new QGraphicsScene(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBtnEnableEditClicked()
{
    ui->plainTextEdit->setReadOnly(!ui->plainTextEdit->isReadOnly());
    ui->btnEnableEdit->setText((ui->plainTextEdit->isReadOnly()?"Enable":"Disable")+QString(" Edit"));
}

void MainWindow::onBtnSaveAsClicked()
{
    if(ui->plainTextEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this,"Save Error","No Text found aborting save operation",QMessageBox::Ok);
        return;
    }
    auto filePath=QFileDialog::getSaveFileName(this,"Save OCR Result");
    if(!filePath.isEmpty())
    {
        std::ofstream out(filePath.toStdString());
        out<<std::noskipws;
        out.write(ui->plainTextEdit->toPlainText().toStdString().c_str(),ui->plainTextEdit->toPlainText().length());
        out.close();
    }
}

void MainWindow::onBtnLoadImageClicked()
{
    currentImagePath=QFileDialog::getOpenFileName(this,"Open Image File","","Image Files(*.png *.jpeg)");
    showImage(QPixmap(currentImagePath));
}

void MainWindow::onBtnCaptureScreenClicked()
{
    this->setWindowState(this->windowState() | Qt::WindowMinimized);
    QTimer::singleShot(500, this,&MainWindow::startCapture);
}

void MainWindow::startCapture()
{
    ScreenCapturer *cap = new ScreenCapturer(this);
    cap->show();
    cap->activateWindow();
}


void MainWindow::extractText()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No opened image.");
        return;
    }

    char *old_ctype = strdup(setlocale(LC_ALL, NULL));
    setlocale(LC_ALL, "C");
    tesseractAPI = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, with specifying tessdata path
    if (tesseractAPI->Init(TESSDATA_PREFIX, "eng")) {
        if(old_ctype)
            free(old_ctype);
        QMessageBox::information(this, "Error", "Could not initialize tesseract.");
        return;
    }

    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);

    tesseractAPI->SetImage(image.bits(), image.width(), image.height(),
                           3, image.bytesPerLine());
    char *outText = tesseractAPI->GetUTF8Text();
    ui->plainTextEdit->setPlainText(outText);
    // Destroy used object and release memory
    tesseractAPI->End();
    delete tesseractAPI;
    tesseractAPI = nullptr;
    delete [] outText;
    setlocale(LC_ALL, old_ctype);
    free(old_ctype);
}


void MainWindow::showImage(QPixmap image)
{

    // 1. Create a QLabel to hold the image.
    QLabel *imageLabel = new QLabel;
    imageLabel->setAttribute(Qt::WA_AlwaysStackOnTop);
    imageLabel->setPixmap(image);
    imageLabel->setScaledContents(true); // Optional: scales the image to fit the label

    // 2. Set window properties. A widget with no parent is a top-level window.
    imageLabel->setWindowTitle("Captured Image");
    imageLabel->adjustSize(); // Adjust window size to the image

    // 3. IMPORTANT: Ensure the window is deleted when closed to prevent memory leaks.
    imageLabel->setAttribute(Qt::WA_DeleteOnClose);

    // 4. Show the new window.
    imageLabel->show();


    imageScene->clear();
    currentImage = imageScene->addPixmap(image);
    imageScene->update();


    extractText();
}



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include<QResizeEvent>
#include<tesseract/baseapi.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void extractText();
private slots:
    void onBtnEnableEditClicked();
    void onBtnSaveAsClicked();
    void onBtnLoadImageClicked();
    void onBtnCaptureScreenClicked();
    void startCapture();

public:
    void showImage(QPixmap image);
private:
    Ui::MainWindow *ui{};
    tesseract::TessBaseAPI *tesseractAPI{};
    QString currentImagePath{};
    QGraphicsPixmapItem *currentImage{};
    QGraphicsScene *imageScene;

};
#endif // MAINWINDOW_H

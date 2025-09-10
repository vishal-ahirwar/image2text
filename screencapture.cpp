#include <QApplication>
#include <QGuiApplication>
#include <QMessageBox>
#include <QRect>
#include <QScreen>
#include <QPainter>
#include <QColor>
#include <QRegion>
#include <QShortcut>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include "screencapture.h"

ScreenCapturer::ScreenCapturer(MainWindow *w):
    QWidget(nullptr), window(w)
{
    setWindowFlags(
        Qt::BypassWindowManagerHint
        | Qt::WindowStaysOnTopHint
        | Qt::FramelessWindowHint
        | Qt::Tool
        );
    showFullScreen();
    setAttribute(Qt::WA_DeleteOnClose);
    // setMouseTracking(true);

    screen = captureDesktop();
    resize(screen.size());
    initShortcuts();
}

ScreenCapturer::~ScreenCapturer() {
}

QPixmap ScreenCapturer::captureDesktop() {
    QRect geometry;
    for (QScreen *const screen : QGuiApplication::screens()) {
        geometry = geometry.united(screen->geometry());
    }
    auto dpi=devicePixelRatio();
    QPixmap pixmap(QApplication::primaryScreen()->grabWindow(0,
        geometry.x()*dpi,
        geometry.y()*dpi,
        geometry.width(),
        geometry.height()
        ));
    pixmap.setDevicePixelRatio(devicePixelRatio());
    return pixmap;
}

void ScreenCapturer::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, screen);

    QRect selectionRect = QRect(p1, p2).normalized();

    QRegion grey(rect());
    if(!selectionRect.isEmpty()) {
        painter.setPen(QColor(200, 100, 50, 255));
        painter.drawRect(selectionRect);
        grey = grey.subtracted(selectionRect);
    }
    painter.setClipRegion(grey);
    QColor overlayColor(20, 20, 20, 50);
    painter.fillRect(rect(), overlayColor);
    painter.setClipRect(rect());
}

void ScreenCapturer::mousePressEvent(QMouseEvent *event)
{
    mouseDown = true;
    p1 = event->pos();
    p2 = event->pos();
    update();
}

void ScreenCapturer::mouseMoveEvent(QMouseEvent *event)
{
    if(!mouseDown) return;
    p2 = event->pos();
    update();
}

void ScreenCapturer::mouseReleaseEvent(QMouseEvent *event)
{
    mouseDown = false;
    p2 = event->pos();
    update();
}



void ScreenCapturer::initShortcuts() {
    auto cl=new QShortcut(Qt::Key_Escape, this);
    connect(cl,&QShortcut::activated,this, &ScreenCapturer::closeMe);
    auto cc=new QShortcut(Qt::Key_Return, this);
    connect(cc,&QShortcut::activated,this, &ScreenCapturer::confirmCapture);
}
void ScreenCapturer::confirmCapture()
{
    QRect selectionRect = QRect(p1, p2).normalized();
    QPixmap image = screen.copy(selectionRect);

    window->showImage(image);
    closeMe();
}
void ScreenCapturer::closeMe()
{
    this->close();
    window->showNormal();
    window->activateWindow();
}

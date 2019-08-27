#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer* click;
    QScreen *screen;
    cv::Size   screenSize;//窗口尺寸
    std::shared_ptr<cv::VideoWriter> m_pVideoWriter;//视频保存操作对象

    //槽函数声明
private slots:
    void StartStopCapture();
    void CaptureWindowFrame();

private:
    //=============辅助函数==============//
    //将Mat转化位QImage
    QImage  cvMatToQImage( const cv::Mat &inMat );
    //将Mat转化为QPixmap
    QPixmap cvMatToQPixmap( const cv::Mat &inMat );
    //将QImage转化为Mat
    cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
    //将QPixmap转化为Mat
    cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );
};

#endif // MAINWINDOW_H

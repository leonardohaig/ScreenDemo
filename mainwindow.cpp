#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QMessageBox>
#include <QScreen>
#include <QTimer>
#include <QDebug>
#include <iostream>
#include <thread>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    ,m_pVideoWriter(nullptr)
{
    ui->setupUi(this);
    click = new QTimer(this);
    click->setInterval(1000/20);//由于视频保存帧率为15fps，故此处设置20

    screen = QGuiApplication::primaryScreen();

    //设置槽函数
    connect(ui->captureBtn,SIGNAL(clicked()),this,SLOT(StartStopCapture()));//开启/关闭定时器以进行视频捕捉
    connect(click,SIGNAL(timeout()),this,SLOT(CaptureWindowFrame()));//定时器函数

}

MainWindow::~MainWindow()
{
    click->stop();
    delete ui;
}

void MainWindow::StartStopCapture()
{
    if(click->isActive() )
    {
        click->stop();
        ui->captureBtn->setText("Start Capture");
        m_pVideoWriter->release();
    }
    else
    {
        //this->showMinimized();//截取整个屏幕时将该程序窗口最小化
        this->showMaximized();

        //视频保存设置
        //screenSize.width = this->width();
        //screenSize.height = this->height();
        screenSize.width = 1280;
        screenSize.height = 720;
        QString fileName= "Video-" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")  + ".avi";//通过时间命名文件

        int fourcc;
#if (CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION==3 )
        fourcc = CV_FOURCC('M', 'J', 'P', 'G');
#elif CV_MAJOR_VERSION == 4
        fourcc = cv::CAP_OPENCV_MJPEG;
#endif
        m_pVideoWriter = std::make_shared<cv::VideoWriter>();
        if( !m_pVideoWriter->open(fileName.toStdString(), fourcc, 15.0, screenSize) )
        {
            QMessageBox::critical(this, "Error", "Set screen capture failed !", QMessageBox::Ok);
            return;
        }

        click->start();

        ui->captureBtn->setText("Stop Capture");
    }
}
void MainWindow::CaptureWindowFrame()
{
    QPixmap pixmap= screen->grabWindow(this->winId());//截取当前项目窗口
    //QPixmap pixmap= screen->grabWindow(0);//截取全屏

    if( m_pVideoWriter )
    {
        cv::Mat video = QPixmapToCvMat(pixmap);
        cv::resize(video,video,screenSize);
        *m_pVideoWriter << video;
    }


    //pixmap.save("test.png","png");  //保存在当前目录下，并命名图片为 test.png
}

//================辅助函数=======================//
//来源：https://blog.csdn.net/qq_27278957/article/details/90638032

//将Mat转化位QImage
QImage  MainWindow::cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_ARGB32 );

        return image;
    }

    // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

    // 8-bit, 1 channel
    case CV_8UC1:
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_Grayscale8 );//Format_Alpha8 and Format_Grayscale8 were added in Qt 5.5
#else//这里还有一种写法，最后给出
        static QVector<QRgb>  sColorTable;

        // only create our color table the first time
        if ( sColorTable.isEmpty() )
        {
            sColorTable.resize( 256 );

            for ( int i = 0; i < 256; ++i )
            {
                sColorTable[i] = qRgb( i, i, i );
            }
        }

        QImage image( inMat.data,
                      inMat.cols, inMat.rows,
                      static_cast<int>(inMat.step),
                      QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );
#endif

        return image;
    }

    default:
        qWarning() << "CVS::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

//将Mat转化为QPixmap
QPixmap MainWindow::cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

//将QImage转化为Mat
cv::Mat MainWindow::QImageToCvMat( const QImage &inImage, bool inCloneImageData)
{
    switch ( inImage.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC4,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
    }

    // 8-bit, 3 channel
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
    {
        if ( !inCloneImageData )
        {
            qWarning() << "CVS::QImageToCvMat() - Conversion requires cloning because we use a temporary QImage";
        }

        QImage   swapped = inImage;

        if ( inImage.format() == QImage::Format_RGB32 )
        {
            swapped = swapped.convertToFormat( QImage::Format_RGB888 );
        }

        swapped = swapped.rgbSwapped();

        return cv::Mat( swapped.height(), swapped.width(),
                        CV_8UC3,
                        const_cast<uchar*>(swapped.bits()),
                        static_cast<size_t>(swapped.bytesPerLine())
                        ).clone();
    }

    // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC1,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qWarning() << "CVS::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
    }

    return cv::Mat();
}

//将QPixmap转化为Mat
cv::Mat MainWindow::QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData)
{
    return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
}

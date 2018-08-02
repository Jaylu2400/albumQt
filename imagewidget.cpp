// imagewidget.cpp
#include "imagewidget.h"
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QPushButton>
#include <stdbool.h>
#include <qDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPainter>
#include <QPalette>
#include <QGesture>
#include <QGestureEvent>
#include <QtMath>
//refer:
//https://blog.csdn.net/taiyang1987912/article/details/45567699#

#define SCALE_LIMIT 4

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
    , mousePress(false)
    , mouseMove(false)
    , curIndex(0)
    , curPosX(-240)
    , isSingleItemUI(false)
    , isFirstDouble(true)
    , isZoomMode(false)
    , cenPixW(0)
    , cenPixH(0)
{
    this->init();

    m_showWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);

    m_showWidget->setAutoFillBackground(true);
    m_showWidget->setGeometry(0, 0, 240 * 2, 320);  //缓存两张图片
    m_showWidget->hide();

    setAttribute(Qt::WA_AcceptTouchEvents);
}

void ImageWidget::initObject(){
    qDebug() << "initObject()";

    m_showWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);
}

ImageWidget::~ImageWidget(){

}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "mousePressEvent";
    mousePress = true;
    m_mouseSrcPos = event->pos();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //qDebug() << "mouse Relase Event";
    if (mouseMove) {

            m_mouseDstPos = event->pos();
            int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();

            if((curIndex == 0) && (xPos > 0)){
                return;
            }else
            if((curIndex == m_listWidget->count() - 1) && (xPos < 0)){
                return;
            }

            if (xPos > 100) {                     //move right
                 curIndex --;
                 curPosX = -240;
            }else
            if(xPos < -100){
                 curIndex ++;
                 curPosX = 240;
            }

            m_showWidget->move(curPosX, 0);
            updateLoadImg(curIndex);
     }

}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "mouse Move Event";
    mouseMove = true;
    m_mouseDstPos = event->pos();

    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    qDebug() << "xPos = " + xPos;

    if(!isZoomMode){
        if((curIndex == 0) && (xPos > 0)){
            xPos = 0;
            curPosX = 0;
        }else
        if((curIndex == m_listWidget->count() - 1) && (xPos < 0)){
            xPos = 0;
            curPosX = 0;
        }

        if(curIndex == 0)
            m_showWidget->move(xPos, 0);
        else
            m_showWidget->move(xPos - 240, 0);
    }else{
        int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();
        qDebug() << "yPos = " + yPos;

        //边界处理
            //计算当前窗口的中心点在图片中的坐标

            //根据坐标可以计算出上下左右4个边界的距离

            //对比移动距离和边界距离，移动图片到达边界时,自动切换到下一张图片居中显示.

        m_showWidget->move(xPos, yPos);
    }
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSize screenSize(240, 320);
    QSize cenPicSize(cenPixW, cenPixH);
    //qDebug() << "mouseDoubleClickEvent";
    isZoomMode = true;
    if(isSingleItemUI){                 //判断当前是否查看图像界面 查看图像界面/缩略图界面
        if(isFirstDouble){              //第一次双击，放大到填充屏幕
            qDebug() << "first double click!!";
            isFirstDouble = false;
            double scale = getScaleValue(cenPicSize, screenSize);

            double w = cenPixW * scale;
            double h = cenPixH * scale;

            m_showWidget->close();
            m_showWidget->setPixmap(cenPixmap.scaled(w, h, Qt::KeepAspectRatio));   //按比例缩放到(w, h)大小
            m_showWidget->resize(w, h);
            m_showWidget->show();
        }else{   //第二次双击，返回slot_itemClicked
            qDebug() << "second double click!!!";
            isFirstDouble = true;

            m_showWidget->resize(showPixmap.size());
            m_showWidget->setPixmap(showPixmap);
            m_showWidget->show();
        }
    }
    else
        QWidget::mouseDoubleClickEvent(event);
}

void ImageWidget::gestureEvent(QGestureEvent *event){
    qDebug() << "gestureEvent";
    printf("gestureEvent\n");
}

void ImageWidget::setLabelMove(bool enable)
{
    label_move = enable;
}

void ImageWidget::zoomOut()
{

}

void ImageWidget::zoomIn()
{

}

void ImageWidget::init() {

    qDebug() << "init()";
    const QSize IMAGE_SIZE(78, 78);
    const QSize ITEM_SIZE(78, 78);

    this->setGeometry(0, 0, 240, 320);

    // 判断路径是否存在
#ifdef OSX
    m_strPath = "/Users/jack/Pictures";
#endif
#ifdef Win32
    m_strPath = "J://img";
#endif
    QDir dir(m_strPath);
    if (!dir.exists()) {
        return;
    }
    // 设置过滤器
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters << "*.png" << "*.jpg" <<"*.bmp";
    dir.setNameFilters(filters);
    m_imgList = dir.entryList();
    if (m_imgList.count() <= 0) {
        return;
    }

    // QListWidget基本设置
    m_listWidget = new QListWidget(this);
    m_listWidget->setGeometry(0, 0, 240, 320);
    m_listWidget->setIconSize(IMAGE_SIZE);
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setViewMode(QListView::IconMode);
    m_listWidget->setMovement(QListView::Static);
    m_listWidget->setSpacing(0);
    QScrollBar *scrollBar = m_listWidget->verticalScrollBar();
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 创建单元项
    for (int i = 0; i<m_imgList.count(); ++i) {
        QPixmap pixmap(m_strPath + "/" + m_imgList.at(i));
        QListWidgetItem *listWidgetItem = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), NULL);  //delete name display
        listWidgetItem->setSizeHint(ITEM_SIZE);
        m_listWidget->insertItem(i, listWidgetItem);
    }

    // 信号与槽
    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));
}

void ImageWidget::updateLoadImg(int index){
    int l = 0, r = 0;     //控制图片加载边界
    int xIndex;           //控制绘图的横坐标索引
    l = index;
    r = index;

    if((l < 0) || (r > (m_listWidget->count() - 1)))
        return;

    if(l > 0){                           //加载第一张以后的图片时，自动加载上一张
        xIndex = index - 1;
        l -= 1;
    }
    else
        xIndex = index;

    if(r < (m_listWidget->count() - 1)) //加载最后一张之前的图片时，自动加载上一张
        r += 1;

    QSize picSize(240,320); //设定屏幕比例参数

#ifdef OSX
    QPixmap pixmap(QSize(this->width() * (r - l + 1), this->height()));
    QPainter painter(&pixmap);
    QImage image;

    qDebug() << "l = " << l << " r = " << r << " xIndex = " << xIndex;

    //如果点击第一张图片，自动加载下一张，禁止右滑|如果点击最后一张，自动加载上一张，禁止左滑
    for(int i = l; i <= r; i++){
        qDebug() << m_strPath + "/" + m_imgList.at(i);
        QImage image(m_strPath + "/" + m_imgList.at(i));
        QPixmap pixmap = QPixmap::fromImage(image).scaled(picSize, Qt::KeepAspectRatio);

        int h = pixmap.height();
        int w = pixmap.width();

        painter.drawPixmap((i - xIndex) * 240 + (240 - w) / 2, (320 - h) / 2, pixmap);

        if(index == i){
            cenPixW = w;
            cenPixH = h;
            cenPixmap = cenPixmap.fromImage(image); //KeepAspectRatio  KeepAspectRatioByExpanding , Qt::SmoothTransformation
        }
    }
    showPixmap = pixmap;

    m_showWidget->resize(showPixmap.size());
    m_showWidget->setPixmap(showPixmap);
    //加载2或3张图片后，总是从第0张开始显示，所以除了点击第一张图片，其它都要左移一个窗口宽度
    m_showWidget->move((l - r + 1) * 240, 0);
#endif
#ifdef Win32
    QPixmap pixmap(QSize(this->width() * (r - l + 1), this->height()));
    QPainter painter(&pixmap);

    qDebug() << "l = " << l << " r = " << r << " xIndex = " << xIndex;

    //如果点击第一张图片，自动加载下一张，禁止右滑|如果点击最后一张，自动加载上一张，禁止左滑
    for(int i = l; i <= r; i++){
        int h = QPixmap(m_strPath + "//" + m_imgList.at(i)).scaled(picSize, Qt::KeepAspectRatio).height();
        int w = QPixmap(m_strPath + "//" + m_imgList.at(i)).scaled(picSize, Qt::KeepAspectRatio).width();

        qDebug() << "w = " << w << " h = " << h;
        painter.drawPixmap((i - xIndex) * 240, (320 - h) / 2, QPixmap(m_strPath + "//" + m_imgList.at(i)).scaled(picSize, Qt::KeepAspectRatio));
    }
    m_showWidget->resize(pixmap.size());
    //m_showWidget->setAlignment(Qt::AlignVCenter);
    m_showWidget->setPixmap(pixmap);
    m_showWidget->move((l - r + 1) * 240, 0);        //居中显示
#endif
}

double ImageWidget::getScaleValue(QSize img, QSize view)
{
    double w = view.width() * 1.0;
    double h = view.height()* 1.0;
    double wi = img.width();
    double hi = img.height();

    double x = w / wi;
    double y = h / hi;

    if((wi <= w) && (hi <= h)){
        if (x >= y)
            return x;
        else
            return y;
    }else
        if((wi > w) && (hi <= h)){
            //get: y > x
            return y;
        }else
            if((wi <= w) && (hi >=h)){
                //get: x > y
                return x;
            }else
                if((wi >= w) && (hi >= h)){
                    if(x > y)
                        return 1/x;
                    else
                        return 1/y;
                }

//    double scale;
//    double scale1 = view.width() * 0.1  / img.width();
//    double scale2 = view.height() * 0.1 / img.height();
//    double scale3 = view.width() * 0.1  / img.height();
//    double scale4 = view.height() * 0.1 / img.width();
//    double zoomRate;

//    if((img.width() / img.height()) > 2 || (img.width() / img.height() < 0.5)){
//        scale = qMin(scale1, scale2);                                       //1

//        zoomRate = qMin(qMax(scale1, scale2), qMax(scale3, scale4)); //2
//        if(zoomRate > SCALE_LIMIT)                                          //3
//            zoomRate = SCALE_LIMIT;

//        while(scale >= zoomRate){                                           //4
//            zoomRate = qMax(qMax(scale1, scale2), qMax(scale3, scale4));
//        }
//        if(zoomRate > SCALE_LIMIT)                                          //5
//            zoomRate = SCALE_LIMIT;
//        return zoomRate;                                                    //6
//    }else{
//        scale = qMin(scale1, scale2);
//        double largerInitRate = qMax(qMin(scale1, scale2), qMin(scale3, scale4));
//        if(largerInitRate > SCALE_LIMIT)
//            largerInitRate = SCALE_LIMIT;

//        zoomRate = qMin(scale2, largerInitRate * 2.0f);
//        if(zoomRate > SCALE_LIMIT)
//            zoomRate = SCALE_LIMIT;

//        while(scale >= zoomRate){
//            zoomRate = largerInitRate * 2.0f;
//        }

//        if(zoomRate > SCALE_LIMIT)
//            zoomRate = SCALE_LIMIT;

//        return zoomRate;
//    }
}

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    qDebug() << "slot_itemClicked, item index= " << m_listWidget->row(item) << "count = " << m_listWidget->count();
    isSingleItemUI = true;

    curIndex = m_listWidget->row(item);
    updateLoadImg(curIndex);

    //menu button
    menuButton->setText("菜单");
    menuButton->setGeometry(210, 300, 30, 20);
    connect(menuButton, SIGNAL(clicked()), this, SLOT(menuView()));

    //back button
    backButton->setText("返回");
    backButton->setGeometry(0, 300, 30, 20);

    //slot and singnal
    connect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));

    m_showWidget->show();
}

void ImageWidget::menuView(void){
    qDebug() << "menuView";
}

void ImageWidget::back2Album(void){
    qDebug() << "back2Album";
    isSingleItemUI = false;
    isZoomMode = false;
//    delete m_showWidget;
//    delete menuButton;
//    delete backButton;

//    m_showWidget    =   NULL;
//    menuButton      =   NULL;
//    backButton      =   NULL;

//    if(NULL != m_showWidget)
//        qDebug() << "delete m_showWidget fail!!!";
//    if(NULL != menuButton)
//        qDebug() << "delete menuButton fail!";
//    if(NULL != backButton)
//        qDebug() << "delete backButton fail!";
    disconnect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));
    m_showWidget->hide();
    this->show();
}

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
    , zoomScale(0.0)
    , xPosLast(0)
    , yPosLast(0)
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
    m_showWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);
}

ImageWidget::~ImageWidget(){

}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    mousePress = true;
    m_mouseSrcPos = event->pos();

    xPosLast = m_showWidget->x();
    yPosLast = m_showWidget->y();

    printf("Press isZoomMode, isFirstDouble, xPos = %d, yPos = %d, xPosLast = m_showWidget.x = %d, yPosLast = m_showWidget.y = %d\n",\
                                       m_mouseSrcPos.x(),    m_mouseSrcPos.y(),   m_showWidget->x(),  m_showWidget->y());
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDstPos = event->pos();
    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

    if(!isZoomMode){
        if (mouseMove) {
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
    }else{
        if(isZoomMode){
            if(isFirstDouble){
                m_showWidget->resize(showPixmap.size());
                m_showWidget->setPixmap(showPixmap);

                if(curIndex == 0)
                    m_showWidget->move(0, 0);
                else
                    m_showWidget->move(-240, 0);

                m_showWidget->show();

                printf("Release isZoomMode, isFirstDouble, xPos = %d, yPos = %d, m_showWidget.x = %d, .y = %d\n",\
                                                   xPos,    yPos,   m_showWidget->x(),  m_showWidget->y());
            }
        }
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseMove = true;
    m_mouseDstPos = event->pos();

    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();

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

        double w = cenPixW * zoomScale;
        double h = cenPixH * zoomScale;

        if(xPos < 0){    //to left
            if(xPos < (240 - w))    //单次向左滑动到达右边界时，禁止继续滑动
                xPos = 240 - w;

            if(m_showWidget->x() < (240 - w)){    //向左滑动到达图像右边界时，停止滑动
                    xPosLast = 0;
                    return;
            }
        }else
            if(xPos > 0){//to right
                if(m_showWidget->x() >= 0){ //向右滑动，图片到达左边界，禁止继续滑动
                    xPosLast = 0;
                    xPos = 0;
                }
            }

        if(yPos < 0){   //to up
            if(yPos < (320 - h))    //向上滑动到达右边界时，禁止继续滑动
                yPos = 320 - h;

            if(m_showWidget->y() < (320 - h)){    //向左滑动到达图像右边界时，停止滑动
                    yPosLast = 0;
                    return;
            }
        }else
            if(yPos > 0){//to down
                if(m_showWidget->y() >= 0){    //向下滑动，图片到达上边界，禁止继续滑动
                    yPosLast = 0;
                    yPos = 0;
                }
            }

        m_showWidget->move(xPos + xPosLast, yPos + yPosLast);
        m_showWidget->show();

        printf("Move isZoomMode, isFirstDouble, xPos = %d, yPos = %d, m_showWidget.x = %d, .y = %d, xPosLast = %d, yPosLast = %d\n",\
                                           xPos,    yPos,   m_showWidget->x(),  m_showWidget->y(), xPosLast, yPosLast);
    }
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSize screenSize(240, 320);
    QSize cenPicSize(cenPixW, cenPixH);

    if(isSingleItemUI){                 //判断当前是否查看图像界面 查看图像界面/缩略图界面
        if(isFirstDouble){              //第一次双击，放大到填充屏幕
            ////qDebug() << "first double click!!";
            isZoomMode = true;
            isFirstDouble = false;

            xPosLast = 0;
            yPosLast = 0;

            zoomScale = getScaleValue(cenPicSize, screenSize);

            double w = cenPixW * zoomScale;
            double h = cenPixH * zoomScale;

            m_showWidget->close();
            m_showWidget->setPixmap(cenPixmap.scaled(w, h, Qt::KeepAspectRatio));   //按比例缩放到(w, h)大小
            m_showWidget->resize(w, h);

            QPoint clickP = event->pos();
            //m_showWidget->move(-(clickP.x() * (scale - 1) / 2), - (clickP.y() * (scale - 1) / 2));    //需要以双击位置为中心点放大
            m_showWidget->move(0, 0);
            m_showWidget->show();

            return;
        }else{   //第二次双击，返回slot_itemClicked
            ////qDebug() << "second double click!!!";
            isFirstDouble = true;
            isZoomMode = false;
        }
    }
    else
        QWidget::mouseDoubleClickEvent(event);
}

void ImageWidget::gestureEvent(QGestureEvent *event){
    ////qDebug() << "gestureEvent";
}


//gesturestart      // 当有两根或多根手指放到屏幕上的时候触发
//gesturechange     // 当有两根或多根手指在屏幕上，并且有手指移动的时候触发
//gestureend        // 当倒数第二根手指提起的时候触发，结束gesture

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

    ////qDebug() << "init()";
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

    pixmap.fill(Qt::black);
    //qDebug() << "l = " << l << " r = " << r << " xIndex = " << xIndex;

    //如果点击第一张图片，自动加载下一张，禁止右滑|如果点击最后一张，自动加载上一张，禁止左滑
    for(int i = l; i <= r; i++){
        //qDebug() << m_strPath + "/" + m_imgList.at(i);
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
    if(index == r)
        m_showWidget->move((l - r) * 240, 0);
    else
        m_showWidget->move((l - r + 1) * 240, 0);
#endif
#ifdef Win32
    QPixmap pixmap(QSize(this->width() * (r - l + 1), this->height()));
    QPainter painter(&pixmap);

    //qDebug() << "l = " << l << " r = " << r << " xIndex = " << xIndex;

    //如果点击第一张图片，自动加载下一张，禁止右滑|如果点击最后一张，自动加载上一张，禁止左滑
    for(int i = l; i <= r; i++){
        int h = QPixmap(m_strPath + "//" + m_imgList.at(i)).scaled(picSize, Qt::KeepAspectRatio).height();
        int w = QPixmap(m_strPath + "//" + m_imgList.at(i)).scaled(picSize, Qt::KeepAspectRatio).width();

        //qDebug() << "w = " << w << " h = " << h;
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
}

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    //qDebug() << "slot_itemClicked, item index= " << m_listWidget->row(item) << "count = " << m_listWidget->count();
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
    //qDebug() << "menuView";
}

void ImageWidget::back2Album(void){
    //qDebug() << "back2Album";
    isSingleItemUI = false;
    isZoomMode = false;

    disconnect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));
    m_showWidget->hide();
    this->show();
}

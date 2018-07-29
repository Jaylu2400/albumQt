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

//refer:
//https://blog.csdn.net/taiyang1987912/article/details/45567699#

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
    , mousePress(false)
    , mouseMove(false)
    , curIndex(0)
    , curPosX(-240)
{
    this->init();

    m_showWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);

    m_showWidget->setAutoFillBackground(true);
    m_showWidget->setGeometry(0, 0, 240 * 2, 320);  //缓存两张图片
    m_showWidget->hide();
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
    qDebug() << "mousePressEvent";
    mousePress = true;
    m_mouseSrcPos = event->pos();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "mouse Relase Event";

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
    qDebug() << "mouse Move Event";
    mouseMove = true;
    m_mouseDstPos = event->pos();

    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();

    if((curIndex == 0) && (xPos > 0)){
        xPos = 0;
        curPosX = 0;
    }else
    if((curIndex == m_listWidget->count() - 1) && (xPos < 0)){
        xPos = 0;
        curPosX = 0;
    }

    m_showWidget->move(xPos, 0);
}

//根据滑动距离自动居中图片
//fase: left    true:right
void ImageWidget::moveCurrentPage(bool direction)
{
    if(direction){
        //show right pic
    }else{
        //show left pic
    }
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

void ImageWidget::updateLoadImg(int itemIndex){
    int l = 0, r = 0;     //控制图片加载边界
    int xIndex;           //控制绘图的横坐标索引
    //int itemIndex = 0;    //图片编号
    //itemIndex = m_listWidget->row(item);
    curIndex = itemIndex;
    l = itemIndex;
    r = itemIndex;

    if((l < 0) || (r > (m_listWidget->count() - 1)))
        return;

    if(l > 0){                           //加载第一张以后的图片时，自动加载上一张
        xIndex = itemIndex - 1;
        l -= 1;
    }
    else
        xIndex = itemIndex;

    if(r < (m_listWidget->count() - 1)) //加载最后一张之前的图片时，自动加载上一张
        r += 1;

    QSize picSize(240,320); //设定屏幕比例参数

#ifdef OSX
    QPixmap pixmap(QSize(this->width() * (r - l + 1), this->height()));
    QPainter painter(&pixmap);

    qDebug() << "l = " << l << " r = " << r << " xIndex = " << xIndex;

    //如果点击第一张图片，自动加载下一张，禁止右滑|如果点击最后一张，自动加载上一张，禁止左滑
    for(int i = l; i <= r; i++){
        painter.drawPixmap((i - xIndex) * 240, 0, QPixmap(m_strPath + "/" + m_imgList.at(i)).scaled(picSize, Qt::KeepAspectRatio));
    }
    m_showWidget->resize(pixmap.size());
    m_showWidget->setPixmap(pixmap);
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

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    qDebug() << "slot_itemClicked, item index= " << m_listWidget->row(item) << "count = " << m_listWidget->count();

    updateLoadImg(m_listWidget->row(item));

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
    QLabel *menuLabel = new QLabel;
    menuLabel->setGeometry(100, 150, 40, 20);

}

void ImageWidget::back2Album(void){
    qDebug() << "back2Album";
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

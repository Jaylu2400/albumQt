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

//refer:
//https://blog.csdn.net/taiyang1987912/article/details/45567699#

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
    , mouse_press(false)
    , mouse_move(false)
    , label_move(true)
    , current_index(0)
    , current_pos_x(0)
    , firstPress(1)
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
    //if(firstPress){
        //firstPress = false;
        mouse_press = true;
        m_mouseSrcPos = event->pos();
    //}
//    else{
//        qDebug() << "second point";
//        firstPress = true;
//        m_mouseDstPos = event->pos();
//    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    int xpos = 0;
    qDebug() << "mouse Relase Event";
    if (mouse_press) {
        if (label_move) {
            m_mouseDstPos = event->pos();
            xpos = m_mouseDstPos.x() - m_mouseSrcPos.x();
            if (xpos > 0) {                     //the plan is:move right
                if(xpos >= 100) {               //mouse gap
                    if(current_index > 0) {     //move right    图片数量边界控制
                        current_index--;
                        moveCurrentPage(false);
                    }
                    else {
                        moveCurrentPage(true);  //move left
                    }
                }
                else {
                     moveCurrentPage(true);     //move left
                }
            }
            else {             //the plan is:move right
                if(xpos <= -100)
                {
                    if(current_index < (m_listWidget->count() - 1)) {
                        current_index++;
                        moveCurrentPage(true);  //move left
                    }
                    else {
                        moveCurrentPage(false); //move right
                    }
                }
                else {
                    moveCurrentPage(false);     //move right
                }
            }
        }
        mouse_press = false;
    }
    else if(mouse_move){
        mouse_move = false;
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mouse Move Event";
    int xPos = 0;
    if(mouse_press) {
        if(label_move) {
            m_mouseDstPos = event->pos();
            xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();

            setLabelMove(false);
            m_showWidget->move(current_pos_x + xPos - 240, 0);
            setLabelMove(true);
        }
    }
    else if(mouse_move) {
        m_mouseDstPos = event->pos();
        //this->move(this->pos() + m_mouseDstPos - m_mouseSrcPos);
    }
}

//根据滑动距离自动居中图片
//fase: left    true:right
void ImageWidget::moveCurrentPage(bool direction)
{
//    setLabelMove(false);
//    int current_pos_x = total_label->x();    //label position
//    int dest_pos_x = -680 * current_index;

    if(direction){
        //show right pic
    }else{
        //show left pic
    }

//    total_label->move(dest_pos_x, 0);
//    setLabelMove(true);
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

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    qDebug() << "slot_itemClicked, item index= " << m_listWidget->row(item) << "count = " << m_listWidget->count();
    int l = 0, r = 0;     //控制图片加载边界
    int xIndex;           //控制绘图的横坐标索引
    int itemIndex = 0;    //图片编号
    itemIndex = m_listWidget->row(item);
    current_index = itemIndex;
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
    m_showWidget->setPixmap(QPixmap(m_strPath + "/" + m_imgList.at(m_listWidget->row(item))).scaled(picSize, Qt::KeepAspectRatio));
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
    //pixmap.
    //painter.drawPixmap();
    m_showWidget->resize(pixmap.size());
    //m_showWidget->setAlignment(Qt::AlignVCenter);
    m_showWidget->setPixmap(pixmap);
    m_showWidget->move((l - r + 1) * 240, 0);        //居中显示
#endif

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

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

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent){

    this->init();
    m_showWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);
}

void ImageWidget::initObject(){
    qDebug() << "initObject()";

    m_showWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);
}

ImageWidget::~ImageWidget(){}

void ImageWidget::init() {

    qDebug() << "init()";
    const QSize IMAGE_SIZE(73, 73);
    const QSize ITEM_SIZE(73, 73);

    this->setGeometry(0, 0, 240, 320);

    // 判断路径是否存在
    m_strPath = "/Users/jack/Pictures";
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

    // 创建单元项
    for (int i = 0; i<m_imgList.count(); ++i) {
        QPixmap pixmap(m_strPath + "/" + m_imgList.at(i));
        QListWidgetItem *listWidgetItem = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), NULL);  //delete name display
        listWidgetItem->setSizeHint(ITEM_SIZE);
        m_listWidget->insertItem(i, listWidgetItem);
    }

    //initObject();

    // 信号与槽
    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));
}

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    qDebug() << "slot_itemClicked";

    QSize picSize(240,320); //设定屏幕比例参数

    m_showWidget->setAutoFillBackground(true);
    m_showWidget->setGeometry(0, 0, 240, 300);
    m_showWidget->setPixmap(QPixmap(m_strPath + "/" + m_imgList.at(m_listWidget->row(item))).scaled(picSize, Qt::KeepAspectRatio));

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

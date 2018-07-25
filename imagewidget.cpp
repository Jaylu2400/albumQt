// imagewidget.cpp
#include "imagewidget.h"
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QPushButton>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent){
    this->init(parent);
    m_showWidget = new QLabel(parent);
}

ImageWidget::~ImageWidget(){}

void ImageWidget::init(QWidget *parent) {
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
    m_listWidget = new QListWidget(parent);
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

    // 信号与槽
    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));
}

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    QSize picSize(240,320); //设定屏幕比例参数
    m_showWidget->setAutoFillBackground(1);
    m_showWidget->setGeometry(0, 0, 240, 320);
    m_showWidget->setPixmap(QPixmap(m_strPath + "/" + m_imgList.at(m_listWidget->row(item))).scaled(picSize, Qt::KeepAspectRatio));

    //return button
    QPushButton backButton;
    backButton.setGeometry(10, 300, 20, 10);
    backButton.setBackgroundRole(QPalette::Dark);

    this->show();
    //propreties button

    //slot of return button
    //connect();
}

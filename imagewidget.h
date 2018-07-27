// imagewidget.h
#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtWidgets/QWidget>
#include <QLabel>
#include <QPushButton>
#include <mainwindow.h>
#include <QPoint>
class QStringList;
class QListWidget;
class QListWidgetItem;

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QWidget *parent = 0);
    ~ImageWidget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void init();
    void initObject();
    QPoint m_mouseSrcPos;   //滑动的初始点
    QPoint m_mouseDstPos;   //滑动的终点
    QLabel *total_label;    //图片滑动的动态页面/放大后的移动页面（大小变化）
    bool mouse_press;
    bool mouse_move;
    bool label_move;
    bool firstPress;
    int current_index;
    int current_pos_x;

    void moveCurrentPage(bool direction);
    void setLabelMove(bool enable);
    void zoomOut(void);     //缩小图片
    void zoomIn(void);      //放大图片

private slots:
    void slot_itemClicked(QListWidgetItem*);
    void menuView(void);
    void back2Album(void);

private:
    QStringList m_imgList;                  // 文件目录下所有的图像文件名
    QListWidget *m_listWidget;              // 预览窗口
    QString m_strPath;                      // 文件夹路径

    QLabel *m_showWidget;                    // 图像显示窗口
    QPushButton *menuButton;
    QPushButton *backButton;
};

#endif // IMAGEWIDGET_H

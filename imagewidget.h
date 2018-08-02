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
class QImage;
class QGestureEvent;
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
    void mouseDoubleClickEvent(QMouseEvent *event);
    void gestureEvent(QGestureEvent *event);
private:
    void init();
    void initObject();
    QPoint m_mouseSrcPos;   //滑动的初始点
    QPoint m_mouseDstPos;   //滑动的终点
    QLabel *total_label;    //图片滑动的动态页面/放大后的移动页面（大小变化）
    bool mousePress;
    bool mouseMove;
    bool label_move;
    bool firstPress;
    int curIndex;
    int curPosX;
    bool isSingleItemUI;    //是否图片查看界面 default:false
    bool isFirstDouble;     //是否第一次双击   default:true
    bool isZoomMode;        //是否缩放模式    default:false
    int cenPixW;
    int cenPixH;

    void setLabelMove(bool enable);
    void zoomOut(void);     //缩小图片
    void zoomIn(void);      //放大图片
    void updateLoadImg(int index);
    double getScaleValue(QSize img, QSize view);

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

    QPixmap cenPixmap;
    QPixmap showPixmap;
    QImage  cenImg;
};

#endif // IMAGEWIDGET_H

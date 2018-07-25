#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagewidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setGeometry(0, 0, 240, 320);

    ImageWidget *imageListVew = new ImageWidget(this);
    imageListVew->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

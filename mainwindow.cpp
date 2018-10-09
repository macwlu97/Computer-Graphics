#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->translationX, SIGNAL(valueChanged(int)), ui->canvas, SLOT(translationXChanged(int)));
    connect(ui->translationY, SIGNAL(valueChanged(int)), ui->canvas, SLOT(translationYChanged(int)));
    connect(ui->translationZ, SIGNAL(valueChanged(int)), ui->canvas, SLOT(translationZChanged(int)));

    connect(ui->rotationX, SIGNAL(valueChanged(int)), ui->canvas, SLOT(rotationXChanged(int)));
    connect(ui->rotationY, SIGNAL(valueChanged(int)), ui->canvas, SLOT(rotationYChanged(int)));
    connect(ui->rotationZ, SIGNAL(valueChanged(int)), ui->canvas, SLOT(rotationZChanged(int)));

    connect(ui->scalingX, SIGNAL(valueChanged(int)), ui->canvas, SLOT(scalingXChanged(int)));
    connect(ui->scalingY, SIGNAL(valueChanged(int)), ui->canvas, SLOT(scalingYChanged(int)));
    connect(ui->scalingZ, SIGNAL(valueChanged(int)), ui->canvas, SLOT(scalingZChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

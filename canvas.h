#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QVector3D>
#include <QVector2D>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);


    QImage *canvas;
    QImage *texture;

    std::vector<QVector3D> vertices;
    std::vector<int> edges;

    QVector3D translation;
    QVector3D rotation;
    QVector3D scaling;

    float half_screen;

    QVector3D vertexA, vertexB, vertexC;

    int Width_start, Width_end, Height_start, Height_end;
    double area;
    double u,v,w;
    double Pu, Pv;

private:
void paintEvent(QPaintEvent *event);

void Object3D();
void draw3DObject();

void to2d();

float isVisible();

void Loop_Settings();

void BarycentricCalculation(int i, int y, int x);


signals:

public slots:
void translationXChanged(int);
void translationYChanged(int);
void translationZChanged(int);
void rotationXChanged(int);
void rotationYChanged(int);
void rotationZChanged(int);
void scalingXChanged(int);
void scalingYChanged(int);
void scalingZChanged(int);

};

#endif // CANVAS_H

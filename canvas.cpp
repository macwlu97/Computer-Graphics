#include "canvas.h"
#include <QPainter>
#include <QMatrix4x4>

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    canvas = new QImage(471, 361, QImage::Format_RGB32);

    texture = new QImage("C:/abc.PNG");

    half_screen = (canvas->width() + canvas->height()) / 2;

    Object3D();
    draw3DObject();
}

void Canvas::paintEvent(QPaintEvent *event){

    QPainter p(this);
    p.drawImage(0, 0, *canvas);

}

void Canvas::Object3D()
{
    scaling = QVector3D(1,1,1);

    vertices.push_back(QVector3D(0, 0, 0));
    vertices.push_back(QVector3D(0, 1, 0));
    vertices.push_back(QVector3D(1, 1, 0));
    vertices.push_back(QVector3D(1, 0, 0));
    vertices.push_back(QVector3D(0, 0, 1));
    vertices.push_back(QVector3D(0, 1, 1));
    vertices.push_back(QVector3D(1, 1, 1));
    vertices.push_back(QVector3D(1, 0, 1));



    for (int i = 0; i < vertices.size(); i++){
        vertices[i] -= QVector3D(0.5, 0.5, 0.5);
    }

    edges.push_back(0);
    edges.push_back(1);
    edges.push_back(2);

    edges.push_back(0);
    edges.push_back(2);
    edges.push_back(3);

    edges.push_back(3);
    edges.push_back(2);
    edges.push_back(6);

    edges.push_back(3);
    edges.push_back(6);
    edges.push_back(7);

    edges.push_back(4);
    edges.push_back(5);
    edges.push_back(1);

    edges.push_back(4);
    edges.push_back(1);
    edges.push_back(0);

    edges.push_back(7);
    edges.push_back(6);
    edges.push_back(5);

    edges.push_back(7);
    edges.push_back(5);
    edges.push_back(4);

    edges.push_back(1);
    edges.push_back(5);
    edges.push_back(6);

    edges.push_back(1);
    edges.push_back(6);
    edges.push_back(2);

    edges.push_back(7);
    edges.push_back(4);
    edges.push_back(0);

    edges.push_back(7);
    edges.push_back(0);
    edges.push_back(3);

}

void Canvas::draw3DObject()
{
  canvas->fill(Qt::black);

  QMatrix4x4 TransformationMatrix;

  TransformationMatrix.translate(QVector3D(0, 0, 1000)); //przesunięcie do wdg z do prozdu

  TransformationMatrix.translate(translation); //wektor przesunięcia
  //Mnoży tę macierz przez inną, która tłumaczy współrzędne według komponentów x, y i z.

  TransformationMatrix.rotate(rotation.x(), 1, 0, 0);
  TransformationMatrix.rotate(rotation.y(), 0, 1, 0);
  TransformationMatrix.rotate(rotation.z(), 0, 0, 1);
  //Mnoży tę macierz przez inną, która obraca współrzędne o kąty stopni wokół wektora (x, y, z).

  TransformationMatrix.scale(scaling * 400); //skalowanie


   for (int i = 0; i < edges.size(); i = i + 3){ //przechodzimy kolejne krawędzie tworzące - trójkąty

       //ustalamy wierzchołki A,B,C na podstawie przechodzonych krawędzi

        vertexA = vertices[edges[i]];
        vertexB = vertices[edges[i+1]];
        vertexC = vertices[edges[i+2]];


       // mnożymy wierzchołki(ich wektory) przez macierz przekształcenia

       vertexA = TransformationMatrix * vertexA;
       vertexB = TransformationMatrix * vertexB;
       vertexC = TransformationMatrix * vertexC;


       //Rzut perspektywiczny punktów 3D na ekran 2D.

       to2d();

       //Czy jest widoczne? --- chowanie tylnych ścian

       float normalDot = isVisible();

       //Jeżeli nie jest widoczne to usuwamy i kontynuujemy pętle
               if (normalDot <= 0){
                   continue;
               }

       //obszar teksturowania tych co widać trójkątów
            area = (double)( (vertexB.y() - vertexC.y()) * (vertexA.x() - vertexC.x()) + (vertexC.x() - vertexB.x()) * (vertexA.y() - vertexC.y()) );

            Loop_Settings();

            for (int y = Height_start; y <= Height_end; y++){
                for (int x = Width_start; x <= Width_end; x++){

                    BarycentricCalculation(i,y,x);

                     if (u >= 0 && v >= 0 && w >= 0){

                         bool parzystyTrojkat = true;
                         if ( (i / 3) % 2 != 0 )
                             parzystyTrojkat = false;

                         QVector2D A, B, C;


                         if (parzystyTrojkat){
                             A = QVector2D(0,0);
                             B = QVector2D(0,1);
                             C = QVector2D(1,1);
                         } else {
                             A = QVector2D(0,0);
                             B = QVector2D(1,1);
                             C = QVector2D(1,0);
                         }


                         //P = u · A + v · B + w · C

                                             double Pu = A.x() * u + B.x() * v + C.x() * w;
                                             double Pv = A.y() * u + B.y() * v + C.y() * w;

                                             int Px = Pu * texture->width();
                                             int Py = Pv * texture->height();

                                             int ByteIndexTexture = Px * 4 + Py * texture->width() * 4;
                                             int ByteIndexCanvas = x * 4 + y * canvas->width() * 4;    //alfa

                                             if (ByteIndexCanvas >= canvas->byteCount())
                                                 ByteIndexCanvas = 0;

                                             canvas->bits()[ByteIndexCanvas] = texture->bits()[ByteIndexTexture] * normalDot;          // blue  chyba ten normalDot to przycieniowanie
                                             canvas->bits()[ByteIndexCanvas+1] = texture->bits()[ByteIndexTexture+1] * normalDot;      // green
                                             canvas->bits()[ByteIndexCanvas+2] = texture->bits()[ByteIndexTexture+2] * normalDot;      // red


                     }

                }
            }

   }

 repaint();
}

void Canvas::BarycentricCalculation(int i, int y, int x){
    //Obliczanie współrzędnych barycentrycznych
             u = (double)( (vertexB.y() - vertexC.y()) * ((double)x - vertexC.x()) + (vertexC.x() - vertexB.x()) * ((double)y - vertexC.y()) )
                                / area;
             v = (double)(
                                (vertexC.y() - vertexA.y())
                                * ((double)x - vertexC.x())
                                + (vertexA.x() - vertexC.x())
                                * ((double)y - vertexC.y()) )
                                / area;

                     w = 1.0 - u - v; //bo u+v+w=1
    }


void Canvas::to2d() {
    //Rzut perspektywiczny punktów 3D na ekran 2D.
    vertexA.setX( (vertexA.x() * half_screen) / (vertexA.z() + half_screen) + canvas->width()/2);
    vertexA.setY( (-vertexA.y() * half_screen) / (vertexA.z() + half_screen) + canvas->height()/2);
    vertexB.setX( (vertexB.x() * half_screen) / (vertexB.z() + half_screen) + canvas->width()/2);
    vertexB.setY( (-vertexB.y() * half_screen) / (vertexB.z() + half_screen) + canvas->height()/2);
    vertexC.setX( (vertexC.x() * half_screen) / (vertexC.z() + half_screen) + canvas->width()/2);
    vertexC.setY( (-vertexC.y() * half_screen) / (vertexC.z() + half_screen) + canvas->height()/2);
}
//((walls[i].x * half_screen) / (walls[i].z + half_screen) + szer/2) ;
float Canvas::isVisible(){

    QVector3D nu = vertexB - vertexA;
    QVector3D nv = vertexC - vertexA;

    QVector3D *n = new QVector3D( //iloczyn wektorowy
        nu.z() * nv.z() - nu.z() * nv.z(),
        nu.z() * nv.x() - nu.x() * nv.z(),
        nu.x() * nv.y() - nu.y() * nv.x()
    );

     n->normalize();

     QVector3D kamera(0, 0, 1);

     float dot = QVector3D::dotProduct(kamera, *n);
     if (dot <= 0) delete n;
     return dot;
}

void Canvas::Loop_Settings(){
    //ustawiamy pętle według wierzchołków i canvas ekranu 2D dla danego trójkąta
     Width_start = vertexA.x();
     Width_end = vertexA.x();

    if (Width_start > vertexB.x())
        Width_start = vertexB.x();
    else if (Width_end < vertexB.x())
        Width_end = vertexB.x();

    if (Width_start > vertexC.x())
        Width_start = vertexC.x();
    else if (Width_end < vertexC.x())
        Width_end = vertexC.x();

    ////

     Height_start = vertexA.y();
     Height_end = vertexA.y();

    if (Height_start > vertexB.y())
        Height_start = vertexB.y();
    else if (Height_end < vertexB.y())
        Height_end = vertexB.y();

    if (Height_start > vertexC.y())
        Height_start = vertexC.y();
    else if (Height_end < vertexC.y())
        Height_end = vertexC.y();


    if (Width_start < 0)
        Width_start = 0;
    if (Width_end >= canvas->width())
        Width_end = canvas->width() - 1;
    if (Height_start < 0)
        Height_start = 0;
    if (Height_end >= canvas->height())
        Height_end = canvas->height() - 1;
}

void Canvas::translationXChanged(int value)
{
    translation.setX(value*10);
    draw3DObject();
}

void Canvas::translationYChanged(int value)
{
    translation.setY(value*10);
     draw3DObject();
}

void Canvas::translationZChanged(int value)
{
    translation.setZ(value*10);
     draw3DObject();
}

void Canvas::rotationXChanged(int value)
{
    rotation.setX(value);
     draw3DObject();
}

void Canvas::rotationYChanged(int value)
{
    rotation.setY(value);
     draw3DObject();
}

void Canvas::rotationZChanged(int value)
{
    rotation.setZ(value);
    draw3DObject();
}

void Canvas::scalingXChanged(int value)
{
    scaling.setX((float)value / 10.0);
     draw3DObject();
}

void Canvas::scalingYChanged(int value)
{
    scaling.setY((float)value / 10.0);
    draw3DObject();
}

void Canvas::scalingZChanged(int value)
{
    scaling.setZ((float)value / 10.0);
     draw3DObject();
}

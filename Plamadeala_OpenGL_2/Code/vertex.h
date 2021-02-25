#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

struct Vertex
{
    QVector3D coords;
    QVector3D color;

    Vertex(QVector3D coords = QVector3D(0, 0, 0), QVector3D color = QVector3D(0, 0, 0))
        :
          coords(coords),
          color(color)
    {}
};

#endif // VERTEX_H


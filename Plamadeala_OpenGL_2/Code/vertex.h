#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

struct Vertex {
    QVector3D coords;
    QVector3D color;
    QVector2D textureCoords;

    Vertex(QVector3D coords = QVector3D(0, 0, 0),
           QVector3D color = QVector3D(0, 0, 0),
           QVector2D textureCoords = QVector2D(0, 0))
        :
        coords(coords),
        color(color),
        textureCoords(textureCoords) {}
};

#endif // VERTEX_H


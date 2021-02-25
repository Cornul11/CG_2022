#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "vertex.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QVector3D>
#include <memory>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
Q_OBJECT

    int globalRotateX;
    int globalRotateY;
    int globalRotateZ;
    float globalScale;

    GLuint VBOCube;
    GLuint VAOCube;
    GLuint VBOPyramid;
    GLuint VAOPyramid;
    GLuint VBOSphere;
    GLuint VAOSphere;

    GLint modelLocation;
    GLint projectionLocation;

    QMatrix4x4 projectTransform;
    QMatrix4x4 transformCube;
    QMatrix4x4 transformPyramid;
    QMatrix4x4 transformSphere;

    std::vector<Vertex> vertexArrayCube;
    std::vector<Vertex> vertexArrayPyramid;
    std::vector<Vertex> vertexArraySphere;

public:
    enum ShadingMode : GLuint {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();
    void rotateFunction(int x, int y, int z);
    void scaleFunction(float s);
    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
    QVector<quint8> imageToBytes(QImage image);

private slots:
    static void onMessageLogged(const QOpenGLDebugMessage &Message);

private:
    QOpenGLDebugLogger debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram shaderProgram;

    void createShaderProgram();
    QVector<QVector3D> sphereVertices;
    int sphereSize;
    void sendVertexData(std::vector<Vertex> sphere,
                        std::vector<Vertex> cube,
                        std::vector<Vertex> pyramid);
    void defineObjects();
};

#endif // MAINVIEW_H

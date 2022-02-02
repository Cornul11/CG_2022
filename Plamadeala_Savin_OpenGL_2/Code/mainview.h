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
    GLuint VBOCat{};
    GLuint VAOCat{};

    GLint modelLocation[4]{};
    GLint projectionLocation[4]{};
    GLint normalLocation[4]{};
    GLint lightLocation[3]{};
    GLint materialLocation[3]{};
    GLint textureLocation[3]{};
    GLint exponentLocation[3]{};

    QMatrix4x4 projectTransform;
    QMatrix4x4 catTransform;

    std::vector<Vertex> catVertexArray;

public:
    enum ShadingMode : GLuint {
        PHONG = 0, NORMAL, GOURAUD, ZBRUSH
    };

    explicit MainView(QWidget *parent = nullptr);
    ~MainView() override;

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int newScale);
    void setShadingMode(ShadingMode shading);
    void setLightPosition(int posX, int posY, int posZ);
    void setPhongExponent(int newExponent);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();
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
    QImage image;
    QVector<quint8> imageData;

    QOpenGLDebugLogger debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLShaderProgram shaderProgram[4];
    int activeShaderProgram = ShadingMode::PHONG;

    GLuint catTexture{};
    int catSize{};
    QVector<QVector3D> catVertices;

    float scale = 1.f;
    float aspectRatio = 1.f;
    float exponent = 1;
    QVector3D lightPositionMatrix = {50, 50, 50};
    QVector3D rotation;

    void updateCatTransform();
    void updateProjectTransform();

    void loadTexture(const QString& file, GLuint *texturePtr);

    void createShaderProgram();
    void sendVertexData(std::vector<Vertex> cat);
    void loadMesh();
};

#endif // MAINVIEW_H

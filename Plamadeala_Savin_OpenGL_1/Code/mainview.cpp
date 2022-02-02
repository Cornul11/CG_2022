#include "mainview.h"

#include <QDateTime>

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));

    transformCube.translate(2, 0, -6);
    transformPyramid.translate(-2, 0, -6);
    transformSphere.translate(0, 0, -10);

    projectTransform.perspective(60.0f, 1.0f, 1.0f, 100);

    globalRotateX = 0;
    globalRotateY = 0;
    globalRotateZ = 0;
    globalScale = 1;
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    qDebug() << "MainView destructor";

    glDeleteBuffers(1, &VBOCube);
    glDeleteVertexArrays(1, &VAOCube);

    glDeleteBuffers(1, &VBOPyramid);
    glDeleteVertexArrays(1, &VAOPyramid);

    glDeleteBuffers(1, &VBOSphere);
    glDeleteVertexArrays(1, &VAOSphere);

    makeCurrent();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)),
            this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color to be used by glClear. This is, effectively, the background color.
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    defineObjects();

    sendVertexData(vertexArraySphere, vertexArrayCube, vertexArrayPyramid);
}

void MainView::defineObjects() {
    // load sphere object
    Model sphereModel = Model(QString(":/models/sphere.obj"));
    sphereModel.unitize();

    sphereVertices = sphereModel.getVertices();
    sphereSize = sphereVertices.size();

    vertexArraySphere.reserve(sphereSize);
    for (int i = 0; i < sphereSize; i++) {
        vertexArraySphere.push_back(createVertex(sphereVertices[i].x(),
                                                 sphereVertices[i].y(),
                                                 sphereVertices[i].z(),
                                                 rand() / double(RAND_MAX),
                                                 rand() / double(RAND_MAX),
                                                 rand() / double(RAND_MAX)));
    }

    // define the vertices that will be used for the square and the pyramid
    v1 = createVertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    v2 = createVertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    v3 = createVertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    v4 = createVertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f);
    v5 = createVertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
    v6 = createVertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
    v7 = createVertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
    v8 = createVertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f);
    v9 = createVertex(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);

    // define the triangles of the cube
    vertexArrayCube = {v1, v2, v3,
                       v3, v4, v1,
                       v1, v4, v5,
                       v4, v8, v5,
                       v1, v5, v6,
                       v1, v6, v2,
                       v2, v6, v7,
                       v2, v7, v3,
                       v4, v3, v7,
                       v4, v7, v8,
                       v5, v8, v7,
                       v5, v7, v6};

    // define the triangles of the pyramid
    vertexArrayPyramid = {v2, v6, v7,
                          v2, v7, v3,
                          v9, v2, v3,
                          v9, v3, v7,
                          v9, v7, v6,
                          v9, v6, v2};
}

/**
 * @brief MainView::sendVertexData
 *
 * Called upon OpenGL initialization
 * Sends the layout of the data and the data itself to the GPU
 * @param sphere
 * @param cube
 * @param pyramid
 */
void MainView::sendVertexData(std::vector<Vertex> sphere,
                              std::vector<Vertex> cube,
                              std::vector<Vertex> pyramid) {
    // CUBE
    Vertex localVertexArrayCube[cube.size()];
    std::copy(cube.begin(), cube.end(), localVertexArrayCube);
    // VBO
    glGenBuffers(1, &VBOCube);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(localVertexArrayCube), localVertexArrayCube, GL_STATIC_DRAW);
    // VAO
    glGenVertexArrays(1, &VAOCube);
    glBindVertexArray(VAOCube);
    // Telling the GPU the layout of the data
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3 * sizeof(GLfloat)));

    // PYRAMID
    Vertex localVertexArrayPyramid[pyramid.size()];
    std::copy(pyramid.begin(), pyramid.end(), localVertexArrayPyramid);
    // VBO
    glGenBuffers(1, &VBOPyramid);
    glBindBuffer(GL_ARRAY_BUFFER, VBOPyramid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(localVertexArrayPyramid), localVertexArrayPyramid, GL_STATIC_DRAW);
    // VAO
    glGenVertexArrays(1, &VAOPyramid);
    glBindVertexArray(VAOPyramid);
    // Telling the GPU the layout of the data
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3 * sizeof(GLfloat)));

    // SPHERE
    Vertex localVertexArraySphere[sphere.size()];
    std::copy(sphere.begin(), sphere.end(), localVertexArraySphere);
    // VBO
    glGenBuffers(1, &VBOSphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBOSphere);
    glBufferData(GL_ARRAY_BUFFER, sizeof(localVertexArraySphere), localVertexArraySphere, GL_STATIC_DRAW);
    // VAO
    glGenVertexArrays(1, &VAOSphere);
    glBindVertexArray(VAOSphere);
    // Telling the GPU the layout of the data
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3 * sizeof(GLfloat)));
}

void MainView::createShaderProgram() {
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/shaders/fragshader.glsl");
    shaderProgram.link();

    modelLocation = shaderProgram.uniformLocation("modelTransform");
    projectionLocation = shaderProgram.uniformLocation("projectionTransform");
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectTransform.data());

    // Draw here
    glBindVertexArray(VAOCube);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, transformCube.data());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(VAOPyramid);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, transformPyramid.data());
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glBindVertexArray(VAOSphere);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, transformSphere.data());
    glDrawArrays(GL_TRIANGLES, 0, sphereSize);

    shaderProgram.release();
}

void MainView::rotateFunction(int x, int y, int z) {
    // Rotate
    transformCube.rotate(x, 1, 0, 0);
    transformCube.rotate(y, 0, 1, 0);
    transformCube.rotate(z, 0, 0, 1);

    transformPyramid.rotate(x, 1, 0, 0);
    transformPyramid.rotate(y, 0, 1, 0);
    transformPyramid.rotate(z, 0, 0, 1);

    transformSphere.rotate(x, 1, 0, 0);
    transformSphere.rotate(y, 0, 1, 0);
    transformSphere.rotate(z, 0, 0, 1);
}

void MainView::scaleFunction(float s) {
    // Set scale
    transformCube.scale(s);
    transformPyramid.scale(s);
    transformSphere.scale(s);
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
    float newAspect = (float) newWidth / (float) newHeight;
    projectTransform.setToIdentity();
    projectTransform.perspective(60.0f, newAspect * 1.0f, 1.0f, 100.0f);
    update();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    transformCube = QMatrix4x4();
    transformCube.translate(2, 0, -6);

    transformPyramid = QMatrix4x4();
    transformPyramid.translate(-2, 0, -6);

    transformSphere = QMatrix4x4();
    transformSphere.translate(0, 0, -10);

    // initialize global rotation variables, so they can be used again before scaling.
    globalRotateX = rotateX;
    globalRotateY = rotateY;
    globalRotateZ = rotateZ;

    scaleFunction(globalScale);
    rotateFunction(rotateX, rotateY, rotateZ);

    update();
}

void MainView::setScale(int scale) {
    transformCube = QMatrix4x4();
    transformCube.translate(2, 0, -6);

    transformPyramid = QMatrix4x4();
    transformPyramid.translate(-2, 0, -6);

    transformSphere = QMatrix4x4();
    transformSphere.translate(0, 0, -10);

    // set global scale
    globalScale = scale / 100.0f;

    // rotation
    rotateFunction(globalRotateX, globalRotateY, globalRotateZ);

    // scaling
    scaleFunction(globalScale);

    update();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged(const QOpenGLDebugMessage &Message) {
    qDebug() << " → Log:" << Message;
}

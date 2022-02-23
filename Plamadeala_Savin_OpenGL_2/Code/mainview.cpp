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
    makeCurrent();

    glDeleteBuffers(1, &VBOCat);
    glDeleteVertexArrays(1, &VAOCat);
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
    qDebug() << ":: Using QT" << QT_VERSION;

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Set the color to be used by glClear. This is, effectively, the background color.
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();

    loadTexture(":/textures/cat_diff.png", &catTexture);
    loadMesh();

    sendVertexData(catVertexArray);

    updateProjectTransform();
    updateCatTransform();
}

void MainView::loadMesh() {
    // load sphere object
    Model catModel(":/models/cat.obj");
    catModel.unitize();

    catVertices = catModel.getVertices();
    catSize = catVertices.size();
    auto catNormals = catModel.getNormals();
    auto catTextureCoords = catModel.getTextureCoords();
    catVertexArray.reserve(8 * catSize);

    for (int i = 0; i < catSize; ++i) {
        catVertexArray.emplace_back(Vertex(catVertices[i],
                                           catNormals[i],
                                           catTextureCoords[i]));
    }
}

void MainView::loadTexture(const QString &file, GLuint *texturePtr) {
    glGenTextures(1, texturePtr);
    glBindTexture(GL_TEXTURE_2D, *texturePtr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // enable anisotropic filtering
    GLfloat f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&f);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,f);

    image = QImage(file);
    imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 image.width(),
                 image.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 imageData.data());
}

/**
 * @brief MainView::sendVertexData
 *
 * Called upon OpenGL initialization
 * Sends the layout of the data and the data itself to the GPU
 * @param cat
 * @param cube
 * @param pyramid
 */
void MainView::sendVertexData(std::vector<Vertex> cat) {
    makeCurrent();

    // Cat
    Vertex localVertexArrayCat[cat.size()];
    std::copy(cat.begin(), cat.end(), localVertexArrayCat);

    // Generate VAO
    glGenVertexArrays(1, &VAOCat);
    glBindVertexArray(VAOCat);

    // Generate VBO
    glGenBuffers(1, &VBOCat);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCat);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(localVertexArrayCat), localVertexArrayCat, GL_STATIC_DRAW);

    // Telling the GPU the layout of the data

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // Set colour coordinates to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Set texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MainView::createShaderProgram() {
    /* 0 for phong
     * 1 for normal
     * 2 for gouraud */

    // Create shader program

    shaderProgram[ShadingMode::PHONG].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                              ":/shaders/phong_vertshader.glsl");
    shaderProgram[ShadingMode::PHONG].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                              ":/shaders/phong_fragshader.glsl");
    shaderProgram[ShadingMode::PHONG].link();

    modelLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("modelTransform");
    projectionLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("projectionTransform");
    normalLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("normalTransform");
    lightLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("lightCoordinates");
    materialLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("material");
    textureLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("textureColor");
    exponentLocation[ShadingMode::PHONG] = shaderProgram[ShadingMode::PHONG].uniformLocation("exponent");

    shaderProgram[ShadingMode::NORMAL].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                               ":/shaders/normal_vertshader.glsl");
    shaderProgram[ShadingMode::NORMAL].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                               ":/shaders/normal_fragshader.glsl");

    shaderProgram[ShadingMode::NORMAL].link();

    modelLocation[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("modelTransform");
    projectionLocation[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("projectionTransform");
    normalLocation[ShadingMode::NORMAL] = shaderProgram[ShadingMode::NORMAL].uniformLocation("normalTransform");

    shaderProgram[ShadingMode::GOURAUD].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                                ":/shaders/gouraud_vertshader.glsl");
    shaderProgram[ShadingMode::GOURAUD].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                                ":/shaders/gouraud_fragshader.glsl");

    shaderProgram[ShadingMode::GOURAUD].link();

    modelLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("modelTransform");
    projectionLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("projectionTransform");
    normalLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("normalTransform");
    lightLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("lightCoordinates");
    materialLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("material");
    textureLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("textureColor");
    exponentLocation[ShadingMode::GOURAUD] = shaderProgram[ShadingMode::GOURAUD].uniformLocation("exponent");

    shaderProgram[ShadingMode::ZBRUSH].addShaderFromSourceFile(QOpenGLShader::Vertex,
                                                               ":/shaders/zbuffer_vertshader.glsl");
    shaderProgram[ShadingMode::ZBRUSH].addShaderFromSourceFile(QOpenGLShader::Fragment,
                                                               ":/shaders/zbuffer_fragshader.glsl");

    shaderProgram[ShadingMode::ZBRUSH].link();

    modelLocation[ShadingMode::ZBRUSH] = shaderProgram[ShadingMode::ZBRUSH].uniformLocation("modelTransform");
    projectionLocation[ShadingMode::ZBRUSH] = shaderProgram[ShadingMode::ZBRUSH].uniformLocation("projectionTransform");
    normalLocation[ShadingMode::ZBRUSH] = shaderProgram[ShadingMode::ZBRUSH].uniformLocation("normalTransform");
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

    QMatrix3x3 normalTransformation = catTransform.normalMatrix();
    QVector3D material = {0.5f, 0.5f, 0.5f};
    shaderProgram[activeShaderProgram].bind();
    if (activeShaderProgram == ShadingMode::PHONG || activeShaderProgram == ShadingMode::GOURAUD) {
        glUniformMatrix4fv(projectionLocation[activeShaderProgram], 1, GL_FALSE, projectTransform.data());
        glUniformMatrix4fv(modelLocation[activeShaderProgram], 1, GL_FALSE, catTransform.data());
        glUniformMatrix3fv(normalLocation[activeShaderProgram], 1, GL_FALSE, normalTransformation.data());
        glUniform3f(lightLocation[activeShaderProgram],
                    lightPositionMatrix.x(),
                    lightPositionMatrix.y(),
                    lightPositionMatrix.z());
        glUniform3f(materialLocation[activeShaderProgram], material.x(), material.y(), material.z());
        glUniform1i(textureLocation[activeShaderProgram], 0);
        glUniform1i(exponentLocation[activeShaderProgram], exponent);
    } else if (activeShaderProgram == ShadingMode::NORMAL || activeShaderProgram == ShadingMode::ZBRUSH) {
        glUniformMatrix4fv(projectionLocation[activeShaderProgram], 1, GL_FALSE, projectTransform.data());
        glUniformMatrix4fv(modelLocation[activeShaderProgram], 1, GL_FALSE, catTransform.data());
        glUniformMatrix3fv(normalLocation[activeShaderProgram], 1, GL_FALSE, normalTransformation.data());
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, catTexture);

    glBindVertexArray(VAOCat);
    glDrawArrays(GL_TRIANGLES, 0, catSize);

    shaderProgram[activeShaderProgram].release();
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
    aspectRatio = (float) newWidth / (float) newHeight;

    updateProjectTransform();
}

void MainView::updateProjectTransform() {
    projectTransform.setToIdentity();
    projectTransform.perspective(60.0f, aspectRatio * 1.0f, 1.0f, 100.0f);

    update();
}

void MainView::updateCatTransform() {
    catTransform.setToIdentity();
    catTransform.translate(0, 0, -10);
    catTransform.scale(scale * 5);
    catTransform.rotate(QQuaternion::fromEulerAngles(rotation));

    update();
}

// --- Public interface
void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    auto x = (float) rotateX;
    auto y = (float) rotateY;
    auto z = (float) rotateZ;
    rotation = QVector3D(x, y, z);
    updateCatTransform();
}

void MainView::setScale(int newScale) {
    scale = static_cast<float>(newScale) / 100.0f;
    updateCatTransform();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    activeShaderProgram = shading;
}

void MainView::setLightPosition(int posX, int posY, int posZ) {
    auto x = static_cast<float>(posX);
    auto y = static_cast<float>(posY);
    auto z = static_cast<float>(posZ);
    lightPositionMatrix = {x, y, z};
    updateCatTransform();
}

void MainView::setPhongExponent(int newExponent) {
    exponent = static_cast<float>(newExponent);
    updateCatTransform();
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
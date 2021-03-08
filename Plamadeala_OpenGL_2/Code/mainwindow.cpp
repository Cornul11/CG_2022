#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

// --- Functions that listen for widget events
// forewards to the mainview

void MainWindow::on_ResetRotationButton_clicked(bool checked) {
    Q_UNUSED(checked);
    ui->RotationDialX->setValue(0);
    ui->RotationDialY->setValue(0);
    ui->RotationDialZ->setValue(0);
    ui->mainView->setRotation(0, 0, 0);
}

void MainWindow::on_RotationDialX_sliderMoved(int value) {
    ui->mainView->setRotation(value,
                              ui->RotationDialY->value(),
                              ui->RotationDialZ->value());
}

void MainWindow::on_RotationDialY_sliderMoved(int value) {
    ui->mainView->setRotation(ui->RotationDialX->value(),
                              value,
                              ui->RotationDialZ->value());
}

void MainWindow::on_RotationDialZ_sliderMoved(int value) {
    ui->mainView->setRotation(ui->RotationDialX->value(),
                              ui->RotationDialY->value(),
                              value);
}

void MainWindow::on_ResetScaleButton_clicked(bool checked) {
    Q_UNUSED(checked);
    ui->ScaleSlider->setValue(100);
    ui->mainView->setScale(100);
}

void MainWindow::on_ScaleSlider_sliderMoved(int value) {
    ui->mainView->setScale(value);
}

void MainWindow::on_PhongButton_toggled(bool checked) {
    if (checked) {
        ui->mainView->setShadingMode(MainView::PHONG);
        ui->mainView->update();
    }
}

void MainWindow::on_NormalButton_toggled(bool checked) {
    if (checked) {
        ui->mainView->setShadingMode(MainView::NORMAL);
        ui->mainView->update();
    }
}

void MainWindow::on_GouraudButton_toggled(bool checked) {
    if (checked) {
        ui->mainView->setShadingMode(MainView::GOURAUD);
        ui->mainView->update();
    }
}

void MainWindow::on_rotationLightX_sliderMoved(int position) {
    ui->mainView->setLightPosition(position, ui->rotationLightY->value(), ui->rotationLightZ->value());
}

void MainWindow::on_rotationLightY_sliderMoved(int position) {
    ui->mainView->setLightPosition(ui->rotationLightX->value(), position, ui->rotationLightZ->value());
}

void MainWindow::on_rotationLightZ_sliderMoved(int position) {
    ui->mainView->setLightPosition(ui->rotationLightY->value(), ui->rotationLightY->value(), position);
}

void MainWindow::on_resetLightPosition_clicked(bool checked) {
    Q_UNUSED(checked);
    ui->rotationLightX->setValue(50);
    ui->rotationLightY->setValue(50);
    ui->rotationLightZ->setValue(50);
    ui->mainView->setLightPosition(50, 50, 50);
}

void MainWindow::on_specularSlider_sliderMoved(int position) {
    ui->mainView->setPhongExponent(position);
}

void MainWindow::on_resetPhongExponent_clicked(bool checked) {
    Q_UNUSED(checked);
    ui->specularSlider->setValue(1);
    ui->mainView->setPhongExponent(1);
}
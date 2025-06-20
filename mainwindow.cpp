#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Настройка графика
    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->yAxis->setLabel("Y");
    ui->customPlot->legend->setVisible(true);

    // Подключение кнопок
   // connect(ui->browseButton1, &QPushButton::clicked, this, &MainWindow::on_browseButton1_clicked);
   // connect(ui->browseButton2, &QPushButton::clicked, this, &MainWindow::on_browseButton2_clicked);
   // connect(ui->plotButton, &QPushButton::clicked, this, &MainWindow::on_plotButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Реализация слотов
void MainWindow::on_browseButton1_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select File 1", "", "Text Files (*.txt)");
    if(!fileName.isEmpty()) {
        ui->file1Edit->setText(fileName);
    }
}

void MainWindow::on_browseButton2_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select File 2", "", "Text Files (*.txt)");
    if(!fileName.isEmpty()) {
        ui->file2Edit->setText(fileName);
    }
}

void MainWindow::on_plotButton_clicked()
{
    QVector<double> x1, y1, x2, y2;

    // Проверка, что хотя бы один файл выбран
    if(ui->file1Edit->text().isEmpty() && ui->file2Edit->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select at least one file!");
        return;
    }

    // Чтение первого файла
    if(!ui->file1Edit->text().isEmpty()) {
        if(!readDataFromFile(ui->file1Edit->text(), x1, y1)) {
            QMessageBox::warning(this, "Error", "Failed to read File 1");
            return;
        }
    }

    // Чтение второго файла
    if(!ui->file2Edit->text().isEmpty()) {
        if(!readDataFromFile(ui->file2Edit->text(), x2, y2)) {
            QMessageBox::warning(this, "Error", "Failed to read File 2");
            return;
        }
    }

    // Очистка предыдущих данных
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->graph(1)->data()->clear();

    // Установка данных для первого графика
    if (!x1.isEmpty()) {
        ui->customPlot->graph(0)->setData(x1, y1);
        ui->customPlot->graph(0)->setName("Curve 1");
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    }

    // Установка данных для второго графика
    if(!x2.isEmpty()) {
        ui->customPlot->graph(1)->setData(x2, y2);
        ui->customPlot->graph(1)->setName("Curve 2");
        ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    }

    // Автомасштабирование и перерисовка
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}

bool MainWindow::readDataFromFile(const QString& fileName, QVector<double>& x, QVector<double>& y)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(' ', Qt::SkipEmptyParts);

        if(values.size() >= 2) {
            bool ok1, ok2;
            double xVal = values[0].toDouble(&ok1);
            double yVal = values[1].toDouble(&ok2);

            if(ok1 && ok2) {
                x.append(xVal);
                y.append(yVal);
            }
        }
    }
    file.close();

    return !x.isEmpty();
}

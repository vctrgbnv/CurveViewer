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
    ui->customPlot->addGraph(); // Graph 0: Curve 1
    ui->customPlot->addGraph(); // Graph 1: Curve 2 (non-overlapping)
    ui->customPlot->addGraph(); // Graph 2: Curve 2 (overlapping)
    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->yAxis->setLabel("Y");
    ui->customPlot->legend->setVisible(true);
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
    if (ui->file1Edit->text().isEmpty() && ui->file2Edit->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select at least one file!");
        return;
    }

    // Чтение первого файла
    if (!ui->file1Edit->text().isEmpty()) {
        if (!readDataFromFile(ui->file1Edit->text(), x1, y1)) {
            QMessageBox::warning(this, "Error", "Failed to read File 1");
            return;
        }
    }

    // Чтение второго файла
    if (!ui->file2Edit->text().isEmpty()) {
        if (!readDataFromFile(ui->file2Edit->text(), x2, y2)) {
            QMessageBox::warning(this, "Error", "Failed to read File 2");
            return;
        }
    }

    // Очистка предыдущих данных
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->graph(1)->data()->clear();
    ui->customPlot->graph(2)->data()->clear();

    // Установка данных для первого графика
    if (!x1.isEmpty()) {
        ui->customPlot->graph(0)->setData(x1, y1);
        ui->customPlot->graph(0)->setName("Curve 1");
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    }

    // Установка данных для второго графика
    if (!x2.isEmpty() && !x1.isEmpty()) {
        QVector<double> x2_overlap, y2_overlap, x2_non_overlap, y2_non_overlap;
        const double epsilon = 1e-5; // Погрешность для сравнения

        // Предполагаем, что x1 и x2 совпадают
        if (x1.size() == x2.size()) {
            for (int i = 0; i < x1.size(); ++i) {
                if (std::abs(y1[i] - y2[i]) < epsilon) {
                    x2_overlap.append(x2[i]);
                    y2_overlap.append(y2[i]);
                } else {
                    x2_non_overlap.append(x2[i]);
                    y2_non_overlap.append(y2[i]);
                }
            }
        } else {
            QMessageBox::warning(this, "Error", "Curves must have the same x values");
            return;
        }

        // График для неперекрывающихся участков
        ui->customPlot->graph(1)->setData(x2_non_overlap, y2_non_overlap);
        ui->customPlot->graph(1)->setName("Curve 2 (non-overlapping)");
        ui->customPlot->graph(1)->setPen(QPen(Qt::red));

        // График для перекрывающихся участков
        ui->customPlot->graph(2)->setData(x2_overlap, y2_overlap);
        ui->customPlot->graph(2)->setName("Curve 2 (overlapping)");
        ui->customPlot->graph(2)->setPen(QPen(Qt::red, 0, Qt::DashLine));
    } else if (!x2.isEmpty()) {
        ui->customPlot->graph(1)->setData(x2, y2);
        ui->customPlot->graph(1)->setName("Curve 2");
        ui->customPlot->graph(1)->setPen(QPen(Qt::red));
    }

    // Автомасштабирование и перерисовка
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}
void MainWindow::on_swapButton_clicked()
{
    // Смена приоритета кривых
    auto data0 = ui->customPlot->graph(0)->data();
    auto data1 = ui->customPlot->graph(1)->data();
    auto data2 = ui->customPlot->graph(2)->data();

    // Меняем местами первую кривую и неперекрывающуюся часть второй
    ui->customPlot->graph(0)->setData(data1);
    ui->customPlot->graph(1)->setData(data0);
    ui->customPlot->graph(2)->setData(data2); // Перекрывающаяся часть остается

    // Обновление имен и стилей
    ui->customPlot->graph(0)->setName("Curve 2 (non-overlapping)");
    ui->customPlot->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot->graph(1)->setName("Curve 1");
    ui->customPlot->graph(1)->setPen(QPen(Qt::blue));
    ui->customPlot->graph(2)->setName("Curve 2 (overlapping)");
    ui->customPlot->graph(2)->setPen(QPen(Qt::red, 0, Qt::DashLine));

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

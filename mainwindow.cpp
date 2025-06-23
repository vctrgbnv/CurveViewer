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
    setWindowIcon(QIcon(":/icons/icon.png"));
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
    // Извлечение имен файлов
    QString fileName1 = ui->file1Edit->text().isEmpty() ? "Curve 1" : QFileInfo(ui->file1Edit->text()).baseName();
    QString fileName2 = ui->file2Edit->text().isEmpty() ? "Curve 2" : QFileInfo(ui->file2Edit->text()).baseName();
    // Установка данных для первого графика
    if (!x1.isEmpty()) {
        ui->customPlot->graph(0)->setData(x1, y1);
        ui->customPlot->graph(0)->setName(fileName1);
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    }

    // Установка данных для второго графика
        if (!x2.isEmpty()) {
            if (x1.isEmpty()) {
                // Если первой кривой нет, вся вторая кривая сплошная
                ui->customPlot->graph(1)->setData(x2, y2);
                ui->customPlot->graph(1)->setName(fileName2);
                ui->customPlot->graph(1)->setPen(QPen(Qt::red));
            } else {
                // Разделение второй кривой на перекрывающиеся и неперекрывающиеся участки
                QVector<double> x2_overlap, y2_overlap, x2_non_overlap, y2_non_overlap;

                for (int i = 0; i < x2.size(); ++i) {
                    //double y1_interp = interpolate(x1, y1, x2[i]);
                    if (y2[i] < y1[i]) {// \\i < x1.size
                        // Y2 < Y1 и X2 в диапазоне X1 — перекрытие (пунктир)
                        x2_overlap.append(x2[i]);
                        y2_overlap.append(y2[i]);
                    } else {
                        // Y2 >= Y1 или X2 вне диапазона X1 — нет перекрытия (сплошная)
                        x2_non_overlap.append(x2[i]);
                        y2_non_overlap.append(y2[i]);
                    }
                }

                // График для неперекрывающихся участков (сплошная линия)
                ui->customPlot->graph(1)->setData(x2_non_overlap, y2_non_overlap);
                ui->customPlot->graph(1)->setName(fileName2);
                ui->customPlot->graph(1)->setPen(QPen(Qt::red));

                // График для перекрывающихся участков (пунктир)
                ui->customPlot->graph(2)->setData(x2_overlap, y2_overlap);
                ui->customPlot->graph(2)->setName(fileName2 + " (overlapping)");
                ui->customPlot->graph(2)->setPen(QPen(Qt::red, 0, Qt::DashLine));
            }
        }

    // Автомасштабирование и перерисовка
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}
void MainWindow::on_swapButton_clicked()
{
    // 1) Меняем местами тексты в полях выбора файлов
    QString tmp = ui->file1Edit->text();
    ui->file1Edit->setText(ui->file2Edit->text());
    ui->file2Edit->setText(tmp);

    // 2) Перестраиваем график «по-новой»
    on_plotButton_clicked();
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

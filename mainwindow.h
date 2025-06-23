#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();  // Объявление деструктора

private slots:
    void on_browseButton1_clicked();
    void on_browseButton2_clicked();
    void on_plotButton_clicked();
    void on_swapButton_clicked();

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    bool readDataFromFile(const QString& fileName, QVector<double>& x, QVector<double>& y);
};

#endif // MAINWINDOW_H

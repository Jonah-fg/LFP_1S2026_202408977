#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Declaraciones adelantadas
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void cargarArchivo();
    void analizar();
    void generarReportes();

private:
    Ui::MainWindow *ui;
};

#endif
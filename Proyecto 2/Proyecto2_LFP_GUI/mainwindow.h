#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "NodoArbol.h"
#include "Token.h"
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
    std::vector<Token> ultimosTokens;
    NodoArbol* ultimaRaiz;
};

#endif
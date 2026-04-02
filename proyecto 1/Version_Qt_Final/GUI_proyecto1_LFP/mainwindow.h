#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <string>

#include "AnalizadorLexico.h"
#include "Parser.h"
#include "Token.h"
#include "ErrorLexico.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void mostrarErroresSintacticos(const std::vector<ErrorSintactico>& erroresSintacticos);
    void on_btnCargarArchivo_clicked();

    void on_btonAnalizar_clicked();

    void on_btonGenerarReportes_clicked();

    void on_btonDiagrama_clicked();

private:
    Ui::MainWindow *ui;

    std::string contenidoActual;
    std::vector<Token> tokensActuales;
    std::vector<ErrorLexico> erroresActuales;
    Parser* parser;
};
#endif // MAINWINDOW_H

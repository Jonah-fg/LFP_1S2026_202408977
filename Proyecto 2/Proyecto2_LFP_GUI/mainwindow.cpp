#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AnalizadorLexico.h"
#include "AnalizadorSintactico.h"
#include "GestorErrores.h"
#include "GeneradorReportes.h"
#include "Token.h"
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <sstream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Conectar botones con sus funciones
    connect(ui->btonCargar, &QPushButton::clicked, this, &MainWindow::cargarArchivo);
    connect(ui->btonAnalizar, &QPushButton::clicked, this, &MainWindow::analizar);
    connect(ui->btonReportes, &QPushButton::clicked, this, &MainWindow::generarReportes);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::cargarArchivo() {
    QString ruta = QFileDialog::getOpenFileName(this, "Abrir archivo .task", "", "Archivos Task (*.task);;Todos (*.*)");
    if (ruta.isEmpty()) return;

    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo.");
        return;
    }
    QString contenido =QString::fromUtf8(archivo.readAll());
    ui->editorCodigo->setPlainText(contenido);
    archivo.close();
}

void MainWindow::analizar() {
    // Obtener texto del editor
    string entrada=ui->editorCodigo->toPlainText().toStdString();

    // Limpiar tablas anteriores
    ui->tablaTokens->setRowCount(0);
    ui->tablaTokens->setColumnCount(4);
    ui->tablaTokens->setHorizontalHeaderLabels({"#", "Lexema", "Tipo", "Línea:Col"});

    ui->tablaErrores->setRowCount(0);
    ui->tablaErrores->setColumnCount(5);
    ui->tablaErrores->setHorizontalHeaderLabels({"#", "Lexema/Token", "Tipo", "Descripción", "Línea:Col"});

    // Análisis léxico
    GestorErrores gestor;
    AnalizadorLexico lexico(entrada, &gestor);
    vector<Token> tokens =lexico.analizarTodo();

    // Llenar tabla de tokens
    for (size_t i =0; i<tokens.size(); ++i) {
        Token &t = tokens[i];
        int fila =ui->tablaTokens->rowCount();
        ui->tablaTokens->insertRow(fila);
        ui->tablaTokens->setItem(fila, 0, new QTableWidgetItem(QString::number(i+1)));
        ui->tablaTokens->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(t.lexema)));
        string tipoStr =tipoTokenToString(t.tipo);
        ui->tablaTokens->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(tipoStr)));
        ui->tablaTokens->setItem(fila, 3, new QTableWidgetItem(QString("L:%1 C:%2").arg(t.linea).arg(t.columna)));
    }

    // Análisis sintáctico
    AnalizadorSintactico sintactico(tokens, &gestor);
    NodoArbol* raiz = sintactico.analizar();

    // Llenar tabla de errores
    vector<EntradaError> errores = gestor.obtenerTodos();
    for (size_t i=0; i< errores.size(); ++i) {
        EntradaError &e = errores[i];
        int fila =ui->tablaErrores->rowCount();
        ui->tablaErrores->insertRow(fila);
        ui->tablaErrores->setItem(fila, 0, new QTableWidgetItem(QString::number(e.numero)));
        ui->tablaErrores->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(e.lexemaOToken)));
        ui->tablaErrores->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(e.tipo)));
        ui->tablaErrores->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(e.descripcion)));
        ui->tablaErrores->setItem(fila, 4, new QTableWidgetItem(QString("L:%1 C:%2").arg(e.linea).arg(e.columna)));
    }

    // Generar reportes si no hay errores (automático)
    if (errores.empty()) {
        GeneradorReportes::generarReporteKanban(tokens, "reporte_kanban.html");
        GeneradorReportes::generarReporteCarga(tokens, "reporte_carga.html");
        GeneradorReportes::generarReporteTareasFechas(tokens, "reporte_tareas_fechas.html");
        GeneradorReportes::generarArbolDot(raiz, "arbol.dot");
        QMessageBox::information(this, "Éxito", "Análisis completado sin errores.\n" "Reportes generados:\n" "- reporte_kanban.html\n" "- reporte_carga.html\n" "- reporte_tareas_fechas.html\n" "- arbol.dot");
    } else {
        QMessageBox::warning(this, "Errores", "Se encontraron errores. No se generaron reportes.");
    }
    delete raiz;
}

void MainWindow::generarReportes() {
    // Puedes reutilizar la función analizar() o pedir que primero se analice.
    analizar();
}
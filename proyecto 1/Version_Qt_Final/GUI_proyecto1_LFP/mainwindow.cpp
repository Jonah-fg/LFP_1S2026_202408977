#include "mainwindow.h"
#include "GeneradorReporte.h"
#include "AnalizadorLexico.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , parser(nullptr)
{
    ui->setupUi(this);

    // Configurar tablas
    ui->tablaTokens->setColumnCount(5);
    QStringList headersTokens = {"No.", "Lexema", "Tipo", "Línea", "Columna"};
    ui->tablaTokens->setHorizontalHeaderLabels(headersTokens);

    ui->tablaErrores->setColumnCount(7);
    QStringList headersErrores = {"No.", "Lexema", "Tipo Error", "Descripción", "Línea", "Columna", "Gravedad"};
    ui->tablaErrores->setHorizontalHeaderLabels(headersErrores);

    std::cout <<"Constructor terminado" << std::endl;
}

MainWindow::~MainWindow()
{
    if (parser != nullptr) {
        delete parser;
    }
    delete ui;
    std::cout << "Destructor terminado" << std::endl;
}

void MainWindow::mostrarErroresSintacticos(const std::vector<ErrorSintactico>& erroresSintacticos) {
    for (const auto& error : erroresSintacticos) {
        ErrorLexico err(error.lexema, error.linea, error.columna);
        err.descripcion =error.mensaje;
        err.tipoError="Sintáctico";
        err.gravedad="ERROR";
        erroresActuales.push_back(err);
    }
}

void MainWindow::on_btnCargarArchivo_clicked()
{
    std::cout << "Click en Cargar Archivo" << std::endl;
    QString archivo = QFileDialog::getOpenFileName(this,"Abrir archivo .med", "", "Archivos MedLang (*.med)");

    std::cout << "Archivo seleccionado: " << archivo.toStdString() << std::endl;
    if (archivo.isEmpty()) {
        std::cout << "Archivo vacío, cancelando" << std::endl;
        return;
    }

    QFile file(archivo);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Error al abrir archivo" << std::endl;
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo");
        return;
    }

    std::cout << "Archivo abierto correctamente" << std::endl;

    QTextStream in(&file);
    QString contenidoQString = in.readAll();
    contenidoActual = contenidoQString.toStdString();
    ui->textEditCodigo->setText(contenidoQString);
    file.close();

    std::cout << "Archivo leído, tamaño: " << contenidoActual.size() << " bytes" << std::endl;

    // Limpiar tablas
    ui->tablaTokens->setRowCount(0);
    ui->tablaErrores->setRowCount(0);
    tokensActuales.clear();
    erroresActuales.clear();

    if (parser != nullptr) {
        delete parser;
        parser = nullptr;
    }

    QMessageBox::information(this, "Éxito", "Archivo cargado correctamente");
    std::cout << "Carga completada exitosamente" << std::endl;
}


void MainWindow::on_btonAnalizar_clicked()
{
    std::cout << "=== Iniciando análisis ===" << std::endl;

    if (contenidoActual.empty()) {
        std::cout << "Error: contenido vacío" << std::endl;
        QMessageBox::warning(this, "Error", "Primero carga un archivo .med");
        return;
    }

    std::cout << "Contenido cargado, tamaño: " << contenidoActual.size() << std::endl;

    // Limpiar tablas
    ui->tablaTokens->setRowCount(0);
    ui->tablaErrores->setRowCount(0);
    tokensActuales.clear();
    erroresActuales.clear();

    std::cout << "Creando analizador léxico..." << std::endl;

    // ANÁLISIS LÉXICO
    AnalizadorLexico lexer(contenidoActual);

    std::cout << "Iniciando tokenización..." << std::endl;

    Token tok = lexer.nextToken();
    int tokenCount =0;
    while (tok.type != TokenType::END_OF_FILE) {
        tokensActuales.push_back(tok);
        tokenCount++;
        tok = lexer.nextToken();
    }

    std::cout << "Tokenización completada: " << tokenCount << " tokens" << std::endl;
    erroresActuales = lexer.errores;
    std::cout << "Errores encontrados: " << erroresActuales.size() << std::endl;

    //TABLA DE TOKENS
    std::cout << "Llenando tabla de tokens..." << std::endl;
    ui->tablaTokens->setRowCount(tokensActuales.size());
    for (size_t i = 0; i < tokensActuales.size(); i++) {
        Token& t = tokensActuales[i];

        ui->tablaTokens->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tablaTokens->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t.lexema)));
        ui->tablaTokens->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(tokenTypeToString(t.type))));
        ui->tablaTokens->setItem(i, 3, new QTableWidgetItem(QString::number(t.linea)));
        ui->tablaTokens->setItem(i, 4, new QTableWidgetItem(QString::number(t.columna)));
    }

    // LLENAR TABLA DE ERRORES
    std::cout << "Llenando tabla de errores..." << std::endl;
    ui->tablaErrores->setRowCount(erroresActuales.size());
    for (size_t i = 0; i < erroresActuales.size(); i++) {
        ErrorLexico& e = erroresActuales[i];

        ui->tablaErrores->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tablaErrores->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.caracter)));
        ui->tablaErrores->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(e.tipoError)));
        ui->tablaErrores->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(e.descripcion)));
        ui->tablaErrores->setItem(i, 4, new QTableWidgetItem(QString::number(e.linea)));
        ui->tablaErrores->setItem(i, 5, new QTableWidgetItem(QString::number(e.columna)));
        ui->tablaErrores->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(e.gravedad)));
    }

    if (!tokensActuales.empty()) {
        parser=new Parser(tokensActuales);
        parser->parsearHospital();

        if (!parser->erroresSintacticos.empty()) {
            int erroresSintacticosCount=parser->erroresSintacticos.size();

            for (const auto& err : parser->erroresSintacticos) {
                ErrorLexico e(err.lexema, err.linea, err.columna);
                e.descripcion = err.mensaje;
                e.tipoError ="Sintáctico";
                e.gravedad="ERROR";
                erroresActuales.push_back(e);
            }

            int filaInicio =ui->tablaErrores->rowCount();
            ui->tablaErrores->setRowCount(filaInicio + erroresSintacticosCount);

            for (int i=0; i< parser->erroresSintacticos.size(); i++) {
                int fila =filaInicio+i;
                ErrorSintactico& e=parser->erroresSintacticos[i];

                ui->tablaErrores->setItem(fila, 0, new QTableWidgetItem(QString::number(fila + 1)));
                ui->tablaErrores->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(e.lexema)));
                ui->tablaErrores->setItem(fila, 2, new QTableWidgetItem("Sintáctico"));
                ui->tablaErrores->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(e.mensaje)));
                ui->tablaErrores->setItem(fila, 4, new QTableWidgetItem(QString::number(e.linea)));
                ui->tablaErrores->setItem(fila, 5, new QTableWidgetItem(QString::number(e.columna)));
                ui->tablaErrores->setItem(fila, 6, new QTableWidgetItem("ERROR"));
            }
            std::cout << "Errores sintácticos encontrados: " << parser->erroresSintacticos.size() << std::endl;
        }
    }
    ui->tablaTokens->resizeColumnsToContents();
    ui->tablaErrores->resizeColumnsToContents();

    QString mensaje = "Análisis completado:\n" + QString::number(tokensActuales.size()) + " tokens encontrados\n" + QString::number(erroresActuales.size()) + " errores encontrados";

    std::cout << "Análisis completado exitosamente" << std::endl;
    QMessageBox::information(this, "Resultado", mensaje);

}



void MainWindow::on_btonGenerarReportes_clicked()
{
    std::cout << "=== Iniciando generación de reportes ===" << std::endl;

    if (tokensActuales.empty()) {
        std::cout << "Error: tokens vacíos" << std::endl;
        QMessageBox::warning(this, "Error", "Primero analiza el archivo");
        return;
    }

    if (!erroresActuales.empty()|| (parser && !parser->erroresSintacticos.empty())) {
        std::cout << "Error: hay errores léxicos" << std::endl;
        QMessageBox::warning(this, "Error", "Hay errores léxicos. Corrígelos antes de generar reportes");
        return;
    }

    std::cout << "Tokens disponibles: " << tokensActuales.size() << std::endl;
    std::cout << "Creando parser..." << std::endl;

    // ANÁLISIS SINTÁCTICO
    if (parser == nullptr) {
        parser = new Parser(tokensActuales);
        std::cout << "Parser creado" << std::endl;
    }

    std::cout << "Iniciando parseo del hospital..." << std::endl;

    if (!parser->parsearHospital()) {
        std::cout << "Error en parseo" << std::endl;
        QMessageBox::warning(this, "Error", "Error en análisis sintctico. Revisa la consola");
        return;
    }

    if (!parser->erroresSintacticos.empty()) {
        mostrarErroresSintacticos(parser->erroresSintacticos);
        QMessageBox::warning(this, "Error", "Hay erores sintácticos. Revia la tabla de errores.");
        return;
    }

    std::cout << "Parseo completado" << std::endl;
    std::cout << "Pacientes: " << parser->pacientes.size() << std::endl;
    std::cout << "Médicos: " << parser->medicos.size() << std::endl;
    std::cout << "Citas: " << parser->citas.size() << std::endl;
    std::cout << "Diagnósticos: " << parser->diagnosticos.size() << std::endl;
    std::cout << "Creando generador de reportes..." << std::endl;

    // GENERAR REPORTES
    GeneradorReporte generador(parser->pacientes, parser->medicos, parser->citas, parser->diagnosticos, "Hospital General");
    std::cout << "Generando todos los reportes..." << std::endl;

    generador.generarTodosReportes();

    std::cout << "Reportes generados exitosamente" << std::endl;

    QMessageBox::information(this, "Éxito", "Reportes generados:\n" "- reporte1_pacienes.html\n" "- reporte2_medicos.html\n" "- reporte3_citas.html\n" "- reporte4_estadistico.html\n\n" "Abre los archivos desde la carpta del proyecto");
}



void MainWindow::on_btonDiagrama_clicked()
{
    std::cout << "=== Iniciando generación de diagrama DOT ===" << std::endl;

    if (tokensActuales.empty()) {
        std::cout << "Error: tokens vacíos"<< std::endl;
        QMessageBox::warning(this, "Error", "Primero analiza el archivo");
        return;
    }

    if (!erroresActuales.empty()) {
        std::cout << "Error: hay errores léxicos" << std::endl;
        QMessageBox::warning(this, "Error", "Hay errores léxicos. Corígelos antes de generar el diagrama");
        return;
    }

    std::cout << "Tokens disponibles: " << tokensActuales.size() << std::endl;

    if (parser==nullptr) {
        std::cout << "Creando parser..." << std::endl;
        parser = new Parser(tokensActuales);
        std::cout << "Iniciando parseo..." << std::endl;
        parser->parsearHospital();
        std::cout << "Parseo completado" << std::endl;
    }

    std::cout << "Creando generador de reportes..." << std::endl;

    // Generar el diagrama DOT
    GeneradorReporte generador(parser->pacientes, parser->medicos, parser->citas, parser->diagnosticos, "Hospital General");

    std::cout << "Generando archivo DOT..." << std::endl;

    generador.generarTodosReportes();

    std::cout << "Diagrama DOT generado: hospital.dot" << std::endl;

    QMessageBox::information(this, "Éxito", "Diagrama DOT generado: hospital.dot\n" "Para visalizar usa el comando:\n" "dot -Tpng hospital.dot -o hospital.png");
}


/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnCargarArchivo;
    QPushButton *btonAnalizar;
    QPushButton *btonGenerarReportes;
    QTextEdit *textEditCodigo;
    QTableWidget *tablaTokens;
    QTableWidget *tablaErrores;
    QPushButton *btonDiagrama;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(987, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnCargarArchivo = new QPushButton(centralwidget);
        btnCargarArchivo->setObjectName("btnCargarArchivo");
        btnCargarArchivo->setGeometry(QRect(170, 50, 131, 29));
        btonAnalizar = new QPushButton(centralwidget);
        btonAnalizar->setObjectName("btonAnalizar");
        btonAnalizar->setGeometry(QRect(420, 50, 131, 29));
        btonGenerarReportes = new QPushButton(centralwidget);
        btonGenerarReportes->setObjectName("btonGenerarReportes");
        btonGenerarReportes->setGeometry(QRect(640, 50, 151, 31));
        textEditCodigo = new QTextEdit(centralwidget);
        textEditCodigo->setObjectName("textEditCodigo");
        textEditCodigo->setGeometry(QRect(100, 100, 791, 151));
        tablaTokens = new QTableWidget(centralwidget);
        tablaTokens->setObjectName("tablaTokens");
        tablaTokens->setGeometry(QRect(100, 270, 381, 192));
        tablaErrores = new QTableWidget(centralwidget);
        tablaErrores->setObjectName("tablaErrores");
        tablaErrores->setGeometry(QRect(520, 270, 371, 192));
        btonDiagrama = new QPushButton(centralwidget);
        btonDiagrama->setObjectName("btonDiagrama");
        btonDiagrama->setGeometry(QRect(420, 500, 131, 29));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 987, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnCargarArchivo->setText(QCoreApplication::translate("MainWindow", "Cargar Archivo", nullptr));
        btonAnalizar->setText(QCoreApplication::translate("MainWindow", "Analizar", nullptr));
        btonGenerarReportes->setText(QCoreApplication::translate("MainWindow", "Generar Reportes", nullptr));
        btonDiagrama->setText(QCoreApplication::translate("MainWindow", "Ver Diagrama", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

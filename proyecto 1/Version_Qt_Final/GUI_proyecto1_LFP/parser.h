#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "Token.h"
#include "Paciente.h"
#include "Medico.h"
#include "Cita.h"
#include "Diagnostico.h"

struct ErrorSintactico {
    string mensaje;
    string lexema;
    int linea;
    int columna;

    ErrorSintactico(string msg, string lex, int lin, int col) : mensaje(msg), lexema(lex), linea(lin), columna(col) {}
};

class Parser {
private:
    vector<Token> tokens;
    int posicionActual;

    bool validarTipoSangre(const string& sangre);
    bool validarEspecialidad(const string& especialidad);
    bool validarDosis(const string& dosis);
    Token tokenActual();
    Token siguienteToken();
    bool coincidir(TokenType tipoEsperado);
    void reportarError(string mensaje, Token token);

public:
    Parser(vector<Token> tokens);
    vector<ErrorSintactico> erroresSintacticos;

    bool parsearHospital();
    void parsearPacientes();
    void parsearMedicos();
    void parsearCitas();
    void parsearDiagnosticos();

    // Datos resultantes
    vector<Paciente> pacientes;
    vector<Medico> medicos;
    vector<Cita> citas;
    vector<Diagnostico> diagnosticos;
    string nombreHospital;
};

#endif


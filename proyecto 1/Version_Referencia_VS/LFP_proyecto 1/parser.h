#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "Token.h"
#include "Paciente.h"
#include "Medico.h"
#include "Cita.h"
#include "Diagnostico.h"

class Parser {
private:
    vector<Token> tokens;
    int posicionActual;

    Token tokenActual();
    Token siguienteToken();
    bool coincidir(TokenType tipoEsperado);
    void reportarError(string mensaje, Token token);

public:
    Parser(vector<Token> tokens);
    vector<string> erroresSintacticos;

    // Métodos para parsear cada sección
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
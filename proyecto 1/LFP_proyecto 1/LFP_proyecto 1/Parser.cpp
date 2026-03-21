#include "Parser.h"
#include <iostream>

Parser::Parser(vector<Token> tokens){
    this->tokens=tokens;
    this->posicionActual=0;
}

Token Parser::tokenActual() {
    if (posicionActual<tokens.size()) {
        return tokens[posicionActual];
    }
    return Token(TokenType::END_OF_FILE, "", -1, -1);
}

Token Parser::siguienteToken() {
    if (posicionActual<tokens.size()) {
        posicionActual++;
    }
    return tokenActual();
}

bool Parser::coincidir(TokenType tipoEsperado) {
    if (tokenActual().type==tipoEsperado) {
        siguienteToken();
        return true;
    }
    return false;
}

//parseo Hospital
bool Parser::parsearHospital(){
    cout << "Iniciando análisis sintáctico..." << endl;

    if (!coincidir(TokenType::HOSPITAL)) {
        reportarError("Se esperaba 'HOSPITAL'", tokenActual());
        return false;
    }
    if (!coincidir(TokenType::LLAVE_ABRE)) {
        reportarError("Se esperaba '{' después de HOSPITAL", tokenActual());
        return false;
    }
    bool encontrePacientes =false;
    bool encontreMedicos =false;
    bool encontreCitas=false;
    bool encontreDiagnosticos=false;

    while (posicionActual< tokens.size() && tokenActual().type !=TokenType::LLAVE_CIERRA) {
        Token actual=tokenActual();

        switch (actual.type){

        case TokenType::PACIENTES:
            if (!encontrePacientes){
                encontrePacientes=true;
                siguienteToken();
                parsearPacientes();
            }
            else {
                reportarError("Sección PACIENTES duplicada", actual);
                siguienteToken();
            }
            break;

        case TokenType::MEDICOS:
            if (!encontreMedicos){
                encontreMedicos=true;
                siguienteToken();
                parsearMedicos();
            }
            else{
                reportarError("Sección MEDICOS duplicada", actual);
                siguienteToken();
            }
            break;

        case TokenType::CITAS:
            if (!encontreCitas){
                encontreCitas =true;
                siguienteToken();
                //parsearCitas();
            }
            else {
                reportarError("Sección CITAS duplicada", actual);
                siguienteToken();
            }
            break;

        case TokenType::DIAGNOSTICOS:
            if (!encontreDiagnosticos) {
                encontreDiagnosticos=true;
                siguienteToken();
                //parsearDiagnosticos();
            }
            else{
                reportarError("Sección DIAGNOSTICOS duplicada", actual);
                siguienteToken();
            }
            break;

        default:
            reportarError("Token inesperado en el nivel superior", actual);
            siguienteToken();
            break;
        }
    }
    if (tokenActual().type== TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }
    cout << "\n=== ANÁLISIS SINTÁCTICO COMPLETADO ===" << endl;
    cout << "Pacientes encontrados: " << pacientes.size() << endl;
    cout << "Médicos encontrados: " << medicos.size() << endl;
    cout << "Citas encontradas: " << citas.size() << endl;
    cout << "Diagnósticos encontrados: " << diagnosticos.size() << endl;
    cout << "Errores sintácticos: " << erroresSintacticos.size() << endl;

    return true;

}

//parseo Pacientes
void Parser::parsearPacientes(){
    if (!coincidir(TokenType::LLAVE_ABRE)){
        reportarError("Se esperaba '{' después de PACIENTES", tokenActual());
        return;
    }

    while (tokenActual().type!=TokenType::LLAVE_CIERRA && tokenActual().type!=TokenType::END_OF_FILE){
        if (tokenActual().type==TokenType::PACIENTE) {
            siguienteToken();  

            if (!coincidir(TokenType::DOS_PUNTOS)) {
                reportarError("Se esperaba ':' después de paciente", tokenActual());
                continue;
            }

            if (tokenActual().type==TokenType::CADENA) {
                string nombre=tokenActual().lexema;
                int lineaPaciente=tokenActual().linea;
                siguienteToken();

                if (coincidir(TokenType::CORCHETE_ABRE)){
                    int edad=0;
                    string tipoSangre="Desconocido";
                    bool encontreEdad=false;
                    bool encontreTipoSangre= false;

                    while (tokenActual().type!=TokenType::CORCHETE_CIERRA){
                        if (tokenActual().type==TokenType::CADENA && tokenActual().lexema=="edad"){
                            siguienteToken();

                            if (coincidir(TokenType::DOS_PUNTOS)){
                                if (tokenActual().type==TokenType::NUMERO){
                                    edad=stoi(tokenActual().lexema);
                                    siguienteToken();
                                }
                                else {
									reportarError("Se esperaba un número para la edad", tokenActual());
                                }
                            }
                        }
                        else if(tokenActual().type==TokenType::CADENA && tokenActual().lexema=="tipo_sangre"){
                            siguienteToken();

                            if (coincidir(TokenType::DOS_PUNTOS)) {
                                if (tokenActual().type==TokenType::CADENA) {
                                    tipoSangre=tokenActual().lexema;
									encontreTipoSangre =true;
                                    siguienteToken();
                                }
                                else {
                                    reportarError("Se esperaba una cadena paa tipo_sangre", tokenActual());
                                }
                            }
                        }
                        else{
                            siguienteToken();  
                        }
                        if (tokenActual().type ==TokenType::COMA) {
                            siguienteToken();
                        }
                    } 
                    if (!encontreEdad) {
                        reportarError("Paciente sin edad especificada",
                            Token(TokenType::ERROR, "", lineaPaciente, 0));
                    }
                    if (!encontreTipoSangre) {
                        reportarError("Paciente sin tipo de sangre especificado",
                            Token(TokenType::ERROR, "", lineaPaciente, 0));
                    }

                    if (tokenActual().type== TokenType::CORCHETE_CIERRA) {
                        siguienteToken();
                    }

                    Paciente paciente(nombre, edad, tipoSangre, lineaPaciente);
                    pacientes.push_back(paciente);
                    cout << "  → Paciente agregado: "<< nombre << ", " << edad << " años, sangre " << tipoSangre << endl;
                }
            }
            else{
                reportarError("Se esperaba el nombre del paciente etre comillas", tokenActual());
            }

            if (tokenActual().type == TokenType::COMA) {
                siguienteToken();
            }
        }
        else {
            reportarError("Se esperaba 'paciente'", tokenActual());
            siguienteToken();
        }
    }
    if (tokenActual().type==TokenType::LLAVE_CIERRA){
        siguienteToken();
    }
}


//parseo Medicos
void Parser::parsearMedicos() {
    cout << "\n--- Parseando MEDICOS ---" << endl;

    if (!coincidir(TokenType::LLAVE_ABRE)) {
        reportarError("Se esperaba '{' después de MEDICOS", tokenActual());
        return;
    }

    while (tokenActual().type!= TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {

        if (tokenActual().type==TokenType::MEDICO) {
            siguienteToken();

            if (!coincidir(TokenType::DOS_PUNTOS)) {
                reportarError("Se esperaba ':' después de 'medico'", tokenActual());
                continue;
            }

            if (tokenActual().type == TokenType::CADENA) {
                string nombre = tokenActual().lexema;
                int lineaMedico = tokenActual().linea;
                siguienteToken();

                if (coincidir(TokenType::CORCHETE_ABRE)) {
                    string especialidad = "Desconocida";
                    string codigo = "Desconocido";
                    bool encontreEspecialidad =false;
                    bool encontreCodigo=false;

                    while (tokenActual().type != TokenType::CORCHETE_CIERRA) {

                        if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema== "especialidad") {
                            siguienteToken();

                            if (coincidir(TokenType::DOS_PUNTOS)) {
                                if (tokenActual().type == TokenType::CARDIOLOGIA || tokenActual().type == TokenType::NEUROLOGIA || tokenActual().type == TokenType::PEDIATRIA || tokenActual().type == TokenType::CIRUGIA || tokenActual().type == TokenType::MEDICINA_GENERAL || tokenActual().type == TokenType::ONCOLOGIA) {

                                    especialidad=tokenActual().lexema;
                                    encontreEspecialidad= true;
                                    siguienteToken();
                                }
                                else {
                                    reportarError("Especialidd médica no válida", tokenActual());
                                }
                            }
                        }

                        else if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema == "codigo") {
                            siguienteToken();

                            if (coincidir(TokenType::DOS_PUNTOS)) {
                                if (tokenActual().type == TokenType::CODIGO_ID) {
                                    codigo = tokenActual().lexema;
                                    encontreCodigo = true;
                                    siguienteToken();
                                }
                                else {
                                    reportarError("Se esperaba un código válido (ej: MED-001)", tokenActual());
                                }
                            }
                        }
                        else {
                            siguienteToken();
                        }

                        if (tokenActual().type == TokenType::COMA) {
                            siguienteToken();
                        }
                    }

                    if (!encontreEspecialidad) {
                        reportarError("Médico sin especialidad especiicada",
                            Token(TokenType::ERROR, "", lineaMedico, 0));
                    }
                    if (!encontreCodigo) {
                        reportarError("Médico sin código especificado",
                            Token(TokenType::ERROR, "", lineaMedico, 0));
                    }

                    if (tokenActual().type == TokenType::CORCHETE_CIERRA) {
                        siguienteToken();
                    }

                    Medico medico(nombre, especialidad, codigo, lineaMedico);
                    medicos.push_back(medico);
                    cout << "  → Médico agregado: " << nombre << ", " << especialidad << ", código " << codigo << endl;
                }
            }
            else {
                reportarError("Se esperaba el nobre del médico entre comillas", tokenActual());
            }

            if (tokenActual().type == TokenType::COMA) {
                siguienteToken();
            }
        }
        else {
            reportarError("Se esperaba 'medico'", tokenActual());
            siguienteToken();
        }
    }

    if (tokenActual().type == TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }

    cout << " Total médicos encontrados: " << medicos.size() << endl;
}

//parseo Diagnostico



void Parser::reportarError(string mensaje, Token token){
    string error="Error sintáctico en línea " + to_string(token.linea) + ", columna " + to_string(token.columna) + ": " + mensaje + ". Encontrado: '" + token.lexema + "'";

    erroresSintacticos.push_back(error);

    cout << error << endl;
}
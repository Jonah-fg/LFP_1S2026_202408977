#include "Parser.h"
#include <iostream>
#include <set>

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

bool Parser::validarTipoSangre(const string& sangre) {
    string sangreSinComillas= sangre;
    if (sangreSinComillas.front()=='"' && sangreSinComillas.back() == '"') {
        sangreSinComillas =sangreSinComillas.substr(1, sangreSinComillas.length() - 2);
    }
    set<string> tiposValidos ={"A+", "A-", "B+", "B-", "O+", "O-", "AB+", "AB-"};
    return tiposValidos.find(sangreSinComillas) != tiposValidos.end();
}

bool Parser::validarEspecialidad(const string& especialidad) {
    set<string> especialidadesValidas ={"CARDIOLOGIA", "NEUROLOGIA", "PEDIATRIA", "CIRUGIA", "MEDICINA_GENERAL", "ONCOLOGIA"};
    return especialidadesValidas.find(especialidad) != especialidadesValidas.end();
}

bool Parser::validarDosis(const string& dosis) {
    set<string> dosisValidas ={"DIARIA", "CADA_8_HORAS", "CADA_12_HORAS", "SEMANAL"};
    return dosisValidas.find(dosis) != dosisValidas.end();
}


bool Parser::parsearHospital() {
    cout << "\n=== INICIANDO ANÁLISIS SINTÁCTICO ===" << endl;
    if (!coincidir(TokenType::HOSPITAL)) {
        reportarError("Se esperaba 'HOSPITAL'", tokenActual());
        return false;
    }
    if (!coincidir(TokenType::LLAVE_ABRE)) {
        reportarError("Se esperaba '{' después de HOSPITAL", tokenActual());
        return false;
    }

    nombreHospital="Hospital General San Carlos";

    bool encontrePacientes = false;
    bool encontreMedicos = false;
    bool encontreCitas=false;
    bool encontreDiagnosticos =false;

    while (posicionActual <tokens.size() && tokenActual().type != TokenType::LLAVE_CIERRA) {
        Token actual =tokenActual();

        if (actual.type == TokenType::COMA) {
            siguienteToken();
            continue;
        }
        switch (actual.type) {
        case TokenType::PACIENTES:
            if (!encontrePacientes) {
                encontrePacientes =true;
                siguienteToken();
                parsearPacientes();
            }
            else {
                reportarError("Sección PACIENTES duplicada", actual);
                siguienteToken();
            }
            break;

        case TokenType::MEDICOS:
            if (!encontreMedicos) {
                encontreMedicos=true;
                siguienteToken();
                parsearMedicos();
            }
            else {
                reportarError("Sección MEDICOS duplicada", actual);
                siguienteToken();
            }
            break;

        case TokenType::CITAS:
            if (!encontreCitas){
                encontreCitas=true;
                siguienteToken();
                parsearCitas();
            }
            else {
                reportarError("Sección CITAS duplicada", actual);
                siguienteToken();
            }
            break;
        case TokenType::DIAGNOSTICOS:
            if (!encontreDiagnosticos){
                encontreDiagnosticos=true;
                siguienteToken();
                parsearDiagnosticos();
            }
            else{
                reportarError("Sección DIAGNOSTICOS duplicada", actual);
                siguienteToken();
            }
            break;

        default:
            siguienteToken();
            break;
        }
    }
    if (tokenActual().type ==TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }
    else {
        reportarError("Se esperaba '}' para cerrar el bloque HOSPITAL", tokenActual());
    }
    cout << "\n=== ANÁLISIS SINTÁCTICO COMPLETADO ===" << endl;
    cout << "Pacientes encontrados: " << pacientes.size() << endl;
    cout << "Médicos encontrados: " << medicos.size() << endl;
    cout << "Citas encontradas: " << citas.size() << endl;
    cout << "Diagnósticos encontrados: " << diagnosticos.size() << endl;
    cout << "Errores sintácticos: " << erroresSintacticos.size() << endl;

    return true;
}



void Parser::parsearPacientes() {
    cout << "\n--- Parseando PACIENTES ---" << endl;

    if (tokenActual().type!=TokenType::LLAVE_ABRE) {
        reportarError("Se esperaba '{' después de PACIENTES", tokenActual());

        while (posicionActual < tokens.size() &&  tokenActual().type != TokenType::PACIENTE && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::MEDICOS && tokenActual().type != TokenType::CITAS && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::END_OF_FILE) {
            siguienteToken();
        }
        return;
    }

    siguienteToken();
    while (tokenActual().type !=TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
        if (tokenActual().type ==TokenType::PACIENTE){
            siguienteToken();

            if (!coincidir(TokenType::DOS_PUNTOS)) {
                reportarError("Se espeaba ':' después de 'paciente'", tokenActual());
                while (tokenActual().type !=TokenType::PACIENTE && tokenActual().type !=TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
                    siguienteToken();
                }
                continue;
            }

            if (tokenActual().type !=TokenType::CADENA) {
                reportarError("Se esperaba el nombre del paciente entre comillas", tokenActual());
                siguienteToken();
                continue;
            }

            string nombre=tokenActual().lexema;
            int lineaPaciente= tokenActual().linea;
            siguienteToken();

            if (!coincidir(TokenType::CORCHETE_ABRE)) {
                reportarError("Se esperaba '[' para abrir los atributos del paciente", tokenActual());
                continue;
            }
            int edad =-1;
            string tipoSangre="";
            int habitacion = -1;
            bool encontreEdad=false;
            bool encontreTipoSangre=false;
            bool encontreHabitacion = false;
            bool edadInvalida =false;
            bool tipoSangreInvalida= false;
            bool habitacionInvalida = false;

            while (tokenActual().type !=TokenType::CORCHETE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
                if (tokenActual().type==TokenType::IDENTIFICADOR && tokenActual().lexema == "edad") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'edad'", tokenActual());
                    }
                    else if (tokenActual().type == TokenType::NUMERO) {
                        edad =stoi(tokenActual().lexema);
                        encontreEdad= true;
                        siguienteToken();
                    }
                    else {
                        reportarError("Se esperaba un número para la edad", tokenActual());
                        edadInvalida=true;
                        encontreEdad= true;
                        siguienteToken();
                    }
                }
                else if (tokenActual().type ==TokenType::IDENTIFICADOR && tokenActual().lexema == "tipo_sangre") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'tipo_sangre'", tokenActual());
                    }
                    else if (tokenActual().type ==TokenType::CADENA) {
                        tipoSangre =tokenActual().lexema;
                        if (validarTipoSangre(tipoSangre)) {
                            encontreTipoSangre=true;
                        }
                        else {
                            reportarError("Tipo de sangre inválido. Valores permitidos: A+, A-, B+, B-, O+, O-, AB+, AB-", tokenActual());
                            tipoSangreInvalida=true;
                            encontreTipoSangre=true;
                        }
                        siguienteToken();
                    }
                    else {
                        reportarError("Se esperaba una cadena para tipo_sangre", tokenActual());
                    }
                }
                else if (tokenActual().type==TokenType::IDENTIFICADOR && tokenActual().lexema == "habitacion") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'habitacion'", tokenActual());
                    }

                    else if (tokenActual().type==TokenType::NUMERO) {
                        habitacion = stoi(tokenActual().lexema);
                        encontreHabitacion =true;
                        siguienteToken();
                    }

                    else {
                        reportarError("Se esperaba un número para habitacion", tokenActual());
                        habitacionInvalida=true;
                        encontreHabitacion=true;
                        siguienteToken();
                    }
                }
                else {
                    reportarError("Atributo inesperad en paciente. Se eseraba 'edad' o 'tipo_sangre'", tokenActual());
                    siguienteToken();
                }
                if (tokenActual().type ==TokenType::COMA) {
                    siguienteToken();
                }
            }

            if (tokenActual().type == TokenType::CORCHETE_CIERRA) {
                siguienteToken();
            }
            else {
                reportarError("Se esperaba ']' para cerrar los atributos del paciente", tokenActual());
            }

            if (!encontreEdad && !edadInvalida) {
                reportarError("Paciente sin edad especificada", Token(TokenType::ERROR, "", lineaPaciente, 0));
            }

            if (!encontreTipoSangre && !tipoSangreInvalida) {
                reportarError("Paciente sin tipo de sangre esecificado", Token(TokenType::ERROR, "", lineaPaciente, 0));
            }

            if (encontreEdad && encontreTipoSangre && !edadInvalida && !tipoSangreInvalida) {
                Paciente paciente(nombre, edad, tipoSangre, lineaPaciente);
                pacientes.push_back(paciente);
                cout << "→ Paciente agregado: " << nombre << ", " << edad << " años, sangre " << tipoSangre << endl;
            }

            if (!encontreTipoSangre && !tipoSangreInvalida) {
                reportarError("Paciente sin tipo de sangre especificado", Token(TokenType::ERROR, "", lineaPaciente, 0));
            }
        }
        else {
            reportarError("Se esperaba 'paciente'", tokenActual());
            siguienteToken();
        }

        if (tokenActual().type ==TokenType::COMA) {
            siguienteToken();
        }
    }

    if (tokenActual().type==TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }
    else {
        reportarError("Se esperaba '}' para cerrar el bloque PACIENTES", tokenActual());
    }
}


void Parser::parsearMedicos(){
    cout << "\n--- Parseando MEDICOS ---" << endl;

    if (tokenActual().type != TokenType::LLAVE_ABRE) {
        reportarError("Se esperaba '{' después de MEDICOS", tokenActual());

        while (posicionActual < tokens.size() && tokenActual().type != TokenType::CITAS &&tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
            siguienteToken();
        }
        return;
    }
    siguienteToken();

    while (tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
        if (tokenActual().type == TokenType::CITAS || tokenActual().type == TokenType::DIAGNOSTICOS || tokenActual().type == TokenType::PACIENTES || tokenActual().type == TokenType::MEDICOS) {
            cout << " → Saliendo de MEDICOS, encontrada nueva seción: " << tokenTypeToString(tokenActual().type) << endl;
            break;
        }

        if (tokenActual().type ==TokenType::MEDICO) {
            siguienteToken();

            if (!coincidir(TokenType::DOS_PUNTOS)) {
                reportarError("Se esperaba ':' después de 'medico'", tokenActual());

                while (tokenActual().type != TokenType::MEDICO && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::CITAS && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::END_OF_FILE) {
                    siguienteToken();
                }
                continue;
            }

            if (tokenActual().type != TokenType::CADENA) {
                reportarError("Se esperaba el nombre del médico entre comillas", tokenActual());
                siguienteToken();
                continue;
            }

            string nombre =tokenActual().lexema;
            int lineaMedico=tokenActual().linea;
            siguienteToken();

            if (tokenActual().type != TokenType::CORCHETE_ABRE) {
                reportarError("Se esperaba '[' para abrir los atributos del médico", tokenActual());

                while (tokenActual().type != TokenType::MEDICO && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::CITAS && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::END_OF_FILE) {
                    siguienteToken();
                }
                continue;
            }
            siguienteToken();

            string especialidad="";
            string codigo ="";
            bool encontreEspecialidad=false;
            bool encontreCodigo =false;
            bool especialidadInvalida=false;
            bool codigoInvalido=false;

            while (tokenActual().type != TokenType::CORCHETE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {

                if (tokenActual().type == TokenType::CITAS || tokenActual().type == TokenType::DIAGNOSTICOS || tokenActual().type == TokenType::PACIENTES ||tokenActual().type == TokenType::MEDICOS || tokenActual().type == TokenType::LLAVE_CIERRA) {
                    break;
                }

                if (tokenActual().type ==TokenType::IDENTIFICADOR && tokenActual().lexema =="especialidad") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'especialidad'", tokenActual());
                    }
                    else {
                        encontreEspecialidad = true;
                        if (tokenActual().type == TokenType::CARDIOLOGIA || tokenActual().type == TokenType::NEUROLOGIA || tokenActual().type == TokenType::PEDIATRIA || tokenActual().type == TokenType::CIRUGIA || tokenActual().type == TokenType::MEDICINA_GENERAL || tokenActual().type == TokenType::ONCOLOGIA) {
                            especialidad =tokenActual().lexema;
                            siguienteToken();
                        }
                        else {
                            reportarError("Especialidad médica no válida. Valores permitidos: CARDIOLOGIA, NEUROLOGIA, PEDIATRIA, CIRUGIA, MEDICINA_GENERAL, ONCOLOGIA", tokenActual());
                            especialidadInvalida = true;
                            siguienteToken();
                        }
                    }
                }
                else if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema == "codigo") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'codigo'", tokenActual());
                    }
                    else {
                        encontreCodigo = true;
                        if (tokenActual().type == TokenType::CADENA || tokenActual().type == TokenType::CODIGO_ID) {
                            codigo=tokenActual().lexema;

                            string codigoSinComillas=codigo;
                            if (codigoSinComillas.front() =='"' && codigoSinComillas.back() == '"') {
                                codigoSinComillas=codigoSinComillas.substr(1, codigoSinComillas.length()- 2);
                            }

                            bool codigoValido = false;
                            if (codigoSinComillas.length() >= 5 && isalpha(codigoSinComillas[0]) && isalpha(codigoSinComillas[1]) && isalpha(codigoSinComillas[2]) && codigoSinComillas[3] == '-') {
                                codigoValido =true;
                                for (int i = 4; i<codigoSinComillas.length(); i++) {
                                    if (!isdigit(codigoSinComillas[i])) {
                                        codigoValido = false;
                                        break;
                                    }
                                }
                            }
                            if (!codigoValido) {
                                reportarError("Formato de código inválido. Debe ser: 3 letras + guión + dígitos (ej: MED-001)", tokenActual());
                                codigoInvalido = true;
                            }
                            siguienteToken();
                        }
                        else {
                            reportarError("Se esperaba un código válido (ej: MED-001)", tokenActual());
                            codigoInvalido = true;
                            siguienteToken();
                        }
                    }
                }
                else {
                    reportarError("Atributo inesperado en médico. Se esperaba 'especialidad' o 'codigo'", tokenActual());
                    siguienteToken();
                }

                if (tokenActual().type ==TokenType::COMA) {
                    siguienteToken();
                }
            }

            if (tokenActual().type ==TokenType::CORCHETE_CIERRA) {
                siguienteToken();
            }
            else if (tokenActual().type != TokenType::CITAS && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::PACIENTES && tokenActual().type != TokenType::MEDICOS) {
                reportarError("Se esperaba ']' para cerrar los atributos del médico", tokenActual());
            }

            if (!encontreEspecialidad && !especialidadInvalida) {
                reportarError("Médico sin especialidad especificada", Token(TokenType::ERROR, "", lineaMedico, 0));
            }
            if (!encontreCodigo && !codigoInvalido) {
                reportarError("Médico sin código especificado", Token(TokenType::ERROR, "", lineaMedico, 0));
            }

            if (encontreEspecialidad && encontreCodigo && !especialidadInvalida && !codigoInvalido) {
                Medico medico(nombre, especialidad, codigo, lineaMedico);
                medicos.push_back(medico);
                cout << "  → Médico agregado: " << nombre << ", " << especialidad << ", código " << codigo << endl;
            }
        }
        else {
            if (tokenActual().type != TokenType::COMA && tokenActual().type != TokenType::LLAVE_CIERRA &&tokenActual().type != TokenType::CITAS && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::PACIENTES && tokenActual().type != TokenType::MEDICOS) {
                reportarError("Se esperaba 'medico'", tokenActual());
            }
            siguienteToken();
        }

        if (tokenActual().type == TokenType::COMA) {
            siguienteToken();
        }
    }

    if (tokenActual().type ==TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }
    else if (tokenActual().type != TokenType::CITAS && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::PACIENTES && tokenActual().type != TokenType::MEDICOS) {
        reportarError("Se esperaba '}' para cerrar el bloque MEDICOS", tokenActual());
    }
}


void Parser::parsearCitas() {
    cout << "\n--- Parseando CITAS ---" << endl;

    if (tokenActual().type != TokenType::LLAVE_ABRE) {
        reportarError("Se esperaba '{' después de CITAS", tokenActual());

        while (posicionActual < tokens.size() && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
            siguienteToken();
        }
        return;
    }
    siguienteToken();

    while (tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
        if (tokenActual().type == TokenType::DIAGNOSTICOS || tokenActual().type == TokenType::PACIENTES || tokenActual().type == TokenType::MEDICOS || tokenActual().type == TokenType::CITAS) {
            cout << "  → Saliendo de CITAS, encontrada nueva sección: " << tokenTypeToString(tokenActual().type) << endl;
            break;
        }

        if (tokenActual().type == TokenType::CITA) {
            siguienteToken();

            if (!coincidir(TokenType::DOS_PUNTOS)) {
                reportarError("Se esperaba ':' después de 'cita'", tokenActual());

                while (tokenActual().type != TokenType::CITA && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::END_OF_FILE) {
                    siguienteToken();
                }
                continue;
            }

            if (tokenActual().type != TokenType::CADENA) {
                reportarError("Se esperaba el nombre del paciente entre comillas", tokenActual());
                siguienteToken();
                continue;
            }

            string nombrePaciente = tokenActual().lexema;
            int lineaCita=tokenActual().linea;
            siguienteToken();

            if (tokenActual().type != TokenType::CON) {
                reportarError("Se esperaba 'con' entre el paciente y el médico", tokenActual());
                while (tokenActual().type != TokenType::CON && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::END_OF_FILE) {
                    siguienteToken();
                }
                if (tokenActual().type != TokenType::CON)
                    continue;
            }
            siguienteToken();

            if (tokenActual().type != TokenType::CADENA) {
                reportarError("Se esperaba el nombre del médico entre comillas", tokenActual());
                siguienteToken();
                continue;
            }

            string nombreMedico = tokenActual().lexema;
            siguienteToken();

            if (tokenActual().type != TokenType::CORCHETE_ABRE) {
                reportarError("Se esperaba '[' para los detalles de la cita", tokenActual());
                continue;
            }
            siguienteToken();

            string fecha= "";
            string hora="";
            bool fechaEncontrada= false;
            bool horaEncontrada =false;
            bool fechaValida =false;
            bool horaValida=false;

            while (tokenActual().type != TokenType::CORCHETE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
                if (tokenActual().type == TokenType::DIAGNOSTICOS || tokenActual().type == TokenType::PACIENTES || tokenActual().type == TokenType::MEDICOS || tokenActual().type == TokenType::CITAS || tokenActual().type == TokenType::LLAVE_CIERRA) {
                    break;
                }

                if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema == "fecha") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'fecha'", tokenActual());
                    }
                    else {
                        fechaEncontrada = true;
                        if (tokenActual().type == TokenType::FECHA) {
                            fecha = tokenActual().lexema;
                            fechaValida = true;
                            siguienteToken();
                        }
                        else if (tokenActual().type ==TokenType::ERROR) {
                            siguienteToken();
                        }
                        else {
                            reportarError("Se esperaba una fecha válida (AAAA-MM-DD)", tokenActual());
                            siguienteToken();
                        }
                    }
                }
                else if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema =="hora") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'hora'", tokenActual());
                    }
                    else {
                        horaEncontrada =true;
                        if (tokenActual().type == TokenType::HORA) {
                            hora =tokenActual().lexema;
                            horaValida=true;
                            siguienteToken();
                        }
                        else if (tokenActual().type ==TokenType::ERROR) {
                            siguienteToken();
                        }
                        else {
                            reportarError("Se esperaba una hora válida (HH:MM)", tokenActual());
                            siguienteToken();
                        }
                    }
                }
                else {
                    reportarError("Atributo inesperado en cita. Se esperaba 'fecha' u 'hora'", tokenActual());
                    siguienteToken();
                }

                if (tokenActual().type == TokenType::COMA) {
                    siguienteToken();
                }
            }
            if (tokenActual().type == TokenType::CORCHETE_CIERRA) {
                siguienteToken();
            }
            else if (tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::PACIENTES && tokenActual().type != TokenType::MEDICOS && tokenActual().type != TokenType::CITAS) {
                reportarError("Se esperaba ']' para cerrar los detalles de la cita", tokenActual());
            }

            if (!fechaEncontrada) {
                reportarError("Cita sin fecha especificada", Token(TokenType::ERROR, "", lineaCita, 0));
            }
            if (!horaEncontrada) {
                reportarError("Cita sin hora especificada", Token(TokenType::ERROR, "", lineaCita, 0));
            }

            if (fechaValida && horaValida) {
                Cita cita(nombrePaciente, nombreMedico, fecha, hora, lineaCita);
                citas.push_back(cita);
                cout << " → Cita agregada: " << nombrePaciente << " con " << nombreMedico << " el " << fecha << " a las " << hora << endl;
            }
        }
        else {
            if (tokenActual().type != TokenType::COMA && tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::PACIENTES && tokenActual().type != TokenType::MEDICOS && tokenActual().type != TokenType::CITAS) {
                reportarError("Se esperaba 'cita'", tokenActual());
            }
            siguienteToken();
        }

        if (tokenActual().type ==TokenType::COMA) {
            siguienteToken();
        }
    }

    if (tokenActual().type == TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }
    else if (tokenActual().type != TokenType::DIAGNOSTICOS && tokenActual().type != TokenType::PACIENTES &&tokenActual().type != TokenType::MEDICOS && tokenActual().type != TokenType::CITAS) {
        reportarError("Se esperaba '}' para cerrar el bloque CITAS", tokenActual());
    }
}


void Parser::parsearDiagnosticos() {
    cout << "\n--- Parseando DIAGNOSTICOS ---" << endl;

    if (!coincidir(TokenType::LLAVE_ABRE)) {
        reportarError("Se esperaba '{' después de DIAGNOSTICOS", tokenActual());
        return;
    }

    while (tokenActual().type != TokenType::LLAVE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
        if (tokenActual().type == TokenType::DIAGNOSTICO) {
            siguienteToken();

            if (!coincidir(TokenType::DOS_PUNTOS)) {
                reportarError("Se esperaba ':' después de 'diagnostico'", tokenActual());
                continue;
            }

            if (tokenActual().type !=TokenType::CADENA) {
                reportarError("Se esperaba el nombre del paciente entre comillas", tokenActual());
                siguienteToken();
                continue;
            }

            string nombrePaciente= tokenActual().lexema;
            int lineaDiagnostico=tokenActual().linea;
            siguienteToken();

            if (!coincidir(TokenType::CORCHETE_ABRE)) {
                reportarError("Se esperaba '[' para los detalles del diagnóstico", tokenActual());
                continue;
            }

            string condicion = "";
            string medicamento = "";
            string dosis = "";
            bool encontreCondicion= false;
            bool encontreMedicamento= false;
            bool encontreDosis =false;
            bool dosisInvalida =false;

            while (tokenActual().type != TokenType::CORCHETE_CIERRA && tokenActual().type != TokenType::END_OF_FILE) {
                if (tokenActual().type ==TokenType::IDENTIFICADOR && tokenActual().lexema == "condicion") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'condicion'", tokenActual());
                    }
                    else if (tokenActual().type == TokenType::CADENA) {
                        condicion = tokenActual().lexema;
                        encontreCondicion = true;
                        siguienteToken();
                    }
                    else {
                        reportarError("Se esperaba una cadena para la condición", tokenActual());
                    }
                }
                else if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema == "medicamento") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'medicamento'", tokenActual());
                    }
                    else if (tokenActual().type == TokenType::CADENA) {
                        medicamento = tokenActual().lexema;
                        encontreMedicamento = true;
                        siguienteToken();
                    }
                    else {
                        reportarError("Se esperaba una cadena para el medicamento", tokenActual());
                    }
                }
                else if (tokenActual().type == TokenType::IDENTIFICADOR && tokenActual().lexema == "dosis") {
                    siguienteToken();
                    if (!coincidir(TokenType::DOS_PUNTOS)) {
                        reportarError("Se esperaba ':' después de 'dosis'", tokenActual());
                    }
                    else {
                        encontreDosis = true;
                        if (tokenActual().type == TokenType::DIARIA ||
                            tokenActual().type == TokenType::CADA_8_HORAS ||
                            tokenActual().type == TokenType::CADA_12_HORAS ||
                            tokenActual().type == TokenType::SEMANAL) {
                            dosis = tokenActual().lexema;
                            siguienteToken();
                        }
                        else {
                            reportarError("Dosis no válida. Use: DIARIA, CADA_8_HORAS, CADA_12_HORAS, SEMANAL", tokenActual());
                            dosisInvalida = true;
                            siguienteToken();
                        }
                    }
                }
                else {
                    reportarError("Atributo inesperado en diagnóstico. Se esperaba 'condicion', 'medicamento' o 'dosis'", tokenActual());
                    siguienteToken();
                }

                if (tokenActual().type ==TokenType::COMA) {
                    siguienteToken();
                }
            }

            if (tokenActual().type== TokenType::CORCHETE_CIERRA) {
                siguienteToken();
            }
            else {
                reportarError("Se esperaba ']' para cerrar los detalles del diagnóstico", tokenActual());
            }

            if (!encontreCondicion) {
                reportarError("Diagnóstico sin condición especificada", Token(TokenType::ERROR, "", lineaDiagnostico, 0));
            }
            if (!encontreMedicamento) {
                reportarError("Diagnóstico sin medicamento especificado", Token(TokenType::ERROR, "", lineaDiagnostico, 0));
            }

            if (!encontreDosis && !dosisInvalida) {
                reportarError("Diagnóstico sin dosis especificada", Token(TokenType::ERROR, "", lineaDiagnostico, 0));
            }

            if (encontreCondicion && encontreMedicamento && encontreDosis && !dosisInvalida) {
                Diagnostico diagnostico(nombrePaciente, condicion, medicamento, dosis, lineaDiagnostico);
                diagnosticos.push_back(diagnostico);
                cout << "  → Diagnóstico agregado: " << nombrePaciente << " - " << condicion << endl;
            }
        }
        else {
            reportarError("Se esperaba 'diagnostico'", tokenActual());
            siguienteToken();
        }

        if (tokenActual().type== TokenType::COMA) {
            siguienteToken();
        }
    }

    if (tokenActual().type==TokenType::LLAVE_CIERRA) {
        siguienteToken();
    }
    else {
        reportarError("Se esperaba '}' para cerrar el bloque DIAGNOSTICOS", tokenActual());
    }
}

void Parser::reportarError(string mensaje, Token token) {
    ErrorSintactico error(mensaje, token.lexema, token.linea, token.columna);
    erroresSintacticos.push_back(error);

    cout << " Error sintáctico en línea "<< token.linea << ", columna " << token.columna << ": " << mensaje << ". Encontrado: '" << token.lexema << "'" << endl;
}
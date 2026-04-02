#include "AnalizadorLexico.h"
#include <cctype>
#include <iostream>
#include <map>
#include <string>
using namespace std;

const map<string, TokenType> AnalizadorLexico::palabrasReservadas = {
    // Secciones principales
    {"HOSPITAL", TokenType::HOSPITAL},
    {"PACIENTES", TokenType::PACIENTES},
    {"MEDICOS", TokenType::MEDICOS},
    {"CITAS", TokenType::CITAS},
    {"DIAGNOSTICOS", TokenType::DIAGNOSTICOS},
    // Elementos
    {"paciente", TokenType::PACIENTE},
    {"medico", TokenType::MEDICO},
    {"cita", TokenType::CITA},
    {"diagnostico", TokenType::DIAGNOSTICO},
    // Palabra clave para citas
    {"con", TokenType::CON}
};

const map<string, TokenType> AnalizadorLexico::especialidadesValidas = {
    {"CARDIOLOGIA", TokenType::CARDIOLOGIA},
    {"NEUROLOGIA", TokenType::NEUROLOGIA},
    {"PEDIATRIA", TokenType::PEDIATRIA},
    {"CIRUGIA", TokenType::CIRUGIA},
    {"MEDICINA_GENERAL", TokenType::MEDICINA_GENERAL},
    {"ONCOLOGIA", TokenType::ONCOLOGIA}
};

const map<string, TokenType> AnalizadorLexico::dosisValidas = {
    {"DIARIA", TokenType::DIARIA},
    {"CADA_8_HORAS", TokenType::CADA_8_HORAS},
    {"CADA_12_HORAS", TokenType::CADA_12_HORAS},
    {"SEMANAL", TokenType::SEMANAL}
};

AnalizadorLexico::AnalizadorLexico(string input) {
    this->input = input;
    this->posicion = 0;
    this->linea = 1;
    this->columna = 1;
}

char AnalizadorLexico::siguienteCaracter() {
    if (posicion >= input.length()) {
        return '\0';
    }
    char c = input[posicion];
    posicion++;
    columna++;
    return c;
}

void AnalizadorLexico::retroceder() {
    if (posicion > 0) {
        posicion--;
        columna--;
    }
}

void AnalizadorLexico::reiniciarParaSiguienteToken(){
    inicioLexema = posicion;
    lineaInicioLexema =linea;
    columnaInicioLexema=columna;
}

void AnalizadorLexico::reportarError(string lexema, string descripcion, int lineaError, int columnaError) {
    ErrorLexico error(lexema, lineaError, columnaError);
    error.descripcion = descripcion;
    error.tipoError ="Léxico";
    error.gravedad="ERROR";
    errores.push_back(error);
}

bool AnalizadorLexico::esLetra(char c) {
    return isalpha(c) || c == '_';
}

bool AnalizadorLexico::esDigito(char c) {
    return isdigit(c);
}

bool AnalizadorLexico::esLetraMayuscula(char c) {
    return isupper(c);
}

bool AnalizadorLexico::esCaracterValido(char c) {
    if (isalnum(c) || c == ' ' || c == '\n' || c == '\t' || c == '\r')
        return true;
    if (c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',' || c == ';' || c == '"' || c == '-')
        return true;
    return false;
}

Token AnalizadorLexico::nextToken() {
    while (posicion < input.length() && isspace(input[posicion])) {
        if (input[posicion] == '\n') {
            linea++;
            columna = 1;
        } else {
            columna++;
        }
        posicion++;
    }

    if (posicion >= input.length()) {
        return Token(TokenType::END_OF_FILE, "", linea, columna);
    }

    reiniciarParaSiguienteToken();
    char c =input[inicioLexema];
    Estado estadoActual = Estado::INICIO;
    string lexemaActual = "";
    bool aceptado =false;
    TokenType tipoToken=TokenType::ERROR;

    while (!aceptado && posicion <= input.length()) {
        char currentChar =(posicion < input.length()) ? input[posicion] : '\0';

        switch (estadoActual) {
        case Estado::INICIO: {
            if (esLetra(currentChar)) {
                estadoActual =Estado::LEYENDO_ID;
                siguienteCaracter();

            }
            else if (esDigito(currentChar)) {
                estadoActual =Estado::LEYENDO_NUMERO;
                siguienteCaracter();

            }
            else if (currentChar == '"') {
                estadoActual = Estado::LEYENDO_CADENA;
                siguienteCaracter();

            }
            else if (currentChar == '{') {
                siguienteCaracter();
                return Token(TokenType::LLAVE_ABRE, "{", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == '}') {
                siguienteCaracter();
                return Token(TokenType::LLAVE_CIERRA, "}", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == '[') {
                siguienteCaracter();
                return Token(TokenType::CORCHETE_ABRE, "[", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == ']') {
                siguienteCaracter();
                return Token(TokenType::CORCHETE_CIERRA, "]", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == ':') {
                siguienteCaracter();
                return Token(TokenType::DOS_PUNTOS, ":", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == ',') {
                siguienteCaracter();
                return Token(TokenType::COMA, ",", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == ';') {
                siguienteCaracter();
                return Token(TokenType::PUNTO_COMA, ";", lineaInicioLexema, columnaInicioLexema);

            }
            else if (currentChar == '-') {
                string charStr(1, currentChar);
                reportarError(charStr, "Caracter '-' no válido por sí solo. Debe formar parte de un código ID (ej. MED-001) o una fecha (AAAA-MM-DD)", linea, columna);
                siguienteCaracter();
                return nextToken();
            }
            else {
                if (!esCaracterValido(currentChar)) {
                    string charStr(1, currentChar);
                    reportarError(charStr, "Caracter no permitido en el lenguaje", linea, columna);
                    siguienteCaracter();
                    return nextToken();
                }
                else {
                    siguienteCaracter();
                    reportarError(string(1, currentChar), "Caracter inesperado", lineaInicioLexema, columnaInicioLexema);
                    return Token(TokenType::ERROR, string(1, currentChar), lineaInicioLexema, columnaInicioLexema);
                }
            }
            break;
        }

        case Estado::LEYENDO_ID: {
            if (esLetra(currentChar) || esDigito(currentChar)) {
                siguienteCaracter();
            }
            else if (currentChar == '-') {
                estadoActual = Estado::LEYENDO_CODIGO_ID;
                siguienteCaracter();
            }
            else {
                aceptado= true;
                string palabra=input.substr(inicioLexema, posicion - inicioLexema);
                auto it =palabrasReservadas.find(palabra);
                if (it !=palabrasReservadas.end()) {
                    tipoToken = it->second;
                }
                else {
                    auto itEspec=especialidadesValidas.find(palabra);
                    if (itEspec !=especialidadesValidas.end()) {
                        tipoToken= itEspec->second;
                    }
                    else {
                        auto itDosis=dosisValidas.find(palabra);
                        if (itDosis !=dosisValidas.end()) {
                            tipoToken= itDosis->second;
                        }
                        else {
                            tipoToken = TokenType::IDENTIFICADOR;
                        }
                    }
                }
            }
            break;
        }

        case Estado::LEYENDO_CODIGO_ID: {
            if (esDigito(currentChar)) {
                siguienteCaracter();
            } else {
                aceptado =true;
                string codigo =input.substr(inicioLexema, posicion- inicioLexema);

                bool valido =false;
                if (codigo.length()>= 5 && isalpha(codigo[0]) && isalpha(codigo[1]) && isalpha(codigo[2]) && codigo[3] == '-') {
                    valido = true;
                    for (int i=4; i < codigo.length(); i++) {
                        if (!isdigit(codigo[i])){
                            valido =false;
                            break;
                        }
                    }
                }
                if (valido) {
                    tipoToken=TokenType::CODIGO_ID;
                }
                else {
                    reportarError(codigo, "Formato de código ID nválido. Debe ser: LETRAS-LETRAS-LETRAS-GUION-DGITOS (ej: MED-001)", lineaInicioLexema, columnaInicioLexema);
                    tipoToken = TokenType::ERROR;
                }
            }
            break;
        }

        case Estado::LEYENDO_NUMERO: {
            if (esDigito(currentChar)) {
                siguienteCaracter();
            }
            else if (currentChar == '-') {
                cout << "\n🔵 Encontrado '-' después de número, cambiando a LEYENDO_FECHA" << endl;
                estadoActual = Estado::LEYENDO_FECHA;
                siguienteCaracter();
            }
            else if (currentChar == ':') {
                estadoActual = Estado::LEYENDO_HORA;
                siguienteCaracter();
            }
            else {
                aceptado = true;
                tipoToken = TokenType::NUMERO;
            }
            break;
        }


        case Estado::LEYENDO_FECHA_ANIO: {
            if (esDigito(currentChar)) {
                siguienteCaracter();
            }
            else if (currentChar == '-') {
                estadoActual = Estado::LEYENDO_FECHA_MES;
                siguienteCaracter();
            }
            else {
                aceptado = true;
                string fecha = input.substr(inicioLexema, posicion - inicioLexema);
                reportarError(fecha, "Formato de fecha inválido. Se esperaba '-' después del año",
                              lineaInicioLexema, columnaInicioLexema);
                tipoToken = TokenType::ERROR;
            }
            break;
        }


        case Estado::LEYENDO_FECHA: {
            // Leer todo hasta encontrar un carácter que no sea dígito o '-'
            if (esDigito(currentChar) || currentChar == '-') {
                siguienteCaracter();
            }
            else {
                aceptado = true;
                string fecha = input.substr(inicioLexema, posicion - inicioLexema);

                // === DEPURACIÓN DETALLADA ===
                cout << "\n=== VALIDANDO FECHA ===" << endl;
                cout << "Fecha: '" << fecha << "'" << endl;
                cout << "Longitud: " << fecha.length() << endl;
                cout << "Caracteres: ";
                for (int i = 0; i < fecha.length(); i++) {
                    cout << fecha[i] << "(" << (int)fecha[i] << ") ";
                }
                cout << endl;

                // Validar formato AAAA-MM-DD
                bool formatoOk = (fecha.length() == 10);
                if (formatoOk) {
                    formatoOk = (isdigit(fecha[0]) && isdigit(fecha[1]) && isdigit(fecha[2]) && isdigit(fecha[3]) &&
                                 fecha[4] == '-' &&
                                 isdigit(fecha[5]) && isdigit(fecha[6]) &&
                                 fecha[7] == '-' &&
                                 isdigit(fecha[8]) && isdigit(fecha[9]));
                }

                cout << "Formato válido: " << (formatoOk ? "SI" : "NO") << endl;

                if (formatoOk) {
                    int mes = stoi(fecha.substr(5, 2));
                    int dia = stoi(fecha.substr(8, 2));
                    int anio = stoi(fecha.substr(0, 4));

                    cout << "Año: " << anio << ", Mes: " << mes << ", Día: " << dia << endl;

                    bool rangoOk = (mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31);
                    cout << "Rango válido: " << (rangoOk ? "SI" : "NO") << endl;

                    if (rangoOk) {
                        tipoToken = TokenType::FECHA;
                        cout << "✅ Fecha VÁLIDA: " << fecha << endl;
                    } else {
                        reportarError(fecha, "Fecha inválida: mes o día fuera de rango (Mes: 01-12, Día: 01-31)",
                                      lineaInicioLexema, columnaInicioLexema);
                        tipoToken = TokenType::ERROR;
                        cout << "❌ Fecha INVÁLIDA (rango)" << endl;
                    }
                } else {
                    reportarError(fecha, "Formato de fecha inválido. Debe ser AAAA-MM-DD",
                                  lineaInicioLexema, columnaInicioLexema);
                    tipoToken = TokenType::ERROR;
                    cout << "❌ Fecha INVÁLIDA (formato)" << endl;
                }
            }
            break;
        }


        case Estado::LEYENDO_FECHA_MES: {
            if (esDigito(currentChar)) {
                siguienteCaracter();
                // Después de leer los 2 dígitos del mes, el siguiente carácter debe ser un guión
                if ((posicion - inicioLexema) == 7) { // Hemos leído AAAA-MM (7 caracteres)
                    if (currentChar == '-') {
                        // Si ya tenemos el guión, pasamos al día
                        estadoActual = Estado::LEYENDO_FECHA_DIA;
                        siguienteCaracter();
                    }
                    // Si no, seguimos leyendo (podría ser que el día venga sin guión)
                }
            }
            else {
                // Si terminamos sin encontrar el día, es error
                aceptado = true;
                string fecha = input.substr(inicioLexema, posicion - inicioLexema);
                if (fecha.length() == 7) { // Solo AAAA-MM
                    // Intentar leer el día del siguiente token
                    // Esto es complicado, mejor usar otro enfoque
                }
                reportarError(fecha, "Formato de fecha inválido",
                              lineaInicioLexema, columnaInicioLexema);
                tipoToken = TokenType::ERROR;
            }
            break;
        }


        case Estado::LEYENDO_FECHA_DIA: {
            cout << "DEBUG - Estado: LEYENDO_FECHA_DIA, char: " << currentChar << ", pos: " << posicion << endl;
            if (esDigito(currentChar)) {
                siguienteCaracter();
            }
            else {
                aceptado = true;
                string fecha = input.substr(inicioLexema, posicion - inicioLexema);
                cout << "DEBUG - Fecha completa capturada: '" << fecha << "', longitud: " << fecha.length() << endl;

                // Validar formato
                if (fecha.length() == 10 &&
                    isdigit(fecha[0]) && isdigit(fecha[1]) && isdigit(fecha[2]) && isdigit(fecha[3]) &&
                    fecha[4] == '-' &&
                    isdigit(fecha[5]) && isdigit(fecha[6]) &&
                    fecha[7] == '-' &&
                    isdigit(fecha[8]) && isdigit(fecha[9])) {

                    int mes = stoi(fecha.substr(5, 2));
                    int dia = stoi(fecha.substr(8, 2));

                    if (mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31) {
                        tipoToken = TokenType::FECHA;
                        cout << "DEBUG - ✅ Fecha VÁLIDA: " << fecha << endl;
                    } else {
                        reportarError(fecha, "Fecha inválida: mes o día fuera de rango",
                                      lineaInicioLexema, columnaInicioLexema);
                        tipoToken = TokenType::ERROR;
                        cout << "DEBUG - ❌ Fecha INVÁLIDA (rango): " << fecha << endl;
                    }
                } else {
                    reportarError(fecha, "Formato de fecha inválido. Debe ser AAAA-MM-DD",
                                  lineaInicioLexema, columnaInicioLexema);
                    tipoToken = TokenType::ERROR;
                    cout << "DEBUG - ❌ Fecha INVÁLIDA (formato): " << fecha << endl;
                }
            }
            break;
        }

        case Estado::LEYENDO_HORA: {
            if (esDigito(currentChar)) {
                estadoActual = Estado::LEYENDO_HORA_MINUTOS;
                siguienteCaracter();
            }
            else {
                aceptado =true;
                string hora=input.substr(inicioLexema, posicion - inicioLexema);
                reportarError(hora, "Formato de hora inválido. Debe ser HH:MM (ej: 09:00)", lineaInicioLexema, columnaInicioLexema);
                tipoToken=TokenType::ERROR;
            }
            break;
        }

        case Estado::LEYENDO_HORA_MINUTOS: {
            if (esDigito(currentChar)) {
                siguienteCaracter();
            }
            else {
                aceptado =true;
                string hora=input.substr(inicioLexema, posicion - inicioLexema);

                if (hora.length()==5 && isdigit(hora[0]) && isdigit(hora[1]) && hora[2] == ':' && isdigit(hora[3]) && isdigit(hora[4])) {

                    int horas =stoi(hora.substr(0, 2));
                    int minutos= stoi(hora.substr(3, 2));

                    if (horas>=0 && horas <= 23 && minutos >=0 && minutos<=59) {
                        tipoToken = TokenType::HORA;
                    }
                    else {
                        reportarError(hora, "Hora inválida. Use rango 00:00 - 23:59", lineaInicioLexema, columnaInicioLexema);
                        tipoToken = TokenType::ERROR;
                    }
                }
                else{
                    reportarError(hora, "Formato de hora inválido. Debe ser HH:MM (ej: 09:00)", lineaInicioLexema, columnaInicioLexema);
                    tipoToken = TokenType::ERROR;
                }
            }
            break;
        }

        case Estado::LEYENDO_CADENA: {
            if (currentChar =='"') {
                siguienteCaracter();
                aceptado=true;
                tipoToken= TokenType::CADENA;
            }
            else if (currentChar=='\n' || currentChar== '\0') {
                aceptado =true;
                string cadena =input.substr(inicioLexema, posicion - inicioLexema);
                reportarError(cadena, "Cadna sin cerrar. Falta la omilla de cierre (\")", lineaInicioLexema, columnaInicioLexema);
                tipoToken = TokenType::ERROR;
                if (currentChar== '\n') {
                    linea++;
                    columna = 1;
                }
            }
            else{
                siguienteCaracter();
            }
            break;
        }

        default:
            aceptado=true;
            tipoToken=TokenType::ERROR;
            break;
        }
    }
    string lexemaFinal = input.substr(inicioLexema, posicion - inicioLexema);
    return Token(tipoToken, lexemaFinal, lineaInicioLexema, columnaInicioLexema);
}




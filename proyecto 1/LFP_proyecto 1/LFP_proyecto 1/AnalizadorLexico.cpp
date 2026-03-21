#include "AnalizadorLexico.h"
#include <cctype>
#include <map> 
#include <string>
using namespace std;

const map<string, TokenType> AnalizadorLexico::palabrasReservadas= {

    {"HOSPITAL", TokenType::HOSPITAL},
    {"PACIENTES", TokenType::PACIENTES},
    {"MEDICOS", TokenType::MEDICOS},
    {"CITAS", TokenType::CITAS},
    {"DIAGNOSTICOS", TokenType::DIAGNOSTICOS},

    {"paciente", TokenType::PACIENTE},
    {"medico", TokenType::MEDICO},
    {"cita", TokenType::CITA},
    {"diagnostico", TokenType::DIAGNOSTICO},

    {"edad", TokenType::IDENTIFICADOR},
    {"tipo_sangre", TokenType::IDENTIFICADOR},
    {"especialidad", TokenType::IDENTIFICADOR},
    {"codigo", TokenType::IDENTIFICADOR},
    {"condicion", TokenType::IDENTIFICADOR},
    {"medicamento", TokenType::IDENTIFICADOR},
    {"dosis", TokenType::IDENTIFICADOR},
    {"fecha", TokenType::IDENTIFICADOR},
    {"hora", TokenType::IDENTIFICADOR},
     {"con", TokenType::IDENTIFICADOR},

    {"CARDIOLOGIA", TokenType::CARDIOLOGIA},
    {"NEUROLOGIA", TokenType::NEUROLOGIA},
    {"PEDIATRIA", TokenType::PEDIATRIA},
    {"CIRUGIA", TokenType::CIRUGIA},
    {"MEDICINA_GENERAL", TokenType::MEDICINA_GENERAL},
    {"ONCOLOGIA", TokenType::ONCOLOGIA},

    {"DIARIA", TokenType::DIARIA},
    {"CADA_8_HORAS", TokenType::CADA_8_HORAS},
    {"CADA_12_HORAS", TokenType::CADA_12_HORAS},
    {"SEMANAL", TokenType::SEMANAL}
};

AnalizadorLexico::AnalizadorLexico(string input) {

    this->input=input;
    this->posicion=0;
    this->linea=1;
    this->columna= 1;
	this->inicioLexema = 0;
}

char AnalizadorLexico::siguienteCaracter() {
    if (posicion>=input.length()){
        return '\0';
    }
    char c=input[posicion];
    posicion++;
    columna++;
    return c;
}

void AnalizadorLexico::retroceder() {
    if (posicion>0){
        posicion--;
        columna--;
    }
}

Token AnalizadorLexico::crearToken(TokenType tipo) {
    string lexema=input.substr(inicioLexema, posicion-inicioLexema);
    return Token(tipo, lexema, linea, columna - (posicion-inicioLexema));
}

void AnalizadorLexico::reportarError(string lexema, string descripcion, int lineaError, int columnaError) {
    ErrorLexico error(lexema, lineaError, columnaError);
    error.descripcion=descripcion;
    error.tipoError="Léxico";
    error.gravedad="ERROR";
    errores.push_back(error);
}

Token AnalizadorLexico::nextToken() {
    Estado estadoActual=Estado::INICIO;
    string lexemaActual="";
    bool aceptado=false;
    TokenType tipoToken = TokenType::ERROR;

    while (posicion < input.length() && isspace(input[posicion])) {
        if (input[posicion] == '\n') {
            linea++;
            columna=1;
        }
        else {
            columna++;
        }
        posicion++;
    }

    if (posicion >= input.length()) {
        return Token(TokenType::END_OF_FILE, "", linea, columna);
    }

    inicioLexema=posicion;
    int lineaInicio=linea;
    int columnaInicio=columna;

    while (!aceptado && posicion<=input.length()){
        char c=(posicion<input.length()) ? input[posicion] :'\0';
        switch (estadoActual){

        case Estado::INICIO:{
            if (c=='\0') {
                aceptado=true;
                tipoToken=TokenType::END_OF_FILE;
            }
            else if (isalpha(c)) {
                estadoActual=Estado::LEYENDO_ID;
                siguienteCaracter();
            }
            else if (isdigit(c)) {
                estadoActual=Estado::LEYENDO_NUMERO;
                siguienteCaracter();
            }
            else if (c =='"') {
                estadoActual=Estado::LEYENDO_CADENA;
                siguienteCaracter();  
            }
            else if (c == '{') {
                siguienteCaracter();
                return Token(TokenType::LLAVE_ABRE, "{", lineaInicio, columnaInicio);
            }
            else if (c == '}') {
                siguienteCaracter();
                return Token(TokenType::LLAVE_CIERRA, "}", lineaInicio, columnaInicio);
            }
            else if (c =='['){
                siguienteCaracter();
                return Token(TokenType::CORCHETE_ABRE, "[", lineaInicio, columnaInicio);
            }
            else if (c ==']') {
                siguienteCaracter();
                return Token(TokenType::CORCHETE_CIERRA, "]", lineaInicio, columnaInicio);
            }
            else if (c == ':') {
                siguienteCaracter();
                return Token(TokenType::DOS_PUNTOS, ":", lineaInicio, columnaInicio);
            }
            else if (c ==',') {
                siguienteCaracter();
                return Token(TokenType::COMA, ",",lineaInicio, columnaInicio);
            }
            else if (c == ';') {
                siguienteCaracter();
                return Token(TokenType::PUNTO_COMA, ";", lineaInicio, columnaInicio);
            }
            else {
                string charStr(1, c);
                reportarError(charStr, "Caracter no permitido en el lenguaje", linea, columna);
                siguienteCaracter();  
                return nextToken();  
            }
            break;
        }

        case Estado::LEYENDO_ID: {
            if (isalnum(c) || c =='_') {
                siguienteCaracter();
            }

            else if (c =='-'){
                estadoActual=Estado::LEYENDO_CODIGO_ID;
                siguienteCaracter();
            }
            else {
                aceptado=true;
                string palabra=input.substr(inicioLexema, posicion - inicioLexema);

                auto buscador= palabrasReservadas.find(palabra);
                if (buscador != palabrasReservadas.end()) {
                    tipoToken =buscador->second;
                }
                else {
                    reportarError(palabra, "Palabra no reconocida", lineaInicio, columnaInicio);
                    tipoToken=TokenType::ERROR;
                }
            }
            break;
        }

        case Estado::LEYENDO_CODIGO_ID: {
            if (isdigit(c)){
                siguienteCaracter();
            }
            else {
                aceptado=true;
                string codigo=input.substr(inicioLexema, posicion - inicioLexema);

                if (codigo.length() >= 5 && isalpha(codigo[0]) && isalpha(codigo[1]) && isalpha(codigo[2]) && codigo[3] == '-') {
                    bool digitosValidos =true;
                    for (size_t i=4; i < codigo.length(); i++) {
                        if (!isdigit(codigo[i])) {
                            digitosValidos = false;
                            break;
                        }
                    }
                    if (digitosValidos){
                        tipoToken=TokenType::CODIGO_ID;
                    }
                    else{
                        reportarError(codigo, "Código inválido. Debe tener dígitos después del guión",lineaInicio, columnaInicio);
                        tipoToken =TokenType::ERROR;
                    }
                }
                else{
                    reportarError(codigo, "Formato de código inválido. Debe ser: LETRAS-DÍGITOS (ej: MED-001)",lineaInicio, columnaInicio);
                    tipoToken=TokenType::ERROR;
                }
            }
            break;
        }

               
        case Estado::LEYENDO_NUMERO: {
            if (isdigit(c)) {
                siguienteCaracter();
            }
            else if (c == '-'){
                estadoActual = Estado::LEYENDO_FECHA_DESPUES_GUION;
                siguienteCaracter();
            }
            else if (c == ':'){
                estadoActual=Estado::LEYENDO_HORA;
                siguienteCaracter();
            }
            else{
                aceptado=true;
                tipoToken=TokenType::NUMERO;
            }
            break;
        }

        case Estado::LEYENDO_FECHA_DESPUES_GUION: {
            if (isdigit(c)){
                estadoActual=Estado::LEYENDO_FECHA;
                siguienteCaracter();
            }
            else {
                aceptado=true;
                string numero=input.substr(inicioLexema, posicion-inicioLexema);
                reportarError(numero, "Número con guión inválido. Si es fecha use AAAA-MM-DD", lineaInicio, columnaInicio);
                tipoToken=TokenType::ERROR;
            }
            break;
        }

        case Estado::LEYENDO_FECHA:{
            if (isdigit(c)) {
                siguienteCaracter();
            }
            else if (c == '-') {
                estadoActual=Estado::LEYENDO_FECHA_DESPUES_GUION;
                siguienteCaracter();
            }
            else{
                aceptado=true;
                string fecha=input.substr(inicioLexema, posicion-inicioLexema);

                if (fecha.length()==10 && isdigit(fecha[0]) && isdigit(fecha[1]) && isdigit(fecha[2]) && isdigit(fecha[3]) && fecha[4] == '-' && isdigit(fecha[5]) && isdigit(fecha[6]) &&fecha[7] == '-' && isdigit(fecha[8]) && isdigit(fecha[9])){

                    int mes=stoi(fecha.substr(5, 2));
                    int dia=stoi(fecha.substr(8, 2));

                    if (mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31){
                        tipoToken=TokenType::FECHA;
                    }
                    else{
                        reportarError(fecha, "Fecha inválida. Mes debe ser 01-12 y día 01-31", lineaInicio, columnaInicio);
                        tipoToken=TokenType::ERROR;
                    }
                }
                else{
                    reportarError(fecha, "Formato de fecha inv{{lido. Use AAAA-MM-DD", lineaInicio, columnaInicio);
                    tipoToken = TokenType::ERROR;
                }
            }
            break;
        }

        case Estado::LEYENDO_HORA:{
            if (isdigit(c)){
                estadoActual=Estado::LEYENDO_HORA_SEGUNDA_PARTE;
                siguienteCaracter();
            }
            else{
                aceptado=true;
                string hora=input.substr(inicioLexema, posicion-inicioLexema);
                reportarError(hora, "Formato de hora inválido. Use HH:MM", lineaInicio, columnaInicio);
                tipoToken = TokenType::ERROR;
            }
            break;
        }

        case Estado::LEYENDO_HORA_SEGUNDA_PARTE: {
            if (isdigit(c)){
                siguienteCaracter();
            }
            else{
                aceptado=true;
                string hora=input.substr(inicioLexema, posicion - inicioLexema);

                if (hora.length() == 5 &&(hora[0]) && isdigit(hora[1]) && hora[2] == ':' &&isdigit(hora[3]) && isdigit(hora[4])) {
                    // hora (00-23) y minutos (00-59)
                    int horas = stoi(hora.substr(0, 2));
                    int minutos = stoi(hora.substr(3, 2));

                    if (horas>=0 && horas <= 23 && minutos >= 0 && minutos <= 59) {
                        tipoToken=TokenType::HORA;
                    }
                    else{
                        reportarError(hora, "Hora inváida. Use 00:00 - 23:59", lineaInicio, columnaInicio);
                        tipoToken=TokenType::ERROR;
                    }
                }
                else {
                    reportarError(hora, "Format de hora inválido. Use HH:MM", lineaInicio, columnaInicio);
                    tipoToken=TokenType::ERROR;
                }
            }
            break;
        }

        case Estado::LEYENDO_CADENA:{
            if (c =='"'){
                siguienteCaracter(); 
                aceptado=true;
                tipoToken=TokenType::CADENA;
            }
            else if (c=='\n'|| c=='\0'){

                aceptado=true;
                string cadena=input.substr(inicioLexema, posicion - inicioLexema);
                reportarError(cadena, "Cadena sin cerrar", lineaInicio, columnaInicio);
                tipoToken=TokenType::ERROR;
                if (c =='\n') {
                    linea++;
                    columna= 1;
                }
            }
            else {
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

    if (tipoToken!=TokenType::ERROR){
        string lexemaFinal=input.substr(inicioLexema, posicion - inicioLexema);
        return Token(tipoToken, lexemaFinal, lineaInicio, columnaInicio);
    }
    else{
        return nextToken();
    }
}


#include "AnalizadorLexico.h"
#include <cctype>   
using namespace std;

AnalizadorLexico::AnalizadorLexico(string entrada, GestorErrores* gestor) {
    fuente=entrada;
    pos =0;
    linea=1;
    columna=1;
    gestorErrores=gestor;
}

char AnalizadorLexico::caracterActual() {
    if (pos<fuente.length()) {
        return fuente[pos];
    }
    return '\0';
}

void AnalizadorLexico::avanzar() {
    if (pos <fuente.length()) {
        if (fuente[pos]== '\n') {
            linea++;
            columna=1;
        }
        else {
            columna++;
        }
        pos++;
    }
}

bool AnalizadorLexico::esFin() {
    return pos>=fuente.length();
}

void AnalizadorLexico::saltarEspacios() {
    while (!esFin() && isspace(caracterActual())) {
        avanzar();
    }
}

void AnalizadorLexico::retroceder() {
    if (pos > 0) {
        pos--;
        if (fuente[pos]== '\n') {
            linea--;
            columna=1;
        }
        else{
            columna--;
        }
    }
}

Token AnalizadorLexico::siguienteToken() {
    saltarEspacios();

    if (esFin()){
        return Token(FIN_ARCHIVO, "EOF", linea, columna);
    }

    int lineaInicio =linea;
    int columnaInicio =columna;
    char c=caracterActual();

    // Estado S0
    if (isalpha(c)) {
        string lexema="";
        while (!esFin() && (isalpha(caracterActual()) || isdigit(caracterActual()) || caracterActual() == '_')) {
            lexema+=caracterActual();
            avanzar();
        }

        TipoToken tipo=DESCONOCIDO;
        if (lexema== "TABLERO") 
            tipo =TABLERO;

        else if (lexema=="COLUMNA")
            tipo =COLUMNA;

        else if (lexema== "tarea") 
            tipo =TAREA;

        else if (lexema =="prioridad")
            tipo=PRIORIDAD;

        else if (lexema=="responsable")
            tipo =RESPONSABLE;

        else if (lexema =="fecha_limite")
            tipo =FECHA_LIMITE;

        else if (lexema=="ALTA")
            tipo =ALTA;

        else if (lexema =="MEDIA")
            tipo=MEDIA;

        else if (lexema== "BAJA")
            tipo=BAJA;

        if (tipo==DESCONOCIDO) {
            gestorErrores->agregarErrorLexico(lexema, lineaInicio, columnaInicio, "Palabra no reconocida: '" +lexema + "'");
        }
        return Token(tipo, lexema, lineaInicio, columnaInicio);
    }

    // Estado S2
    else if (isdigit(c)) {
        string lexema="";
        while (!esFin() && isdigit(caracterActual())) {
            lexema +=caracterActual();
            avanzar();
        }

        if (lexema.length()==4 && !esFin() && caracterActual()=='-') {
            string fechaLexema=lexema;
            fechaLexema+='-';
            avanzar(); 

            int digitosMes= 0;
            while (!esFin() && isdigit(caracterActual()) && digitosMes < 2) {
                fechaLexema+= caracterActual();
                avanzar();
                digitosMes++;
            }

            if (digitosMes==2 && !esFin() && caracterActual() == '-') {
                fechaLexema +='-';
                avanzar(); 

                int digitosDia=0;
                while (!esFin() && isdigit(caracterActual()) && digitosDia < 2) {
                    fechaLexema +=caracterActual();
                    avanzar();
                    digitosDia++;
                }

                if (digitosDia ==2) {
                    return Token(FECHA, fechaLexema, lineaInicio, columnaInicio);
                }
                else {
                    gestorErrores->agregarErrorLexico(fechaLexema, lineaInicio, columnaInicio, "Formato de fecha invalido (dia incompleto)");
                    return Token(DESCONOCIDO, fechaLexema, lineaInicio, columnaInicio);
                }
            }
            else {
                gestorErrores->agregarErrorLexico(fechaLexema, lineaInicio, columnaInicio, "Forato de fecha invalido (mes incompleto o falta '-')");
                return Token(DESCONOCIDO, fechaLexema, lineaInicio, columnaInicio);
            }
        }
        else {
            return Token(ENTERO, lexema, lineaInicio, columnaInicio);
        }
    }

    // Estado S3
    else if (c =='"') {
        string lexema="";
        lexema+='"';
        avanzar(); 

        while (!esFin() && caracterActual() !='"' && caracterActual() != '\n') {
            lexema+=caracterActual();
            avanzar();
        }

        if (esFin() || caracterActual()== '\n') {
            if (!lexema.empty() && lexema.back()!='"') {
                lexema.pop_back();
                retroceder();
            }
            gestorErrores->agregarErrorLexico(lexema, lineaInicio, columnaInicio, "Cadena sin cerrar antes de fin de linea");
            return Token(DESCONOCIDO, lexema, lineaInicio, columnaInicio);
        }
        lexema +='"';
        avanzar(); 
        return Token(CADENA, lexema, lineaInicio, columnaInicio);
    }

    // Estado S7
    else {
        char delim =c;
        avanzar();
        string lexema="";
        lexema +=delim;

        if (delim=='{')
            return Token(LLAVE_IZQ, lexema, lineaInicio, columnaInicio);

        if (delim=='}') 
            return Token(LLAVE_DER, lexema, lineaInicio, columnaInicio);

        if (delim==':') 
            return Token(DOS_PUNTOS, lexema, lineaInicio, columnaInicio);

        if (delim ==',') 
            return Token(COMA, lexema, lineaInicio, columnaInicio);

        if (delim ==';') 
            return Token(PUNTO_COMA, lexema, lineaInicio, columnaInicio);
        if (delim == '[')
            return Token(CORCHETE_IZQ, "[", lineaInicio, columnaInicio);

        if (delim == ']')
            return Token(CORCHETE_DER, "]", lineaInicio, columnaInicio);

        gestorErrores->agregarErrorLexico(lexema, lineaInicio, columnaInicio, "Caracter no reconocido: '" + lexema + "'");
        return Token(DESCONOCIDO, lexema, lineaInicio, columnaInicio);
    }
}

vector<Token> AnalizadorLexico::analizarTodo() {
    tokens.clear();
    Token tok;
    do {
        tok = siguienteToken();
        if (tok.tipo !=DESCONOCIDO) {
            tokens.push_back(tok);
        }
    }
    while (tok.tipo != FIN_ARCHIVO);
    return tokens;
}

vector<Token> AnalizadorLexico::obtenerTokens() {
    return tokens;
}
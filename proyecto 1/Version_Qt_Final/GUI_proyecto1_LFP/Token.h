#ifndef TOKEN_H
#define TOKEN_H
#include <string>
using namespace std;

enum class TokenType {
    // --- Palabras Reservadas (Secciones y Elementos) ---
    HOSPITAL,
    PACIENTES,      
    MEDICOS,       
    CITAS,          
    DIAGNOSTICOS,   
    PACIENTE,       
    MEDICO,         
    CITA,
    DIAGNOSTICO,
    CON,

    // --- Literales y Valores ---
    CADENA,         
    NUMERO,         
    FECHA,          
    HORA,           
    CODIGO_ID, 
    IDENTIFICADOR,

    // --- Enumeraciones (Especialidades) ---
    CARDIOLOGIA,
    NEUROLOGIA,
    PEDIATRIA,
    CIRUGIA,
    MEDICINA_GENERAL,
    ONCOLOGIA,

    // --- Enumeraciones (Dosis) ---
    DIARIA,
    CADA_8_HORAS,
    CADA_12_HORAS,
    SEMANAL,

    // --- -----  Símbolos ---
    LLAVE_ABRE,         
    LLAVE_CIERRA,       
    CORCHETE_ABRE,      
    CORCHETE_CIERRA,    
    DOS_PUNTOS,         
    COMA,               
    PUNTO_COMA,         

    // --- Especiales ---
    END_OF_FILE,
    ERROR
};

class Token {

public:
    TokenType type;
    string lexema;
    int linea;
    int columna;

    Token(TokenType type, string lexema, int linea, int columna);
};
string tokenTypeToString(TokenType type);
#endif


#include "Token.h"
#include <map>

Token::Token(TokenType type, std::string lexema, int linea, int columna) {
    this->type=type;
    this->lexema=lexema;
    this->linea=linea;
    this->columna=columna;
}

string tokenTypeToString(TokenType type) {

    static map<TokenType, string> nombres={
        // Palabras reservadas
        {TokenType::IDENTIFICADOR, "IDENTIFICADOR"},
        {TokenType::HOSPITAL, "HOSPITAL"},
        {TokenType::PACIENTES, "PACIENTES"},
        {TokenType::MEDICOS, "MEDICOS"},
        {TokenType::CITAS, "CITAS"},
        {TokenType::DIAGNOSTICOS, "DIAGNOSTICOS"},
        {TokenType::PACIENTE, "PACIENTE"},
        {TokenType::MEDICO, "MEDICO"},
        {TokenType::CITA, "CITA"},
        {TokenType::DIAGNOSTICO, "DIAGNOSTICO"},
        {TokenType::CON, "CON"},

        // Literales
        {TokenType::CADENA, "CADENA"},
        {TokenType::NUMERO, "NUMERO"},
        {TokenType::FECHA, "FECHA"},
        {TokenType::HORA, "HORA"},
        {TokenType::CODIGO_ID, "CODIGO_ID"},

        // Enumeraciones
        {TokenType::CARDIOLOGIA, "CARDIOLOGIA"},
        {TokenType::NEUROLOGIA, "NEUROLOGIA"},
        {TokenType::PEDIATRIA, "PEDIATRIA"},
        {TokenType::CIRUGIA, "CIRUGIA"},
        {TokenType::MEDICINA_GENERAL, "MEDICINA_GENERAL"},
        {TokenType::ONCOLOGIA, "ONCOLOGIA"},
        {TokenType::DIARIA, "DIARIA"},
        {TokenType::CADA_8_HORAS, "CADA_8_HORAS"},
        {TokenType::CADA_12_HORAS, "CADA_12_HORAS"},
        {TokenType::SEMANAL, "SEMANAL"},

        // Símbolos
        {TokenType::LLAVE_ABRE, "{"},
        {TokenType::LLAVE_CIERRA, "}"},
        {TokenType::CORCHETE_ABRE, "["},
        {TokenType::CORCHETE_CIERRA, "]"},
        {TokenType::DOS_PUNTOS, ":"},
        {TokenType::COMA, ","},
        {TokenType::PUNTO_COMA, ";"},

        // Especiales
        {TokenType::END_OF_FILE, "EOF"},
        {TokenType::ERROR, "ERROR"}

    };

    auto it=nombres.find(type);
    if (it !=nombres.end()) {
        return it->second;
    }
    return "DESCONOCIDO";
}




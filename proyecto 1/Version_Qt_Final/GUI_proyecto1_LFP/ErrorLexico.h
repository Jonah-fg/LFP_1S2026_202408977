#ifndef ERRORLEXICO_H
#define ERRORLEXICO_H
#include <string>

using namespace std;

    class ErrorLexico{

    public:

        string caracter;
        int linea;
        int columna;
		string descripcion;
		string tipoError;
		string gravedad;

        ErrorLexico(string caracter, int linea, int columna);
    };

#endif
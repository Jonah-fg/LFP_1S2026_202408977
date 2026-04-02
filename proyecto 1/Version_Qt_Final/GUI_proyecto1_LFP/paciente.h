#ifndef PACIENTE_H
#define PACIENTE_H

#include <string>
using namespace std;

class Paciente {
public:
    string nombre;
    int edad;
    string tipoSangre;
    int linea;      

    Paciente(string nombre, int edad, string tipoSangre, int linea){
        this->nombre = nombre;
        this->edad = edad;
        this->tipoSangre = tipoSangre;
        this->linea = linea;
    }
};

#endif


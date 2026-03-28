#ifndef MEDICO_H
#define MEDICO_H

#include <string>
using namespace std;

class Medico{
public:
    string nombre;
    string especialidad;
    string codigo;
    int linea;

    Medico(string nombre, string especialidad, string codigo, int linea) {
        this->nombre =nombre;
        this->especialidad=especialidad;
        this->codigo =codigo;
        this->linea =linea;
    }
};
#endif
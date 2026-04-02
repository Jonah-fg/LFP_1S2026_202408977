#ifndef DIAGNOSTICO_H
#define DIAGNOSTICO_H

#include <string>
using namespace std;

class Diagnostico {
public:
    string nombrePaciente;
    string condicion;
    string medicamento;
    string dosis;
    int linea;

    Diagnostico(string paciente, string condicion, string medicamento, string dosis, int linea) {
        this->nombrePaciente =paciente;
        this->condicion =condicion;
        this->medicamento =medicamento;
        this->dosis=dosis;
        this->linea=linea;
    }
};

#endif


#ifndef CITA_H
#define CITA_H

#include <string>
using namespace std;

class Cita {
public:
    string nombrePaciente;
    string nombreMedico;
    string fecha;
    string hora;
    int linea;

    Cita(string paciente, string medico, string fecha, string hora, int linea) {
        this->nombrePaciente =paciente;
        this->nombreMedico =medico;
        this->fecha=fecha;
        this->hora=hora;
        this->linea=linea;
    }
};
#endif

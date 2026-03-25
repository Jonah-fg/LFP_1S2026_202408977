#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <string>
#include <vector>
#include "Paciente.h"
#include "Medico.h"
#include "Cita.h"
#include "Diagnostico.h"

using namespace std;
class GeneradorReporte {
private:
    vector<Paciente> pacientes;
    vector<Medico> medicos;
    vector<Cita> citas;
    vector<Diagnostico> diagnosticos;
    string escaparParaGraphviz(string texto);
    string nombreHospital;
    

    void generarHTMLReporte1();
    void generarHTMLReporte2();
    void generarHTMLReporte3();
    void generarHTMLReporte4();
    void generarArchivoDot();

    string obtenerDiagnosticoPaciente(string nombrePaciente, string& medicamento, string& dosis);
    int contarCitasPorMedico(string nombreMedico);
    int contarPacientesDistintosPorMedico(string nombreMedico);
    bool ConflictoCita(Cita cita);
    string obtenerNivelCarga(int numCitas);
    string obtenerEstadoCita(Cita cita);
    string obtenerEspecialidadMedico(string nombreMedico);

public:
    GeneradorReporte(vector<Paciente> pacientes, vector<Medico> medicos, vector<Cita> citas, vector<Diagnostico> diagnosticos, string nombreHospital);
    void generarTodosReportes();
};
#endif


#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <string>
#include <vector>
#include "ErrorLexico.h"
#include "Paciente.h"
#include "Medico.h"
#include "Cita.h"
#include "Diagnostico.h"
#include <ctime>

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

    string obtenerDiagnosticoPaciente(string nombrePaciente, string& medicamento, string& dosis);
    int contarCitasPorMedico(string nombreMedico);
    int contarPacientesDistintosPorMedico(string nombreMedico);
    bool ConflictoCita(Cita cita);
    string obtenerNivelCarga(int numCitas);
    string obtenerEstadoCita(Cita cita);
    string obtenerEspecialidadMedico(string nombreMedico);

    int contarCitasConConflicto();
    int contarPacientesConDiagnostico();
    string obtenerMedicamentoMasFrecuente();
    string obtenerEspecialidadMayorCarga(int& maxCitas, string& medicoNombre);
    float calcularPromedioEdad();
    bool esFechaFutura(const string& fecha);

    struct EstadisticaEspecialidad {
        string nombre;
        int numMedicos;
        int numCitas;
        int numPacientes;
        float porcentaje;
    };
    vector<EstadisticaEspecialidad> obtenerEstadisticasPorEspecialidad();

public:
    GeneradorReporte(vector<Paciente> pacientes, vector<Medico> medicos, vector<Cita> citas, vector<Diagnostico> diagnosticos, string nombreHospital);
    void generarTodosReportes(const vector<ErrorLexico>& errores=vector<ErrorLexico>());
    void generarArchivoDot();
    void convertirDotAPNG();
    void generarHTMLErrores(const vector<ErrorLexico>& errores);
};
#endif


#include "GeneradorReporte.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
using namespace std;

GeneradorReporte::GeneradorReporte(vector<Paciente> pacientes, vector<Medico> medicos, vector<Cita> citas, vector<Diagnostico> diagnosticos, string nombreHospital) {
    this->pacientes=pacientes;
    this->medicos =medicos;
    this->citas =citas;
    this->diagnosticos=diagnosticos;
    this->nombreHospital =nombreHospital;
}

bool GeneradorReporte::esFechaFutura(const string& fecha) {
    time_t t=time(nullptr);
    tm* ahora = localtime(&t);

    int anioActual=ahora->tm_year+1900;
    int mesActual =ahora->tm_mon +1;
    int diaActual=ahora->tm_mday;

    int anioCita;
    int mesCita;
    int diaCita;
    char guion1;
    char guion2;

    istringstream ss(fecha);
    ss >> anioCita >> guion1 >> mesCita >> guion2 >> diaCita;

    if (anioCita>anioActual) {
        return true;
    }
    else if (anioCita<anioActual) {
        return false;
    }
    else {
        if (mesCita> mesActual) {
            return true;
        }
        else if (mesCita<mesActual) {
            return false;
        }
        else {
            return (diaCita>diaActual);
        }
    }
}

string GeneradorReporte::escaparParaGraphviz(string texto) {
    string resultado;
    for (char c :texto) {
        switch (c) {
        case '\\':
            resultado+= "\\\\"; break;
        case '"':
            resultado+="\\\""; break;
        case '\n':
            resultado +="\\n"; break;
        case '\r':
            break;
        default:
            resultado +=c; break;
        }
    }
    return resultado;
}

int GeneradorReporte::contarCitasConConflicto() {
    int conflictos=0;
    for (int i =0; i<citas.size(); i++) {
        for (int j=i + 1; j<citas.size(); j++) {
            if (citas[i].nombreMedico== citas[j].nombreMedico && citas[i].fecha== citas[j].fecha && citas[i].hora == citas[j].hora) {
                conflictos++;
                break; 
            }
        }
    }
    return conflictos;
}


int GeneradorReporte::contarPacientesConDiagnostico() {
    int contador=0;
    for (int i=0; i<pacientes.size(); i++) {
        for (int j= 0; j <diagnosticos.size(); j++) {
            if (diagnosticos[j].nombrePaciente== pacientes[i].nombre) {
                contador++;
                break;
            }
        }
    } 
    return contador;
}

string GeneradorReporte::obtenerMedicamentoMasFrecuente() {
    if (diagnosticos.empty()) {
        return "Ninguno";
    }
    map<string, int> frecuenciaMedicamentos;

    for (int i=0; i<diagnosticos.size(); i++) {
        frecuenciaMedicamentos[diagnosticos[i].medicamento]++;
    }
    string medicamentoMasFrecuente;
    int maxFrecuencia= 0;

    for (map<string, int>::iterator i =frecuenciaMedicamentos.begin(); i != frecuenciaMedicamentos.end(); ++i) {
        if (i->second> maxFrecuencia) {
            maxFrecuencia =i->second;
            medicamentoMasFrecuente=i->first;
        }
    }
    return medicamentoMasFrecuente +"(" + to_string(maxFrecuencia) + "pacientes)";
}


string GeneradorReporte::obtenerEspecialidadMayorCarga(int& maxCitas, string& medicoNombre) {
    map<string, int> citasPorEspecialidad;

    for (int i = 0; i<medicos.size(); i++) {
        citasPorEspecialidad[medicos[i].especialidad]= 0;
    }

    for (int i=0; i <citas.size(); i++) {
        string especialidad= obtenerEspecialidadMedico(citas[i].nombreMedico);
        citasPorEspecialidad[especialidad]++;
    }
    string especialidadMax;
    maxCitas=0;
    for (map<string, int>::iterator i=citasPorEspecialidad.begin(); i != citasPorEspecialidad.end(); ++i) {
        if (i->second> maxCitas) {
            maxCitas =i->second;
            especialidadMax= i->first;
        }
    }
    for (int i =0; i<medicos.size(); i++) {
        if (medicos[i].especialidad==especialidadMax) {
            medicoNombre= medicos[i].nombre;
            break;
        }
    }
    return especialidadMax;
}

float GeneradorReporte::calcularPromedioEdad() {
    if (pacientes.empty()){
        return 0.0;
    }

    int sumaEdades= 0;
    for (int i=0; i<pacientes.size(); i++) {
        sumaEdades+= pacientes[i].edad;
    }
    return (float)sumaEdades/ pacientes.size();
}

vector<GeneradorReporte::EstadisticaEspecialidad>
GeneradorReporte::obtenerEstadisticasPorEspecialidad() {
    vector<EstadisticaEspecialidad> estadisticas;

    map<string, vector<Medico>> medicosPorEspecialidad;
    for (int i =0; i<medicos.size(); i++) {
        medicosPorEspecialidad[medicos[i].especialidad].push_back(medicos[i]);
    }

    for (map<string, vector<Medico>>::iterator i =medicosPorEspecialidad.begin(); i !=medicosPorEspecialidad.end(); ++i) {
        EstadisticaEspecialidad est;
        est.nombre = i->first;
        est.numMedicos = i->second.size();

        est.numCitas =0;
        for (int i =0; i< citas.size(); i++) {
            string especialidadMedico=obtenerEspecialidadMedico(citas[i].nombreMedico);
            if (especialidadMedico == est.nombre){
                est.numCitas++;
            }
        }

 //pacientes distintos atendidos por médicos de esta especialidad
        vector<string> pacientesAtendidos;
        for (int i = 0; i <citas.size(); i++) {
            string especialidadMedico = obtenerEspecialidadMedico(citas[i].nombreMedico);
            if (especialidadMedico == est.nombre) {
                bool yaExiste = false;
                for (int j =0; j<pacientesAtendidos.size(); j++) {
                    if (pacientesAtendidos[j]== citas[i].nombrePaciente) {
                        yaExiste =true;
                        break;
                    }
                }
                if (!yaExiste) {
                    pacientesAtendidos.push_back(citas[i].nombrePaciente);
                }
            }
        }
        est.numPacientes =pacientesAtendidos.size();

        if (citas.size()>0) {
            est.porcentaje =(est.numCitas * 100.0) / citas.size();
        }
        else {
            est.porcentaje = 0;
        }
        estadisticas.push_back(est);
    }
    return estadisticas;
}


string GeneradorReporte::obtenerDiagnosticoPaciente(string nombrePaciente, string& medicamento, string& dosis) {
    for (size_t i = 0; i<diagnosticos.size(); i++) {
        if (diagnosticos[i].nombrePaciente==nombrePaciente) {
            medicamento=diagnosticos[i].medicamento;
            dosis=diagnosticos[i].dosis;
            return diagnosticos[i].condicion;
        }
    }
    medicamento ="";
    dosis ="";
    return "";
}


int GeneradorReporte::contarCitasPorMedico(string nombreMedico) {
    int contador=0;
    for (size_t i=0; i<citas.size(); i++) {
        if (citas[i].nombreMedico==nombreMedico) {
            contador++;
        }
    }
    return contador;
}

int GeneradorReporte::contarPacientesDistintosPorMedico(string nombreMedico) {
    vector<string> pacientesAtendidos;

    for (size_t i =0; i<citas.size(); i++){
        if (citas[i].nombreMedico==nombreMedico){
            bool yaExiste=false;
            for (size_t j=0; j<pacientesAtendidos.size(); j++) {
                if (pacientesAtendidos[j]==citas[i].nombrePaciente) {
                    yaExiste= true;
                    break;
                }
            }
            if (!yaExiste){
                pacientesAtendidos.push_back(citas[i].nombrePaciente);
            }
        }
    }
    return pacientesAtendidos.size();
}

bool GeneradorReporte::ConflictoCita(Cita cita) {
    for (size_t i=0; i<citas.size(); i++){
        if (cita.linea==citas[i].linea &&
            cita.nombrePaciente==citas[i].nombrePaciente) {
            continue;
        }
        // Verificar conflicto
        if (cita.nombreMedico ==citas[i].nombreMedico && cita.fecha== citas[i].fecha && cita.hora==citas[i].hora) {
            return true;
        }
    }
    return false;
}

string GeneradorReporte::obtenerNivelCarga(int numCitas) {
    if (numCitas >= 1 && numCitas <=3) {
        return "BAJA";
    }
    else if (numCitas >=4 && numCitas<=6) {
        return "NORMAL";
    }
    else if (numCitas >=7 && numCitas<=8) {
        return "ALTA";
    }
    else if (numCitas>=9) {
        return "SATURADA";
    }
    return "SIN CITAS";
}

string GeneradorReporte::obtenerEspecialidadMedico(string nombreMedico) {
    for (int i=0; i <medicos.size(); i++){
        if (medicos[i].nombre ==nombreMedico) {
            return medicos[i].especialidad;
        }
    }
    return "DESCONOCIDA";
}

string GeneradorReporte::obtenerEstadoCita(Cita cita) {
    if (ConflictoCita(cita)) {
        return "CONFLICTO";
    }
    if (esFechaFutura(cita.fecha)) {
        return "PENDIENTE";
    }
    return "CONFIRMADA";
}


void GeneradorReporte::generarHTMLReporte1() {
    ofstream archivo("reporte1_pacientes.html");

    archivo << "<!DOCTYPE html>\n";
    archivo << "<html>\n";
    archivo << "<head>\n";
    archivo << "    <meta charset=\"UTF-8\">\n";
    archivo << "    <title>Reporte 1 - Historial de Pacientes</title>\n";
    archivo << "    <style>\n";
    archivo << "        body { font-family: Arial; margin: 20px; }\n";
    archivo << "        h1 { color: #2c3e50; }\n";
    archivo << "        table { border-collapse: collapse; width: 100%; }\n";
    archivo << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    archivo << "        th { background-color: #2c3e50; color: white; }\n";
    archivo << "        .activo { background-color: #d4edda; }\n";
    archivo << "        .sin-diag { background-color: #fff3cd; }\n";
    archivo << "    </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "    <h1>Historial de Pacientes</h1>\n";
    archivo << "     <table>\n";
    archivo << "         <tr>\n";
    archivo << "             <th>Paciente</th>\n";
    archivo << "             <th>Edad</th>\n";
    archivo << "             <th>Sangre</th>\n";
    archivo << "             <th>Diagnóstico</th>\n";
    archivo << "             <th>Medicamento / Dosis</th>\n";
    archivo << "             <th>Estado</th>\n";
    archivo << "         </tr>\n";


    for (int i=0; i<pacientes.size(); i++) {
        Paciente pac =pacientes[i];
        string medicamento;
        string dosis;
        string diagnostico=obtenerDiagnosticoPaciente(pac.nombre, medicamento, dosis);

        string estado;
        string clase;
        string medicamentoDosis = "—";

        if (diagnostico =="") {
            estado ="SIN DIAG.";
            clase="sin-diag";
        }
        else {
            medicamentoDosis=medicamento +" / " + dosis;
            estado ="ACTIVO";
            clase="activo";
        }

        archivo << "         <tr class=\"" << clase << "\">\n";
        archivo << "             <td>" << pac.nombre << "</td>\n";
        archivo << "             <td>" << pac.edad << "</td>\n";
        archivo << "             <td>" << pac.tipoSangre << "</td>\n";
        archivo << "             <td>" << (diagnostico == "" ? "—" : diagnostico) << "</td>\n";
        archivo << "             <td>" << medicamentoDosis << "</td>\n";
        archivo << "             <td>" << estado << "</td>\n";
        archivo << "         </tr>\n";
    }
    archivo << "     </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";
    archivo.close();
    cout << " Reporte 1: reporte1_pacients.html"<< endl;
}


//-----------------------
void GeneradorReporte::generarHTMLReporte2() {
    ofstream archivo("reporte2_medicos.html"); 
    archivo << "<!DOCTYPE html>\n";
    archivo << "<html>\n";
    archivo << "<head>\n";
    archivo << "    <meta charset=\"UTF-8\">\n";
    archivo << "    <title>Reporte 2 - Carga de Médicos</title>\n";
    archivo << "    <style>\n";
    archivo << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    archivo << "        h1 { color: #2c3e50; text-align: center; }\n";
    archivo << "        table { border-collapse: collapse; width: 100%; margin-top: 20px; }\n";
    archivo << "        th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }\n";
    archivo << "        th { background-color: #2c3e50; color: white; }\n";
    archivo << "        .baja { background-color: #cce5ff; }\n";
    archivo << "        .normal { background-color: #d4edda; }\n";
    archivo << "        .alta { background-color: #fff3cd; }\n";
    archivo << "        .saturada { background-color: #f8d7da; }\n";
    archivo << "    </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";

    archivo << "    <h1> Carga de Médicos por Especialidad</h1>\n";
    archivo << "    <p>Niveles: BAJA (azul), NORMAL (verde), ALTA (naranja), SATURADA (rojo)</p>\n";

    archivo << "     <table border=\"1\">\n";
    archivo << "         <tr>\n";
    archivo << "             <th>Médico</th>\n";
    archivo << "             <th>Código</th>\n";
    archivo << "             <th>Especialidad</th>\n";
    archivo << "             <th>Citas Programadas</th>\n";
    archivo << "             <th>Pacientes Atendidos</th>\n";
    archivo << "             <th>Nivel de Carga</th>\n";
    archivo << "         </tr>\n";

    for (int i=0; i<medicos.size(); i++) {
        Medico med =medicos[i];
        int numCitas=contarCitasPorMedico(med.nombre);
        int numPacientes =contarPacientesDistintosPorMedico(med.nombre);
        string nivel=obtenerNivelCarga(numCitas);

        string clase;
        if (nivel =="BAJA") {
            clase ="baja";
        }
        else if (nivel =="NORMAL") {
            clase = "normal";
        }
        else if (nivel=="ALTA") {
            clase = "alta";
        }
        else if (nivel== "SATURADA") {
            clase ="saturada";
        }
        else {
            clase="";
        }

        archivo << "         <tr class=\"" << clase << "\">\n";
        archivo << "              <td>" << med.nombre << "</td>\n";
        archivo << "              <td>" << med.codigo << "</td>\n";
        archivo << "              <td>" << med.especialidad << "</td>\n";
        archivo << "              <td>" << numCitas << "</td>\n";
        archivo << "              <td>" << numPacientes << "</td>\n";
        archivo << "              <td>" << nivel << "</td>\n";
        archivo << "          </tr>\n";
    }
    archivo << "      </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << " Reporte 2: reporte2_medicos.html" << endl;
} 


//-------------
void GeneradorReporte::generarHTMLReporte3() {
    ofstream archivo("reporte3_citas.html");

    archivo << "<!DOCTYPE html>\n";
    archivo << "<html>\n";
    archivo << "<head>\n";
    archivo << "    <meta charset=\"UTF-8\">\n";
    archivo << "    <title>Reporte 3 - Agenda de Citas</title>\n";
    archivo << "    <style>\n";
    archivo << "       body { font-family: Arial; margin: 20px; }\n";
    archivo << "        h1 { color: #2c3e50; }\n";
    archivo << "        table { border-collapse: collapse; width: 100%; }\n";
    archivo << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    archivo << "        th { background-color: #2c3e50; color: white; }\n";
    archivo << "        .confirmada { background-color: #d4edda; }\n";
    archivo << "        .conflicto { background-color: #f8d7da; }\n";
    archivo << "        .pendiente { background-color: #fff3cd; }\n";
    archivo << "    </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";
    archivo << "    <h1>Agenda de Citas</h1>\n";
    archivo << "    <p>Las filas en ROJO indican CONFLICTO de horario.</p>\n";
    archivo << "     <table>\n";
    archivo << "         <tr>\n";
    archivo << "             <th>Fecha</th>\n";
    archivo << "             <th>Hora</th>\n";
    archivo << "             <th>Paciente</th>\n";
    archivo << "             <th>Médico</th>\n";
    archivo << "             <th>Especialidad</th>\n";
    archivo << "             <th>Estado</th>\n";
    archivo << "         </tr>\n";

    for (int i=0; i<citas.size(); i++) {
        Cita c=citas[i];
        string especialidad=obtenerEspecialidadMedico(c.nombreMedico);
        string estado=obtenerEstadoCita(c);
        string clase=(estado =="CONFLICTO") ? "conflicto" : "confirmada";

        archivo << "         <tr class=\"" << clase << "\">\n";
        archivo << "             <td>" << c.fecha << "</td>\n";
        archivo << "             <td>" << c.hora << "</td>\n";
        archivo << "             <td>" << c.nombrePaciente << "</td>\n";
        archivo << "             <td>"<< c.nombreMedico << "</td>\n";
        archivo << "             <td>" << especialidad << "</td>\n";
        archivo << "             <td>" << estado << "</td>\n";
        archivo << "         </tr>\n";
    }
    archivo << "     </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << " Reporte 3: reporte3_citas.html" << endl;
}


//---------------------
void GeneradorReporte::generarHTMLReporte4() {

    ofstream archivo("reporte4_estadistico.html");
    if (!archivo.is_open()) {
        cout << "Error: No se pudo crear reporte4_estadistico.html" << endl;
        return;
    }

// seccion KPIs
    int totalPacientes= pacientes.size();
    int totalMedicos=medicos.size();
    int totalCitas =citas.size();
    int citasConflicto =contarCitasConConflicto();
    int pacientesConDiagnostico=contarPacientesConDiagnostico();
    float porcentajeDiagnostico =(totalPacientes > 0) ? (pacientesConDiagnostico * 100.0) / totalPacientes : 0;
    string medicamentoFrecuente=obtenerMedicamentoMasFrecuente();
    
    int maxCitasEspecialidad;
    string medicoEjemplo;
    string especialidadMayorCarga =obtenerEspecialidadMayorCarga(maxCitasEspecialidad, medicoEjemplo);
    
    float promedioEdad = calcularPromedioEdad();
    vector<EstadisticaEspecialidad> estadisticas=obtenerEstadisticasPorEspecialidad();
    
    archivo << "<!DOCTYPE html>\n";
    archivo << "<html>\n";
    archivo << "<head>\n";
    archivo << "    <meta charset=\"UTF-8\">\n";
    archivo << "    <title>Reporte 4 - Estadístico General del Hospital</title>\n";
    archivo << "    <style>\n";
    archivo << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    archivo << "        h1 { color: #2c3e50; text-align: center; }\n";
    archivo << "        h2 { color: #34495e; margin-top: 30px; }\n";
    archivo << "        table { border-collapse: collapse; width: 100%; margin-top: 20px; }\n";
    archivo << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    archivo << "        th { background-color: #2c3e50; color: white; }\n";
    archivo << "        .barra-progreso { background-color: #ecf0f1; height: 20px; width: 100%; }\n";
    archivo << "        .barra { background-color: #27ae60; height: 20px; color: white; text-align: center; line-height: 20px; }\n";
    archivo << "        .barra-saturada { background-color: #e74c3c; }\n";
    archivo << "        tr.alerta { background-color: #f8d7da; }\n";
    archivo << "    </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";

    archivo << "    <h1>Estadístico General del Hospital</h1>\n";

    archivo << "    <h2>Sección A — Indicadores clave del hospital</h2>\n";
    archivo << "     <table border=\"1\">\n";
    archivo << "         <thead>\n";
    archivo << "             <tr>\n";
    archivo << "                 <th>Indicador</th>\n";
    archivo << "                 <th>Valor</th>\n";
    archivo << "             </tr>\n";
    archivo << "         </thead>\n";
    archivo << "         <tbody>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Nombre del hospital</td>\n";
    archivo << "                 <td>" << nombreHospital << "</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Total de pacientes registrados</td>\n";
    archivo << "                 <td>" << totalPacientes << "</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Total de médicos activos</td>\n";
    archivo << "                 <td>" << totalMedicos << "</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Total de citas programadas</td>\n";
    archivo << "                 <td>" << totalCitas << "</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Citas con conflicto de horario</td>\n";
    archivo << "                 <td>" << citasConflicto << "</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Pacientes con diagnóstico activo</td>\n";
    archivo << "                 <td>" << pacientesConDiagnostico << " de " << totalPacientes<< "("<< (totalPacientes > 0 ? (int)(pacientesConDiagnostico * 100.0 / totalPacientes) : 0) << "%)</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Medicamento más prescrito</td>\n";
    archivo << "                 <td>" << medicamentoFrecuente << "</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Especialidad con mayor carga de citas</td>\n";
    archivo << "                 <td>" << especialidadMayorCarga << " — " << medicoEjemplo<< " (" << maxCitasEspecialidad << " citas)</td>\n";
    archivo << "             </tr>\n";

    archivo << "             <tr>\n";
    archivo << "                 <td>Promedio de edad de los pacientes</td>\n";
    archivo << "                 <td>" << promedioEdad << " años</td>\n";
    archivo << "             </tr>\n";

    archivo << "         </tbody>\n";
    archivo << "     </table>\n";
    archivo << "    <h2>Sección B — Distribución de carga por especialidad</h2>\n";
    archivo << "     <table border=\"1\">\n";
    archivo << "         <thead>\n";
    archivo << "             <tr>\n";
    archivo << "                 <th>Especialidad</th>\n";
    archivo << "                 <th>Médicos</th>\n";
    archivo << "                 <th>Citas</th>\n";
    archivo << "                 <th>Pacientes</th>\n";
    archivo << "                 <th>Barra de ocupación</th>\n";
    archivo << "             </tr>\n";
    archivo << "         </thead>\n";
    archivo << "         <tbody>\n";
    
    for (int i =0; i < estadisticas.size(); i++) {
        EstadisticaEspecialidad estadisticaEspe = estadisticas[i]; 
        bool saturada=(estadisticaEspe.porcentaje > 80);

        string claseBarra ="barra";
        if (saturada){
            claseBarra ="barra barra-saturada";
        }
        archivo << "             <tr" << (saturada ? " class=\"alerta\"" : "") << ">\n";
        archivo << "                 <td>" << estadisticaEspe.nombre << "</td>\n";
        archivo << "                 <td>" << estadisticaEspe.numMedicos << "</td>\n";
        archivo << "                 <td>" << estadisticaEspe.numCitas <<"</td>\n";
        archivo << "                 <td>" << estadisticaEspe.numPacientes << "</td>\n";
        archivo << "                 <td style=\"width: 200px;\">\n";
        archivo << "                     <div class=\"barra-progreso\">\n";
        archivo << "                         <div class=\"" << claseBarra<< "\" style=\"width: " << estadisticaEspe.porcentaje << "%;\">" << (int)estadisticaEspe.porcentaje << "%</div>\n";
        archivo << "                     </div>\n";
        archivo << "                 </td>\n";
        archivo << "             </tr>\n";
    }
    archivo << "         </tbody>\n";
    archivo << "     </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";

    archivo.close();
    cout << " Reporte 4: reporte4_estadistico.html" << endl;
}



void GeneradorReporte::generarArchivoDot() {
    ofstream archivo("hospital.dot");
    archivo << "digraph Hospital {\n";
    archivo << "    graph [charset=\"UTF-8\"];\n";
    archivo << "    rankdir=TB;\n";
    archivo << "    node [shape=box, style=filled, fontname=\"Arial\"];\n\n";

    archivo << "    H [label=\"" << escaparParaGraphviz(nombreHospital)<< "\", fillcolor=\"#1A4731\", fontcolor=white, shape=ellipse];\n\n";

    archivo << "    P [label=\"PACIENTES\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    archivo << "    M [label=\"MEDICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    archivo << "    C [label=\"CITAS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    archivo << "    D [label=\"DIAGNOSTICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n\n";
    archivo << "    H -> P;H -> M; H -> C; H -> D;\n";

    for (int i=0; i<pacientes.size(); i++) {
        archivo << "    p" << i << " [label=\"" << escaparParaGraphviz(pacientes[i].nombre) << "\\n" << pacientes[i].edad << " anios\\n" << escaparParaGraphviz(pacientes[i].tipoSangre) << "\", fillcolor=\"#D4EDDA\"];\n";
        archivo << "    P -> p" << i << ";\n";
    }
    archivo << "\n";

    for (int i =0; i<medicos.size(); i++) {
        archivo << "   m" <<i<< " [label=\"" << escaparParaGraphviz(medicos[i].nombre) << "\\n" << escaparParaGraphviz(medicos[i].codigo) << "\\n" << escaparParaGraphviz(medicos[i].especialidad) << "\", fillcolor=\"#D6EAF8\"];\n";
        archivo << "    M -> m" << i << ";\n";
    }
    archivo << "\n";

    //citas
    for (int i =0; i<citas.size(); i++) {
        int idxPacienteCita=-1;
        for (int j= 0; j<pacientes.size(); j++) {
            if(pacientes[j].nombre== citas[i].nombrePaciente) {
                idxPacienteCita =j;
                break;
            }
        }
        int idxMedicoCita= -1;
        for (int j =0; j<medicos.size(); j++) {
            if (medicos[j].nombre== citas[i].nombreMedico) {
                idxMedicoCita =j;
                break;
            }
        }
        if (idxPacienteCita!= -1 && idxMedicoCita!=-1) {
            archivo<< "   p" << idxPacienteCita << "-> m" << idxMedicoCita <<" [label=\"" << escaparParaGraphviz(citas[i].fecha) << " " << escaparParaGraphviz(citas[i].hora) << "\", color=\"#E67E22\", style=dashed];\n";
        }
    }
    archivo<<"\n";

    for (int i=0; i< diagnosticos.size(); i++) {
        int idxPacienteDiag= -1;
        for (int j =0; j<pacientes.size(); j++) {
            if (pacientes[j].nombre== diagnosticos[i].nombrePaciente) {
                idxPacienteDiag=j;
                break;
            }
        }
        archivo<< "   d" <<i << " [label=\"" << escaparParaGraphviz(diagnosticos[i].condicion) << "\\n" << escaparParaGraphviz(diagnosticos[i].medicamento) << "\\n" << escaparParaGraphviz(diagnosticos[i].dosis) << "\", fillcolor=\"#FDEBD0\"];\n";
        archivo << "   D -> d" <<i<< ";\n";

        if (idxPacienteDiag !=-1) {
            archivo << "    d" <<i<< " -> p" <<idxPacienteDiag << " [label=\"diagnostico activo\", color=\"#C0392B\"];\n";
        }
    }
    archivo << "}\n";
    archivo.close();
    cout << "   Archivo Graphviz geneado: hospital.dot" << endl;
    cout << "   Para viualzar: dot -Tpng hospital.dot -o hospital.png" << endl;
}


void GeneradorReporte::generarHTMLErrores(const vector<ErrorLexico>& errores) {
    ofstream archivo("errores.html");

    archivo << "<!DOCTYPE html>\n";
    archivo << "<html>\n";
    archivo << "<head>\n";
    archivo << "    <meta charset=\"UTF-8\">\n";
    archivo << "    <title>Reporte de Errores Léxicos</title>\n";
    archivo << "    <style>\n";
    archivo << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    archivo << "        h1 { color: #c0392b; text-align: center; }\n";
    archivo << "        table { border-collapse: collapse; width: 100%; margin-top: 20px; }\n";
    archivo << "        th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }\n";
    archivo << "        th { background-color: #c0392b; color: white; }\n";
    archivo << "        tr:nth-child(even) { background-color: #f2f2f2; }\n";
    archivo << "        .critico { background-color: #f8d7da; }\n";
    archivo << "        .error { background-color: #fff3cd; }\n";
    archivo << "    </style>\n";
    archivo << "</head>\n";
    archivo << "<body>\n";

    archivo << "    <h1>Reporte de Errores Léxicos</h1>\n";
    archivo << "    <p>Total de errores encontrados: " << errores.size() << "</p>\n";
    archivo << "    <table>\n";
    archivo << "        <tr>\n";
    archivo << "            <th>No.</th>\n";
    archivo << "            <th>Lexema</th>\n";
    archivo << "            <th>Tipo de Error</th>\n";
    archivo << "            <th>Descripción</th>\n";
    archivo << "            <th>Línea</th>\n";
    archivo << "            <th>Columna</th>\n";
    archivo << "            <th>Gravedad</th>\n";
    archivo << "        </tr>\n";

    for (int i=0; i <errores.size(); i++) {
        const ErrorLexico& e=errores[i];
        string clase=(e.gravedad == "CRÍTICO") ? "critico" : "error";

        archivo << "         <tr class=\"" << clase << "\">\n";
        archivo << "             <td>" << (i + 1) << "</td>\n";
        archivo << "             <td>" << escaparParaGraphviz(e.caracter) << "</td>\n";
        archivo << "             <td>" << e.tipoError << "</td>\n";
        archivo << "             <td>" << e.descripcion << "</td>\n";
        archivo << "             <td>" << e.linea << "</td>\n";
        archivo << "             <td>" << e.columna << "</td>\n";
        archivo << "             <td>" << e.gravedad << "</td>\n";
        archivo << "         </tr>\n";
    }
    archivo << "     </table>\n";
    archivo << "</body>\n";
    archivo << "</html>\n";
    archivo.close();
    cout << " Reporte de errores: errores.html" << endl;
}


void GeneradorReporte::convertirDotAPNG(){

    string comando = "dot -Tpng hospital.dot -o hospital.png";
    cout << "Convirtiendo diagrama a PNG..." << endl;

    int resultado= system(comando.c_str());
    if (resultado ==0) {
        cout << "Diagrama convertido exitosamente: hospital.png" << endl;
    }
    else {
        cout << " Error al convertir el diagrama. Asegurate de tener Graphviz instalado." << endl;
        cout << " Para instalar Graphviz:" << endl;
        cout << "   - Windows: https://graphviz.org/download/" << endl;
        cout << "   - Linux: sudo apt install graphviz" << endl;
        cout << "   - macOS: brew install graphviz" << endl;
    }
}


void GeneradorReporte::generarTodosReportes(const vector<ErrorLexico>& errores) {
    cout << "\n--- GENERANDO REPORTES ---" << endl;

    generarHTMLReporte1();
    generarHTMLReporte2();
    generarHTMLReporte3();
    generarHTMLReporte4();
    generarArchivoDot();
    convertirDotAPNG();
    if (!errores.empty()){
        generarHTMLErrores(errores);
    }
    cout << "Todos los reportes generados exitosamente" << endl;
}


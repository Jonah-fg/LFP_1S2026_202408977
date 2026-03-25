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
    for (size_t i=0; i<citas.size(); i++) {
        if (cita.linea ==citas[i].linea &&
            cita.nombrePaciente == citas[i].nombrePaciente) {
            continue;
        }
        // Verificar conflicto
        if (cita.nombreMedico == citas[i].nombreMedico && cita.fecha == citas[i].fecha && cita.hora == citas[i].hora) {
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
        string diagnostico =obtenerDiagnosticoPaciente(pac.nombre, medicamento, dosis);

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
    cout << " Reporte 1: reporte1_pacientes.html"<< endl;
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
    archivo << "        tr:nth-child(even) { background-color: #f2f2f2; }\n";
    archivo << "        .baja{ background-color: #cce5ff; }\n";
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
        if (nivel=="BAJA")
            clase ="baja";

        else if (nivel == "NORMAL")
            clase ="normal";

        else if (nivel =="ALTA")
            clase ="alta";

        else clase="saturada";

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
        archivo << "             <td>" << c.nombreMedico << "</td>\n";
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
            archivo << "    d" << i << " -> p" <<idxPacienteDiag << " [label=\"diagnostico activo\", color=\"#C0392B\"];\n";
        }
    }
    archivo << "}\n";
    archivo.close();
    cout << "   Archivo Graphviz geneado: hospital.dot" << endl;
    cout << "   Para viualizar: dot -Tpng hospital.dot -o hospital.png" << endl;
}


void GeneradorReporte::generarTodosReportes() {
    cout << "\n--- GENERANDO REPORTES ---" << endl;

    generarHTMLReporte1();
    generarHTMLReporte2();
    generarHTMLReporte3();
    generarArchivoDot();
    cout << "Todos los reportes generados exitosamente" << endl;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "AnalizadorLexico.h"
#include "Parser.h"
#include "Token.h"

using namespace std;

string leerArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return "";
    }

    stringstream buffer;
    buffer <<archivo.rdbuf();
    return buffer.str();
}

void mostrarTokens(const vector<Token>& tokens) {
    cout << "\n=== TOKENS ENCONTRADOS ===\n";
    cout << "No.\tTipo\t\t\tLexema\t\tLínea\tColumna\n";
    cout << "--------------------------------------------------------\n";

    for (size_t i =0; i<tokens.size(); i++){
        cout << i + 1 << "\t";
        cout << tokenTypeToString(tokens[i].type) << "\t\t";
        cout << tokens[i].lexema << "\t\t";
        cout << tokens[i].linea << "\t";
        cout << tokens[i].columna << "\n";
    }
}


int main() {
    string nombreArchivo ="hospital_completo.med";

    cout << "==========================================\n";
    cout << "   ANALIZADOR LÉXICO Y SINTÁCTICO\n";
    cout << "==========================================\n";
    cout << "Archivo: " << nombreArchivo << "\n\n";

    
    string contenido=leerArchivo(nombreArchivo);
    if (contenido.empty()){
        cout << "No se pudo leer el archivo. Verifica que existe.\n";
        cout << " Crea el archivo " << nombreArchivo << " en la misma carpeta.\n";
        return 1;
    }

    cout << " ANALIZANDO LEXICAMENTE...\n";
    AnalizadorLexico lexer(contenido);
    vector<Token> tokens;

    Token tok=lexer.nextToken();
    while (tok.type !=TokenType::END_OF_FILE){
        tokens.push_back(tok);
        tok=lexer.nextToken();
    }
    cout << " Total tokens encontrados: " << tokens.size() << "\n";


    if (!lexer.errores.empty()) {
        cout << "\n ERRORES LÉXICOS (" << lexer.errores.size() << "):\n";
        for (const auto& error: lexer.errores){
            cout << "   Línea " << error.linea << ", Columna " << error.columna << ": " << error.descripcion << " ('" << error.caracter << "')\n";
        }
    }
    else {
        cout << "No hay errores léxicos\n";
    }

    cout << "\n ANALIZANDO SINTACTICAMENTE...\n";
    Parser parser(tokens);

    if (parser.parsearHospital()){
        cout << "\n ANÁLISIS SINTÁCTICO EXITOSO\n";

        cout << "\n==========================================\n";
        cout << "   DATOS EXTRAÍDOS DEL ARCHIVO\n";
        cout << "==========================================\n";

        // Pacientes
        cout << "\n PACIENTES (" << parser.pacientes.size() << "):\n";
        for (size_t i = 0; i < parser.pacientes.size(); i++) {
            Paciente pac =parser.pacientes[i];
            cout << "   • " << pac.nombre << " | Edad: " << pac.edad << " | Sangre: " << pac.tipoSangre << " (línea " << pac.linea << ")\n";
        }

        // Médicos
        cout << "\n MÉDICOS (" << parser.medicos.size() << "):\n";
        for (size_t i = 0; i < parser.medicos.size(); i++) {
            Medico med = parser.medicos[i];
            cout << "   • " << med.nombre << " | " << med.especialidad << " | Código: " << med.codigo << " (línea " << med.linea << ")\n";
        }

        // Citas
        cout << "\n CITAS (" << parser.citas.size() << "):\n";
        for (size_t i = 0; i<parser.citas.size(); i++) {
            Cita c =parser.citas[i];
            cout << "   • " << c.nombrePaciente << " → " << c.nombreMedico << " | " << c.fecha << " " << c.hora << "\n";
        }

        // Diagnósticos
        cout << "\n DIAGNÓSTICOS (" << parser.diagnosticos.size() << "):\n";
        for (size_t i = 0; i<parser.diagnosticos.size(); i++) {
            Diagnostico diag =parser.diagnosticos[i];
            cout << "   • " << diag.nombrePaciente << " | " << diag.condicion << " | " << diag.medicamento << " (" << diag.dosis << ")\n";
        }

        cout << "\n==========================================\n";
        cout << "               ESTADiSTICAS\n";
        cout << "==========================================\n";
        cout << "Total pacientes: " << parser.pacientes.size() << "\n";
        cout << "Total médicos: " << parser.medicos.size() << "\n";
        cout << "Total citas: " << parser.citas.size() << "\n";
        cout << "Total diagnósticos: " << parser.diagnosticos.size() << "\n";

        if (parser.pacientes.size()>0) {
            int sumaEdades=0;
            for (size_t i =0; i<parser.pacientes.size(); i++) {
                sumaEdades= sumaEdades+parser.pacientes[i].edad;
            }
            float promedio=(float)sumaEdades / parser.pacientes.size();
            cout << "Promedio de edad: " << promedio << " años\n";
        }

        cout << "\n CITAS POR MEDICO:\n";
        for (size_t i=0; i<parser.medicos.size(); i++) {
            Medico med =parser.medicos[i];
            int contador =0;

            for (size_t j=0; j<parser.citas.size(); j++) {
                if (parser.citas[j].nombreMedico == med.nombre) {
                    contador++;
                }
            }
            cout << "   • " << med.nombre << ": " << contador << " cita(s)\n";
        }

    }
    else {
        cout << "\n ERRORES EN ANÁLISIS SINTÁCTICO\n";
    }


    if (parser.erroresSintacticos.size() > 0) {
        cout << "\nERRORES SINTÁCTICOS (" << parser.erroresSintacticos.size() << "):\n";
        for (size_t i =0; i<parser.erroresSintacticos.size(); i++) {
            cout << "   "<< parser.erroresSintacticos[i] << "\n";
        }
    }

    cout << "\n==========================================\n";
    cout << "           FIN DEL ANÁLISIS\n";
    cout << "==========================================\n";

    return 0;
}
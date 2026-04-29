#include "GeneradorReportes.h"
#include <fstream>
#include <sstream>
#include <map>
#include <set>
using namespace std;

// Reporte 1 
void GeneradorReportes::generarReporteKanban(vector<Token>& tokens, string nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open())
        return;

    archivo << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    archivo << "<title>Tablero Kanban</title>";
    archivo << "<style>";
    archivo << "body { font-family: Arial; background: #f0f2f5; }";
    archivo << ".tablero { display: flex; gap: 20px; padding: 20px; }";
    archivo << ".columna { background: #fff; border-radius: 8px; padding: 10px; min-width: 200px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }";
    archivo << ".columna h3 { text-align: center; margin: 5px 0; }";
    archivo << ".tarjeta { background: #fafafa; margin: 10px 0; padding: 8px; border-left: 4px solid #ccc; border-radius: 4px; }";
    archivo << ".prioridad-alta { border-left-color: #e74c3c; }";
    archivo << ".prioridad-media { border-left-color: #f1c40f; }";
    archivo << ".prioridad-baja { border-left-color: #2ecc71; }";
    archivo << ".tarjeta p { margin: 3px 0; }";
    archivo << "</style></head><body>";

    archivo << "<h1 style='text-align:center;'>Tablero Kanban</h1>";
    archivo << "<div class='tablero'>";

    for (size_t i =0; i<tokens.size(); ++i) {
        if (tokens[i].tipo== COLUMNA) {

            string nombreColumna= tokens[i + 1].lexema;
            archivo << "<div class='columna'><h3>" <<nombreColumna << "</h3>";

            size_t j = i+2; 
            while (j <tokens.size() && tokens[j].tipo!= COLUMNA && tokens[j].tipo != FIN_ARCHIVO) {
                if (tokens[j].tipo==TAREA) {
                    string nombreTarea =tokens[j + 2].lexema; 
                    string prioridad ="BAJA";
                    string responsable= "";
                    string fecha ="";

                    int k=j + 3; 
                    while (k <tokens.size() && tokens[k].tipo != LLAVE_DER) {
                        if (tokens[k].tipo==PRIORIDAD) {
                            prioridad =tokens[k + 2].lexema; 
                        }
                        else if (tokens[k].tipo ==RESPONSABLE) {
                            responsable= tokens[k + 2].lexema;
                            responsable = responsable.substr(1, responsable.length() - 2);
                        }
                        else if (tokens[k].tipo== FECHA_LIMITE) {
                            fecha =tokens[k + 2].lexema;
                        }
                        ++k;
                    }

                    string clasePrioridad= "prioridad-baja";
                    if (prioridad =="ALTA")
                        clasePrioridad= "prioridad-alta";

                    else if (prioridad=="MEDIA")
                        clasePrioridad ="prioridad-media";

                    archivo << "<div class='tarjeta " << clasePrioridad << "'>";
                    archivo << "<strong>" << nombreTarea<< "</strong>";
                    archivo << "<p>Prioridad: " << prioridad << "</p>";
                    if (!responsable.empty())
                        archivo << "<p>Responsable: " << responsable << "</p>";

                    if (!fecha.empty()) 
                        archivo << "<p>Fecha límite: "<< fecha << "</p>";

                    archivo << "</div>";
                    j = k+1; 
                }
                else {
                    ++j;
                }
            }
            archivo <<"</div>";
        }
    }
    archivo << "</div></body></html>";
    archivo.close();
}


// Reporte 2                
void GeneradorReportes::generarReporteCarga(vector<Token>& tokens, string nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    archivo << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    archivo << "<title>Carga por Responsable</title>";
    archivo << "<style>";
    archivo << "body { font-family: Arial; background: #fff; }";
    archivo << "table { border-collapse: collapse; width: 80%; margin: 20px auto; }";
    archivo << "th, td { border: 1px solid #ccc; padding: 10px; text-align: center; }";
    archivo << "th { background: #2c3e50; color: white; }";
    archivo << ".barra { background: #ecf0f1; height: 20px; border-radius: 10px; overflow: hidden; }";
    archivo << ".barra div { height: 100%; background: #3498db; }";
    archivo << "</style></head><body>";
    archivo << "<h1 style='text-align:center;'>Carga de Trabajo por Responsable</h1>";

    map<string, int> total, alta, media, baja;
    int totalGeneral =0;

    for (int i= 0; i<tokens.size(); ++i) {
        if (tokens[i].tipo== TAREA) {
            string resp ="";
            string prior ="";
            size_t j=i + 3;
            while (j< tokens.size() && tokens[j].tipo != LLAVE_DER) {
                if (tokens[j].tipo ==RESPONSABLE) {
                    resp =tokens[j + 2].lexema;
                    resp =resp.substr(1, resp.length() - 2);
                }
                else if (tokens[j].tipo==PRIORIDAD) {
                    prior =tokens[j + 2].lexema;
                }
                ++j;
            }
            if (!resp.empty()) {
                total[resp]++;
                totalGeneral++;
                if (prior =="ALTA") 
                    alta[resp]++;

                else if (prior == "MEDIA")
                    media[resp]++;

                else if (prior == "BAJA")
                    baja[resp]++;
            }
        }
    }
    archivo << "<table><tr><th>Responsable</th><th>Total</th><th>Alta</th><th>Media</th><th>Baja</th><th>Distribución</th></tr>";

    for (auto& par:total) {
        string nombre=par.first;
        int t= par.second;
        int a=alta[nombre];
        int m = media[nombre];
        int b =baja[nombre];
        int porcentaje=(totalGeneral >0)?(t * 100 / totalGeneral) : 0;

        archivo << "<tr>";
        archivo << "<td>" << nombre<< "</td>";
        archivo << "<td>" << t<< "</td>";
        archivo << "<td>" << a << "</td>";
        archivo << "<td>" << m << "</td>";
        archivo << "<td>" << b << "</td>";
        archivo << "<td><div class='barra'><div style='width:" << porcentaje << "%;'>" << porcentaje << "%</div></div></td>";
        archivo << "</tr>";
    }
    archivo << "</table></body></html>";
    archivo.close();
}


// reporte 3
void GeneradorReportes::generarReporteTareasFechas(vector<Token>& tokens, string nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) 
        return;

    archivo << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    archivo << "<title>Tareas y Fechas Límite</title>";
    archivo << "<style>body { font-family: Arial; margin: 20px; } table { border-collapse: collapse; width: 100%; } th, td { border: 1px solid #ccc; padding: 8px; } th { background: #2980b9; color: white; }</style>";
    archivo << "</head><body><h1>Tareas y Fechas Límite</h1><table><tr><th>Tarea</th><th>Responsable</th><th>Fecha Límite</th></tr>";

    for (int i =0; i<tokens.size(); ++i) {
        if (tokens[i].tipo==TAREA) {
            string nombreTarea =tokens[i + 2].lexema;
            string responsable="";
            string fecha ="";
            size_t j = i+3;
            while (j < tokens.size() && tokens[j].tipo != LLAVE_DER) {
                if (tokens[j].tipo==RESPONSABLE) {
                    responsable=tokens[j + 2].lexema;
                    responsable= responsable.substr(1, responsable.length() - 2);
                }
                else if (tokens[j].tipo== FECHA_LIMITE) {
                    fecha =tokens[j + 2].lexema;
                }
                ++j;
            }
            archivo << "<tr><td>" << nombreTarea << "</td><td>" << (responsable.empty() ? "-" : responsable) << "</td><td>" << (fecha.empty() ? "-" : fecha) << "</td></tr>";
        }
    }
    archivo <<"</table></body></html>";
    archivo.close();
}


//reporte 4 DOT
void GeneradorReportes::generarArbolDot(NodoArbol* raiz, string nombreArchivo) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) 
        return;

    if (raiz ==NULL) {
        archivo.close();
        return;
    }
    archivo << "digraph ArbolDerivacion {\n";
    archivo << "rankdir=TB;\n";
    archivo << "node [shape=box, style=filled, fontname=\"Arial\"];\n";

    int contador =0;

    escribirNodoRecursivo(raiz, -1, contador, archivo);
    archivo << "}\n";
    archivo.close();
}


void GeneradorReportes::escribirNodoRecursivo(NodoArbol* nodo, int idPadre, int& contador, ofstream& archivo) {
    int miId = contador;
    contador++;

    // Escapar la etiqueta antes de escribirla
    string etiquetaEscapada = escaparDot(nodo->etiqueta);

    if (nodo->tipoToken == DESCONOCIDO) {
        archivo << "n" << miId << " [label=\"" << etiquetaEscapada << "\", fillcolor=\"#2E75B6\", fontcolor=\"white\"];\n";
    }
    else {
        archivo << "n" << miId << " [label=\"" << etiquetaEscapada<< "\", fillcolor=\"#D6EAF8\", fontcolor=\"black\"];\n";
    }
    if (idPadre != -1) {
        archivo << "n"<<idPadre << " -> n" << miId << ";\n";
    }

    for (int i =0; i<nodo->hijos.size(); i++) {
        escribirNodoRecursivo(nodo->hijos[i], miId, contador, archivo);
    }
}

//metodo traido del "proyecto 1" para escapar caracteres especiales en DOT
string GeneradorReportes::escaparDot(string texto) {
    string resultado = "";
    for (int i = 0; i < texto.length(); i++) {
        char c = texto[i];
        if (c == '"') {
            resultado += "\\\"";   
        }
        else if (c=='\\') {
            resultado += "\\\\";  
        }
        else {
            resultado+=c;
        }
    }
    return resultado;
}
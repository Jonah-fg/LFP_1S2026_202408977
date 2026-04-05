# LFP_1S2026_202408977
# MedLexer - Analizador Léxico para Gestión Hospitalaria
**Autor:** Jonathan Eduardo Fuentes Garcia 
**Carné:** 202408977  
**Curso:** Lenguajes Formales y de Programación  
**Semestre:** 1S2026

## Descripción
MedLexer es una aplicación de escritorio desarrollada en **C++** con interfaz gráfica **Qt 6**, que implementa un analizador léxico manual (Autómata Finito Determinista) para interpretar archivos con extensión `.med` escritos en el lenguaje **MedLang**. El sistema permite cargar, analizar y extraer información estructurada de pacientes, médicos, citas y diagnósticos, generando reportes HTML y diagramas Graphviz.

## Características principales
- Análisis léxico manual sin expresiones regulares (AFD implementado en C++).
- Tabla de tokens con número, lexema, tipo, línea y columna.
- Detección y reporte de errores léxicos y sintácticos con posición exacta.
- Recuperación de errores: el análisis continúa para reportar todos los problemas.
- Interfaz gráfica en Qt con paneles para código fuente, tokens y errores.
- Generación de 4 reportes HTML con estadísticas médicas.
- Generación de diagrama de relaciones (Graphviz) del hospital.

## Requisitos previos
- **Sistema operativo**: Windows, Linux o macOS.
- **Compilador**: g++ con soporte C++17 (MinGW, GCC) o MSVC 2019/2022.
- **Qt**: Versión 6.x (open source).
- **Graphviz**: Necesario para convertir el archivo `.dot` a PNG (opcional).

### Instalación de dependencias

#### En Windows (con Qt Creator)
1. Descargar e instalar **Qt 6.x Open Source** desde [qt.io](https://www.qt.io/download-open-source).
2. Durante la instalación, seleccionar el compilador **MinGW 11.2.0** o **MSVC 2022**.
3. Instalar **Graphviz** desde [graphviz.org](https://graphviz.org/download/) y agregarlo al PATH.
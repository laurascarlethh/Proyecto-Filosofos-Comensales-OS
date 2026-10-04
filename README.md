# Proyecto-Filosofos-Comensales-OS
# Simulación del Problema de los Filósofos Comensales (Monitores & C++)
Este repositorio contiene la solución e implementación del problema clásico de concurrencia **"Los Filósofos Comensales"**, desarrollado como proyecto académico para la asignatura de **Sistemas Operativos**.
---
## Descripción del Proyecto
El objetivo principal es resolver la competencia por recursos compartidos (palillos) entre hilos concurrentes (filósofos) garantizando la **exclusión mutua** y previniendo problemas como el **interbloqueo (*deadlock*)** y la **inanición (*starvation*)**.
Para lograr esto, se diseñó e implementó un **Monitor** utilizando C++ e hilos nativos con la API de Windows (`<windows.h>`) mediante secciones críticas (`CRITICAL_SECTION`).
---
## Tecnologías y Herramientas Utilizadas
* **Lenguaje:** C++
* **Herramienta de Diseño:** PSeInt (Diseño del algoritmo base y diagramas de flujo)
* **Mecanismos de Sincronización:** Monitores y Secciones Críticas (`CRITICAL_SECTION`)
* **Gestión de Memoria:** Memoria dinámica con punteros (`new` / `delete`)
* **Entorno de Desarrollo:** Dev-C++ / MinGW / VS Code
---
## Características de la Implementación
1. **Exclusión Mutua Estricta:** Solo un hilo a la vez puede modificar la estructura de estados dentro del monitor.
2. **Evaluación de Vecinos:** Un filósofo solo cambia al estado `COMIENDO` si ni su vecino izquierdo ni el derecho están comiendo.
3. **Espera Pasiva y Sincronización:** Si los recursos no están disponibles, los filósofos esperan ordenadamente hasta que un vecino libere los palillos.
4. **Liberación Limpia de Recursos:** Gestión explícita de la memoria dinámica eliminando punteros y estructuras al terminar.
5. **Menú Interactivo:** Permite configurar rondas, ejecutar la simulación y realizar la limpieza de memoria de forma explícita.
---
## Instrucciones de Compilación y Ejecución
### Opción 1: En Dev-C++
1. Abrir el archivo `main.cpp` en **Dev-C++**.
2. Presionar **F11** (Compilar y Ejecutar).
### Opción 2: Desde Consola (G++)
```bash
g++ main.cpp -o filosofos
./filosofos
```bash
g++ main.cpp -o filosofos
./filosofos

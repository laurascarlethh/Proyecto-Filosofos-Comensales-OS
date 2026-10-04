#include <iostream>
#include <windows.h> // API de Windows para hilos y exclusion mutua nativos en Dev-C++

using namespace std;

// Constantes globales
const int NUM_FILOSOFOS = 5;

// Estados posibles de un filosofo
enum Estado { PENSANDO, HAMBRIENTO, COMIENDO };

// =========================================================
// ESTRUCTURA DEL MONITOR (NATIVO WINDOWS / DEV-C++)
// =========================================================
struct MonitorMesa {
    Estado* estados;                // Arreglo dinamico de estados
    CRITICAL_SECTION cerrojo;       // Seccion critica para exclusión mutua nativa

    // Constructor: Inicializa la memoria dinamica y la seccion critica
    MonitorMesa() {
        estados = new Estado[NUM_FILOSOFOS];
        InitializeCriticalSection(&cerrojo);

        for (int i = 0; i < NUM_FILOSOFOS; i++) {
            estados[i] = PENSANDO;
        }
    }

    // Destructor: Libera la memoria reservada y la seccion critica
    ~MonitorMesa() {
        delete[] estados;
        DeleteCriticalSection(&cerrojo);
    }

    // Metodo para probar si un filosofo puede comer
    void probar(int id) {
        int izq = (id + NUM_FILOSOFOS - 1) % NUM_FILOSOFOS;
        int der = (id + 1) % NUM_FILOSOFOS;

        // Regla del monitor: solo come si no hay conflicto con sus vecinos
        if (estados[id] == HAMBRIENTO && estados[izq] != COMIENDO && estados[der] != COMIENDO) {
            estados[id] = COMIENDO;
            cout << "  ==> [Filosofo " << id + 1 << "] Tomo los palillos y esta COMIENDO.\n";
        }
    }

    // Solicitar tomar palillos
    void tomarPalillos(int id) {
        EnterCriticalSection(&cerrojo); // Bloqueo exclusivo del monitor

        cout << "[Filosofo " << id + 1 << "] Tiene hambre e intenta comer...\n";
        estados[id] = HAMBRIENTO;

        probar(id);

        if (estados[id] != COMIENDO) {
            cout << "  [ESPERA] Filosofo " << id + 1 << " debe esperar a que sus vecinos liberen palillos.\n";
        }

        LeaveCriticalSection(&cerrojo); // Libera la seccion critica
    }

    // Liberar palillos
    void soltarPalillos(int id) {
        EnterCriticalSection(&cerrojo); // Bloqueo exclusivo del monitor

        int izq = (id + NUM_FILOSOFOS - 1) % NUM_FILOSOFOS;
        int der = (id + 1) % NUM_FILOSOFOS;

        estados[id] = PENSANDO;
        cout << "  <== [Filosofo " << id + 1 << "] Termino de comer. Libera palillos y vuelve a PENSAR.\n";

        // Verifica si sus vecinos estaban esperando para comer
        probar(izq);
        probar(der);

        LeaveCriticalSection(&cerrojo); // Libera la seccion critica
    }
};

// Puntero global al monitor
MonitorMesa* mesaMonitor = 0;

// Estructura para pasar datos a cada hilo individual
struct DatosHilo {
    int id;
    int rondas;
};

// =========================================================
// FUNCIÓN QUE EJECUTA CADA HILO EN WINDOWS (DEV-C++)
// =========================================================
DWORD WINAPI accionFilosofo(LPVOID param) {
    DatosHilo* datos = (DatosHilo*)param;
    int id = datos->id;
    int rondas = datos->rondas;

    for (int i = 0; i < rondas; i++) {
        Sleep(300); // Pensando
        if (mesaMonitor != 0) {
            mesaMonitor->tomarPalillos(id);
        }
        Sleep(500); // Comiendo
        if (mesaMonitor != 0) {
            mesaMonitor->soltarPalillos(id);
        }
    }

    delete datos; // Liberar memoria del parametro
    return 0;
}

// =========================================================
// FUNCIÓN PRINCIPAL CON MENÚ INTERACTIVO
// =========================================================
int main() {
    int opcion = 0;
    int rondas = 2;

    // Arreglo de manejadores de hilos para Windows
    HANDLE hilos[NUM_FILOSOFOS];

    do {
        cout << "\n========================================\n";
        cout << "   SIMULADOR: FILOSOFOS COMENSALES      \n";
        cout << "========================================\n\n";
        cout << "1. Configurar rondas de comida (Actual: " << rondas << ")\n";
        cout << "2. Ejecutar simulacion multihilo (Monitor)\n";
        cout << "3. Liberar memoria del sistema\n";
        cout << "4. Salir del programa\n\n";
        cout << "========================================\n\n";
        cout << "Seleccione una opcion (1-4): ";
        cin >> opcion;

        switch (opcion) {

            case 1:
                cout << "\n--- CONFIGURACION DE RONDAS ---\n";
                cout << "Ingrese la cantidad de veces que comera cada filosofo (1-5): ";
                cin >> rondas;

                if (rondas < 1 || rondas > 5) {
                    cout << "Cantidad no valida. Se establece en 2 rondas por defecto.\n";
                    rondas = 2;
                } else {
                    cout << "Configuracion guardada exitosamente.\n";
                }
                break;

            case 2:
                cout << "\n--- INICIANDO SIMULACION DE CONCURRENCIA EN DEV-C++ ---\n";

                // Si existia una instancia previa del monitor, la liberamos
                if (mesaMonitor != 0) {
                    delete mesaMonitor;
                    mesaMonitor = 0;
                }

                // Reservamos memoria dinamica para el monitor
                mesaMonitor = new MonitorMesa();

                // Creacion de los hilos de ejecucion nativos
                for (int i = 0; i < NUM_FILOSOFOS; i++) {
                    DatosHilo* datos = new DatosHilo;
                    datos->id = i;
                    datos->rondas = rondas;

                    hilos[i] = CreateThread(NULL, 0, accionFilosofo, datos, 0, NULL);
                }

                // Esperar a que todos los hilos terminen
                WaitForMultipleObjects(NUM_FILOSOFOS, hilos, TRUE, INFINITE);

                // Cerrar las etiquetas de los hilos
                for (int i = 0; i < NUM_FILOSOFOS; i++) {
                    CloseHandle(hilos[i]);
                }

                cout << "\n========================================\n";
                cout << "  SIMULACION CONCLUIDA SIN INTERBLOQUEOS  \n";
                cout << "========================================\n";
                break;

            case 3:
                cout << "\n--- LIBERACION DE MEMORIA DINAMICA ---\n";

                if (mesaMonitor != 0) {
                    delete mesaMonitor;
                    mesaMonitor = 0;
                    cout << "Instancia del Monitor liberada correctamente.\n";
                } else {
                    cout << "No hay memoria del monitor reservada para liberar.\n";
                }
                break;

            case 4:
                cout << "\nSaliendo del programa...\n";

                if (mesaMonitor != 0) {
                    delete mesaMonitor;
                    mesaMonitor = 0;
                }

                cout << "Memoria final liberada correctamente :)\n";
                break;

            default:
                cout << "\nOpcion invalida. Por favor, seleccione un numero del 1 al 4.\n";
                break;
        }

    } while (opcion != 4);

    return 0;
}

# Simple OS Simulator - C++

Simple OS Simulator es un prototipo funcional de Sistema Operativo desarrollado en C++ para entornos WSL/Linux. Este proyecto proporciona un entorno práctico e interactivo para comprender y experimentar con la gestión de memoria, planificación de procesos y sincronización mediante hilos reales.

## 📑 Tabla de Contenidos

- [Características principales](#características-principales)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Conceptos teóricos implementados](#conceptos-teóricos-implementados)
- [Arquitectura del sistema](#arquitectura-del-sistema)
- [Componentes del simulador](#componentes-del-simulador)
- [Instalación y configuración](#instalación-y-configuración)
- [Manual de comandos](#manual-de-comandos)
- [Ejemplos de uso](#ejemplos-de-uso)
- [Escenarios de demostración](#escenarios-de-demostración)

## 🚀 Características principales

- **Shell interactivo completo**: Interfaz de línea de comandos intuitiva para interactuar con todos los componentes del sistema operativo
- **Gestión avanzada de procesos**: Creación, monitoreo y terminación de procesos con hilos reales (`std::thread`)
- **Planificador FCFS**: Algoritmo First-Come, First-Served con ejecución concurrente de múltiples procesos
- **Gestor de memoria First-Fit**: Sistema de gestión de memoria con soporte para asignación, liberación y fusión automática de bloques
- **Sincronización robusta**: Implementación de `std::mutex` y `std::condition_variable` para protección de recursos críticos
- **Multithreading real**: Cada proceso ejecuta en su propio hilo del sistema operativo
- **Visualización de estados**: Herramientas para visualizar el estado de la memoria y procesos en tiempo real
- **Manejo de señales**: Captura de Ctrl+C para terminación ordenada del sistema

## 📁 Estructura del proyecto

```
Simple-OS-Simulator/
├── MemoryManager.h           # Declaración del gestor de memoria
├── MemoryManager.cpp         # Implementación First-Fit + fusión de bloques
├── ProcessScheduler.h        # Declaración del planificador FCFS
├── ProcessScheduler.cpp      # Implementación con std::thread
├── Shell.h                   # Declaración del shell interactivo
├── Shell.cpp                 # Implementación del intérprete de comandos
├── main.cpp                  # Punto de entrada del simulador
├── Makefile                  # Sistema de compilación automática
├── compile.sh                # Script de compilación rápida
└── README.md                 # Documentación principal
```

## 📚 Conceptos teóricos implementados

### Planificación de procesos (FCFS)

Algoritmo de planificación no apropiativo donde cada proceso ejecuta hasta completarse:

- **Simplicidad**: Cola FIFO (`std::queue`) con procesos ejecutándose en orden de llegada
- **No preemption**: Los procesos corren hasta terminar naturalmente
- **Concurrencia real**: Cada proceso ejecuta en su propio `std::thread` del sistema operativo
- **Sincronización**: Uso de `std::mutex` y `std::condition_variable` para coordinación

**Funcionamiento del algoritmo:**
```cpp
while (scheduler_running) {
    if (!ready_queue.empty()) {
        auto process = ready_queue.front();
        ready_queue.pop();
        
        // Crear hilo para el proceso
        process->thread_ptr = std::make_unique<std::thread>(
            &ProcessScheduler::process_execution, this, process
        );
    }
}
```

### Gestión de memoria (First-Fit)

Sistema de asignación de memoria contigua que busca el primer bloque disponible:

- **Algoritmo rápido**: Encuentra el primer bloque libre suficientemente grande en O(n)
- **División de bloques**: Si el bloque es mayor, se divide en [ocupado][libre]
- **Fusión automática**: Los bloques libres adyacentes se combinan automáticamente
- **Thread-safe**: Todas las operaciones protegidas con `std::mutex`

**Estados de memoria:**
```cpp
struct Block {
    size_t size;        // Tamaño del bloque en bytes
    bool is_free;       // true = libre, false = ocupado
    size_t start_addr;  // Dirección de inicio (simulada)
};
```

**Algoritmo First-Fit:**
```cpp
for (bloque : bloques_memoria) {
    if (bloque.is_free && bloque.size >= tamaño_requerido) {
        asignar_bloque(bloque);
        if (bloque.size > tamaño_requerido) {
            dividir_bloque(bloque, tamaño_requerido);
        }
        return bloque.start_addr;
    }
}
```

### Sincronización (std::mutex + std::condition_variable)

Primitivas de sincronización para proteger recursos compartidos:

- **Exclusión mutua**: `std::mutex` garantiza acceso exclusivo a estructuras compartidas
- **Variables de condición**: `std::condition_variable` para notificación entre hilos
- **RAII con lock_guard**: Gestión automática de locks para prevenir deadlocks
- **Operaciones atómicas**: `std::atomic<int>` para contadores thread-safe (PIDs)

**Protección de recursos críticos:**
```cpp
// En MemoryManager
std::lock_guard<std::mutex> lock(memory_mutex);
// Operaciones sobre memory_blocks protegidas

// En ProcessScheduler
std::unique_lock<std::mutex> lock(scheduler_mutex);
cv.wait(lock, [this] { return !ready_queue.empty() || !scheduler_running; });
```

## 🏗️ Arquitectura del sistema

El simulador implementa una arquitectura por capas donde cada componente tiene responsabilidades claramente definidas:

```
┌─────────────────────────────────────────────────────┐
│              CAPA DE PRESENTACIÓN                   │
│   ┌─────────────────────────────────────────┐       │
│   │           Shell Interactivo             │       │
│   │  - Análisis de comandos                 │       │
│   │  - Validación de entrada                │       │
│   │  - Interfaz de usuario (REPL)           │       │
│   └─────────────────────────────────────────┘       │
└─────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────┐
│           CAPA DE GESTIÓN DE PROCESOS               │
│   ┌──────────────────────┐  ┌──────────────────┐   │
│   │  ProcessScheduler    │  │  Process (PCB)   │   │
│   │  - Cola FCFS         │  │  - PID           │   │
│   │  - Scheduler loop    │  │  - std::thread   │   │
│   │  - std::mutex        │  │  - Memoria req.  │   │
│   │  - condition_var     │  │  - Estado        │   │
│   └──────────────────────┘  └──────────────────┘   │
└─────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────┐
│          CAPA DE RECURSOS DEL SISTEMA               │
│   ┌─────────────────────────────────────────┐       │
│   │         MemoryManager                   │       │
│   │  - std::vector<Block> memory_blocks     │       │
│   │  - Algoritmo First-Fit                  │       │
│   │  - Fusión de bloques libres             │       │
│   │  - std::mutex para thread-safety        │       │
│   └─────────────────────────────────────────┘       │
└─────────────────────────────────────────────────────┘
```

### Flujo de ejecución: Creación de un proceso

```
┌─────────┐     ┌────────┐     ┌──────────────┐     ┌───────────────┐     ┌─────────┐
│ Usuario │────>│ Shell  │────>│ Scheduler    │────>│ MemoryManager │────>│ Proceso │
└─────────┘     └────────┘     └──────────────┘     └───────────────┘     └─────────┘
     │              │                  │                      │                  │
     │ exec P1 512  │                  │                      │                  │
     │─────────────>│                  │                      │                  │
     │              │ crear_proceso()  │                      │                  │
     │              │─────────────────>│                      │                  │
     │              │                  │ alloc(512)           │                  │
     │              │                  │─────────────────────>│                  │
     │              │                  │                      │ First-Fit        │
     │              │                  │                      │ search...        │
     │              │                  │<─────────────────────│                  │
     │              │                  │ addr=0               │                  │
     │              │                  │                      │                  │
     │              │                  │ Crear std::thread    │                  │
     │              │                  │──────────────────────────────────────>  │
     │              │                  │                      │                  │
     │              │                  │ Añadir a ready_queue │                  │
     │              │                  │ cv.notify_one()      │                  │
     │              │<─────────────────│                      │                  │
     │              │ PID=1            │                      │                  │
     │<─────────────│                  │                      │                  │
     │ "Proceso     │                  │                      │                  │
     │  creado"     │                  │ scheduler_loop()     │                  │
     │              │                  │ ejecuta proceso...   │                  │
     │              │                  │                      │                  │<─┐
     │              │                  │                      │                  │  │
     │              │                  │                      │                  │  │ Simulación
     │              │                  │                      │                  │  │ de trabajo
     │              │                  │                      │                  │  │ (1-5 seg)
     │              │                  │                      │                  │<─┘
     │              │                  │<──────────────────────────────────────────│
     │              │                  │ Proceso terminado    │                  │
     │              │                  │                      │                  │
     │              │                  │ free(addr)           │                  │
     │              │                  │─────────────────────>│                  │
     │              │                  │                      │ Liberar bloque   │
     │              │                  │                      │ Fusionar libres  │
```

## 🔧 Componentes del simulador

### Shell (Shell.h / Shell.cpp)

**Funcionalidad**: Interfaz de línea de comandos que implementa un bucle REPL completo.

**Características principales**:
- Parsing de comandos con tokenización (`std::istringstream`)
- Validación de argumentos y manejo de errores descriptivos
- Mensajes informativos con prefijos `[SHELL]`
- Integración directa con `MemoryManager` y `ProcessScheduler`

**Métodos principales**:
```cpp
void run()                              // Bucle principal REPL
void process_command(const string& cmd) // Procesador de comandos
void cmd_alloc(const vector<string>&)   // Comando: alloc
void cmd_exec(const vector<string>&)    // Comando: exec
void cmd_free(const vector<string>&)    // Comando: free
void cmd_ps(const vector<string>&)      // Comando: ps
void cmd_mem(const vector<string>&)     // Comando: mem
void cmd_kill(const vector<string>&)    // Comando: kill
void cmd_help(const vector<string>&)    // Comando: help
void cmd_clear(const vector<string>&)   // Comando: clear
```

### ProcessScheduler (ProcessScheduler.h / ProcessScheduler.cpp)

**Funcionalidad**: Implementa el planificador FCFS con soporte completo para multithreading real.

**Atributos principales**:
```cpp
MemoryManager& memory_manager;           // Referencia al gestor de memoria
std::queue<shared_ptr<Process>> ready_queue;  // Cola FCFS de procesos listos
unordered_map<int, shared_ptr<Process>> running_processes;  // Procesos activos
std::mutex scheduler_mutex;              // Protección para estructuras compartidas
std::condition_variable cv;              // Notificación entre hilos
std::atomic<int> next_pid;               // Generador de PIDs thread-safe
std::thread scheduler_thread;            // Hilo principal del planificador
```

**Métodos principales**:
```cpp
int crear_proceso(const string& name, size_t mem)  // Crea proceso y pide memoria
void start_scheduler()                             // Inicia el hilo del scheduler
void stop_scheduler()                              // Detiene ordenadamente
void scheduler_loop()                              // Bucle principal FCFS
void execute_process(shared_ptr<Process> p)        // Crea hilo para proceso
void process_execution(shared_ptr<Process> p)      // Simula ejecución (1-5 seg)
void display_processes() const                     // Muestra estado de procesos
bool terminate_process(int pid)                    // Termina proceso por PID
```

**Estructura Process**:
```cpp
struct Process {
    int pid;                              // ID único del proceso
    std::string name;                     // Nombre descriptivo
    size_t memory_required;               // Bytes de memoria necesarios
    size_t memory_address;                // Dirección base asignada
    std::unique_ptr<std::thread> thread_ptr;  // Hilo del proceso
};
```

### MemoryManager (MemoryManager.h / MemoryManager.cpp)

**Funcionalidad**: Gestor de memoria con algoritmo First-Fit y fusión automática de bloques.

**Atributos principales**:
```cpp
std::vector<Block> memory_blocks;        // Vector de bloques de memoria
size_t total_memory;                     // Memoria total disponible (8192 bytes)
mutable std::mutex memory_mutex;         // Protección thread-safe
```

**Estructura Block**:
```cpp
struct Block {
    size_t size;          // Tamaño del bloque
    bool is_free;         // Estado: true=libre, false=ocupado
    size_t start_addr;    // Dirección de inicio (0, 512, 1024, ...)
};
```

**Métodos principales**:
```cpp
MemoryManager(size_t total_size)         // Constructor: crea bloque inicial libre
size_t alloc(size_t size)                // Asigna memoria con First-Fit
bool free(size_t start_addr)             // Libera bloque y fusiona adyacentes
void display_memory() const              // Muestra mapa visual de memoria
void get_memory_stats(...) const         // Estadísticas: total, usado, libre
void merge_free_blocks()                 // Fusiona bloques libres contiguos
```

**Algoritmo de fusión de bloques**:
```cpp
void merge_free_blocks() {
    // 1. Ordenar bloques por dirección
    sort(memory_blocks.begin(), memory_blocks.end(), 
         [](const Block& a, const Block& b) { 
             return a.start_addr < b.start_addr; 
         });
    
    // 2. Fusionar bloques libres adyacentes
    for (auto it = memory_blocks.begin(); it != memory_blocks.end() - 1;) {
        if (it->is_free && (it + 1)->is_free && 
            (it->start_addr + it->size == (it + 1)->start_addr)) {
            it->size += (it + 1)->size;
            memory_blocks.erase(it + 1);
        } else {
            ++it;
        }
    }
}
```

### main.cpp

**Funcionalidad**: Punto de entrada que inicializa todos los componentes del sistema.

**Proceso de inicialización**:
```cpp
int main() {
    // 1. Configurar manejador de señales (Ctrl+C)
    signal(SIGINT, signal_handler);
    
    // 2. Crear gestor de memoria con 8192 bytes
    MemoryManager memory_manager(8192);
    
    // 3. Crear planificador de procesos
    ProcessScheduler process_scheduler(memory_manager);
    
    // 4. Crear shell interactivo
    Shell shell(memory_manager, process_scheduler);
    global_shell = &shell;  // Para captura de señales
    
    // 5. Iniciar scheduler en hilo separado
    process_scheduler.start_scheduler();
    
    // 6. Ejecutar bucle del shell (bloquea hasta exit)
    shell.run();
    
    // 7. Detener scheduler ordenadamente
    process_scheduler.stop_scheduler();
    
    return 0;
}
```

## ⚙️ Instalación y configuración

### Prerrequisitos

- **Sistema operativo**: WSL Ubuntu, Linux (cualquier distribución)
- **Compilador**: g++ 7.0+ con soporte C++17
- **Bibliotecas**: pthread (incluida en Linux)
- **Herramientas opcionales**: make

### Pasos de instalación

1. **Instalar dependencias en Ubuntu/WSL**:
```bash
sudo apt update
sudo apt install build-essential g++ make
```

2. **Verificar instalación**:
```bash
g++ --version
# Debe mostrar versión 7.0 o superior
```

3. **Compilar el proyecto**:
```bash
# Opción 1: Con Makefile
make clean
make

# Opción 2: Con script
chmod +x compile.sh
./compile.sh

# Opción 3: Manual
g++ -std=c++17 -Wall -Wextra -O2 -pthread \
    main.cpp MemoryManager.cpp ProcessScheduler.cpp Shell.cpp \
    -o os_sim
```

4. **Ejecutar**:
```bash
./os_sim
```

### Configuración del sistema

**Modificar tamaño de memoria** (main.cpp línea 30):
```cpp
const size_t TOTAL_MEMORY = 8192;  // Cambiar a 16384, 32768, etc.
```

**Modificar tiempo de ejecución de procesos** (ProcessScheduler.cpp línea 131):
```cpp
std::uniform_int_distribution<> dis(1000, 5000);  // min y max en milisegundos
```

## 📖 Manual de comandos

### Gestión de memoria

| Comando | Sintaxis | Descripción | Ejemplo |
|---------|----------|-------------|---------|
| `alloc` | `alloc <tamaño>` | Asigna un bloque de memoria usando First-Fit | `alloc 1024` |
| `free` | `free <dirección>` | Libera el bloque en la dirección especificada | `free 0` |
| `mem` | `mem` | Muestra el mapa completo de la memoria con estadísticas | `mem` |

### Gestión de procesos

| Comando | Sintaxis | Descripción | Ejemplo |
|---------|----------|-------------|---------|
| `exec` | `exec <nombre> <memoria>` | Crea un proceso con memoria especificada y lo ejecuta | `exec editor 512` |
| `ps` | `ps` | Lista todos los procesos en ejecución con sus estados | `ps` |
| `kill` | `kill <pid>` | Termina forzosamente el proceso con el PID especificado | `kill 1` |

### Comandos del sistema

| Comando | Sintaxis | Descripción | Ejemplo |
|---------|----------|-------------|---------|
| `help` | `help` | Muestra todos los comandos disponibles con ejemplos | `help` |
| `clear` | `clear` | Limpia la pantalla del terminal | `clear` |
| `exit` | `exit` o `quit` | Sale del simulador de forma ordenada | `exit` |

## 🎯 Ejemplos de uso

### Ejemplo 1: Ciclo básico de un proceso

```bash
SimpleOS> # Ver estado inicial de la memoria
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            8192        LIBRE
----------------------------------------
Total: 8192 | Usado: 0 | Libre: 8192

SimpleOS> # Crear un proceso que necesita 512 bytes
SimpleOS> exec calculadora 512
[MEMORY] Asignados 512 bytes en dirección 0
[SCHEDULER] Proceso creado: calculadora (PID: 1, Memoria: 512 bytes en dirección 0)
[SCHEDULER] Ejecutando proceso calculadora (PID: 1)
[PROCESO 1] Iniciando ejecución de calculadora

SimpleOS> # Ver estado mientras el proceso ejecuta
SimpleOS> ps
=== Estado de Procesos ===
Procesos en cola de listos: 0
Procesos en ejecución: 1

Procesos en ejecución:
PID    Nombre          Memoria    Dirección
------------------------------------------------
1      calculadora     512        0

SimpleOS> # El proceso imprime mensajes mientras trabaja
[PROCESO 1] calculadora trabajando... (Memoria: 0)
[PROCESO 1] calculadora trabajando... (Memoria: 0)
[PROCESO 1] calculadora terminado después de 3421ms
[MEMORY] Liberados 512 bytes en dirección 0

SimpleOS> # Ver memoria después de que el proceso termina
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            8192        LIBRE
----------------------------------------
Total: 8192 | Usado: 0 | Libre: 8192
```

### Ejemplo 2: Múltiples procesos concurrentes

```bash
SimpleOS> # Crear varios procesos que ejecutarán simultáneamente
SimpleOS> exec navegador 2048
[MEMORY] Asignados 2048 bytes en dirección 0
[SCHEDULER] Proceso creado: navegador (PID: 1, Memoria: 2048 bytes en dirección 0)

SimpleOS> exec editor 1024
[MEMORY] Asignados 1024 bytes en dirección 2048
[SCHEDULER] Proceso creado: editor (PID: 2, Memoria: 1024 bytes en dirección 2048)

SimpleOS> exec terminal 512
[MEMORY] Asignados 512 bytes en dirección 3072
[SCHEDULER] Proceso creado: terminal (PID: 3, Memoria: 512 bytes en dirección 3072)

SimpleOS> # Ver todos los procesos ejecutando concurrentemente
SimpleOS> ps
=== Estado de Procesos ===
Procesos en cola de listos: 0
Procesos en ejecución: 3

Procesos en ejecución:
PID    Nombre        Memoria    Dirección
------------------------------------------------
1      navegador     2048       0
2      editor        1024       2048
3      terminal      512        3072

SimpleOS> # Los tres procesos imprimen mensajes simultáneamente
[PROCESO 1] navegador trabajando... (Memoria: 0)
[PROCESO 2] editor trabajando... (Memoria: 2048)
[PROCESO 3] terminal trabajando... (Memoria: 3072)
[PROCESO 1] navegador trabajando... (Memoria: 0)
[PROCESO 3] terminal terminado después de 2156ms
[MEMORY] Liberados 512 bytes en dirección 3072
[PROCESO 2] editor trabajando... (Memoria: 2048)
[PROCESO 1] navegador trabajando... (Memoria: 0)
```

### Ejemplo 3: Gestión manual de memoria y fragmentación

```bash
SimpleOS> # Asignar varios bloques de memoria
SimpleOS> alloc 1000
[MEMORY] Asignados 1000 bytes en dirección 0

SimpleOS> alloc 500
[MEMORY] Asignados 500 bytes en dirección 1000

SimpleOS> alloc 800
[MEMORY] Asignados 800 bytes en dirección 1500

SimpleOS> alloc 1200
[MEMORY] Asignados 1200 bytes en dirección 2300

SimpleOS> # Ver el estado de la memoria completamente asignada
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            1000        OCUPADO
1000         500         OCUPADO
1500         800         OCUPADO
2300         1200        OCUPADO
3500         4692        LIBRE
----------------------------------------
Total: 8192 | Usado: 3500 | Libre: 4692

SimpleOS> # Liberar el bloque del medio para crear fragmentación
SimpleOS> free 1500
[MEMORY] Liberados 800 bytes en dirección 1500

SimpleOS> # Ver fragmentación creada
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            1000        OCUPADO
1000         500         OCUPADO
1500         800         LIBRE
2300         1200        OCUPADO
3500         4692        LIBRE
----------------------------------------
Total: 8192 | Usado: 2700 | Libre: 5492

SimpleOS> # Liberar bloque adyacente para demostrar fusión automática
SimpleOS> free 2300
[MEMORY] Liberados 1200 bytes en dirección 2300

SimpleOS> # Ver cómo los bloques libres se fusionaron automáticamente
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            1000        OCUPADO
1000         500         OCUPADO
1500         6692        LIBRE
----------------------------------------
Total: 8192 | Usado: 1500 | Libre: 6692

SimpleOS> # Los bloques en 1500, 2300 y 3500 se fusionaron en uno solo
```

## 🧪 Escenarios de demostración

### Escenario 1: Demostración de multithreading real

Este escenario muestra cómo múltiples procesos ejecutan verdaderamente en paralelo:

```bash
SimpleOS> # Crear 4 procesos que ejecutarán en hilos separados
SimpleOS> exec proceso_A 600
SimpleOS> exec proceso_B 700
SimpleOS> exec proceso_C 500
SimpleOS> exec proceso_D 800

SimpleOS> ps
# Todos aparecen como "en ejecución"

# Los mensajes se intercalan demostrando ejecución paralela real:
[PROCESO 1] proceso_A trabajando... (Memoria: 0)
[PROCESO 3] proceso_C trabajando... (Memoria: 1300)
[PROCESO 2] proceso_B trabajando... (Memoria: 600)
[PROCESO 4] proceso_D trabajando... (Memoria: 1800)
[PROCESO 1] proceso_A trabajando... (Memoria: 0)
[PROCESO 3] proceso_C terminado después de 2301ms
[PROCESO 2] proceso_B trabajando... (Memoria: 600)
[PROCESO 4] proceso_D trabajando... (Memoria: 1800)
# El orden de los mensajes es impredecible (verdadera concurrencia)
```

### Escenario 2: Algoritmo First-Fit en acción

Demuestra cómo First-Fit encuentra el primer bloque disponible:

```bash
SimpleOS> # Crear patrón de memoria fragmentada
SimpleOS> alloc 1000  # Bloque A en dirección 0
SimpleOS> alloc 2000  # Bloque B en dirección 1000
SimpleOS> alloc 1000  # Bloque C en dirección 3000
SimpleOS> alloc 1500  # Bloque D en dirección 4000

SimpleOS> mem
# [A:1000][B:2000][C:1000][D:1500][LIBRE:2692]

SimpleOS> # Liberar bloque B (crea hueco de 2000 bytes)
SimpleOS> free 1000

SimpleOS> mem
# [A:1000][LIBRE:2000][C:1000][D:1500][LIBRE:2692]

SimpleOS> # Asignar 800 bytes: First-Fit usa el primer hueco (ex-B)
SimpleOS> alloc 800

SimpleOS> mem
# [A:1000][OCUPADO:800][LIBRE:1200][C:1000][D:1500][LIBRE:2692]
# El hueco de 2000 se dividió en [800 ocupado][1200 libre]

SimpleOS> # Asignar 1000 bytes: First-Fit usa el segundo hueco
SimpleOS> alloc 1000

SimpleOS> mem
# [A:1000][OCUPADO:800][OCUPADO:1000][LIBRE:200][C:1000][D:1500][LIBRE:2692]
# El hueco de 1200 se dividió en [1000 ocupado][200 libre]
```

### Escenario 3: Manejo de falta de memoria

Demuestra el comportamiento cuando no hay memoria suficiente:

```bash
SimpleOS> # Llenar casi toda la memoria
SimpleOS> exec proceso_grande 7000
[MEMORY] Asignados 7000 bytes en dirección 0
[SCHEDULER] Proceso creado: proceso_grande (PID: 1, ...)

SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            7000        OCUPADO
7000         1192        LIBRE
----------------------------------------
Total: 8192 | Usado: 7000 | Libre: 1192

SimpleOS> # Intentar crear proceso que necesita más memoria de la disponible
SimpleOS> exec otro_proceso 2000
[MEMORY] Error: No hay espacio suficiente para 2000 bytes
[SCHEDULER] Error: No se pudo asignar memoria para el proceso otro_proceso (PID: 2)

SimpleOS> # El proceso no se crea si no hay memoria suficiente

SimpleOS> # Terminar proceso existente para liberar memoria
SimpleOS> kill 1
[SCHEDULER] Terminando proceso proceso_grande (PID: 1)
[MEMORY] Liberados 7000 bytes en dirección 0

SimpleOS> # Ahora sí hay memoria disponible
SimpleOS> exec otro_proceso 2000
[MEMORY] Asignados 2000 bytes en dirección 0
[SCHEDULER] Proceso creado: otro_proceso (PID: 2, Memoria: 2000 bytes en dirección 0)
```

### Escenario 4: Terminación ordenada con Ctrl+C

Demuestra el manejo de señales y limpieza de recursos:

```bash
SimpleOS> # Iniciar varios procesos de larga duración
SimpleOS> exec servidor_web 1500
SimpleOS> exec base_datos 2000
SimpleOS> exec monitor 800
SimpleOS> exec backup 1200

SimpleOS> ps
=== Estado de Procesos ===
Procesos en cola de listos: 0
Procesos en ejecución: 4

Procesos en ejecución:
PID    Nombre          Memoria    Dirección
------------------------------------------------
1      servidor_web    1500       0
2      base_datos      2000       1500
3      monitor         800        3500
4      backup          1200       4300

# Presionar Ctrl+C
^C
[SISTEMA] Señal recibida (2). Cerrando sistema...
[SHELL] Cerrando sistema operativo...
[SCHEDULER] Scheduler detenido
[PROCESO 1] servidor_web terminado después de 1823ms
[PROCESO 3] monitor terminado después de 1956ms
[MEMORY] Liberados 1500 bytes en dirección 0
[MEMORY] Liberados 2000 bytes en dirección 1500
[MEMORY] Liberados 800 bytes en dirección 3500
[PROCESO 2] base_datos terminado después de 2134ms
[PROCESO 4] backup terminado después de 2401ms
[MEMORY] Liberados 1200 bytes en dirección 4300
[SCHEDULER] Destruyendo planificador de procesos
[MEMORY] Destruyendo gestor de memoria
[MAIN] Sistema operativo terminado correctamente
```

### Escenario 5: Fusión automática de bloques libres

Demuestra la coalescencia automática de memoria:

```bash
SimpleOS> # Crear patrón: [A][B][C][D][E]
SimpleOS> alloc 500   # A en 0
SimpleOS> alloc 500   # B en 500
SimpleOS> alloc 500   # C en 1000
SimpleOS> alloc 500   # D en 1500
SimpleOS> alloc 500   # E en 2000

SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            500         OCUPADO
500          500         OCUPADO
1000         500         OCUPADO
1500         500         OCUPADO
2000         500         OCUPADO
2500         5692        LIBRE
----------------------------------------

SimpleOS> # Liberar bloques alternados: B y D
SimpleOS> free 500
[MEMORY] Liberados 500 bytes en dirección 500

SimpleOS> free 1500
[MEMORY] Liberados 500 bytes en dirección 1500

SimpleOS> mem
# Patrón: [A:OCUPADO][B:LIBRE][C:OCUPADO][D:LIBRE][E:OCUPADO][LIBRE]
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            500         OCUPADO
500          500         LIBRE
1000         500         OCUPADO
1500         500         LIBRE
2000         500         OCUPADO
2500         5692        LIBRE
----------------------------------------

SimpleOS> # Liberar C (entre dos bloques libres)
SimpleOS> free 1000
[MEMORY] Liberados 500 bytes en dirección 1000

SimpleOS> mem
# Los bloques B, C y D se fusionaron automáticamente en uno solo
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            500         OCUPADO
500          1500        LIBRE      <-- ¡Fusión de B+C+D!
2000         500         OCUPADO
2500         5692        LIBRE
----------------------------------------

SimpleOS> # Liberar E (adyacente al bloque libre final)
SimpleOS> free 2000
[MEMORY] Liberados 500 bytes en dirección 2000

SimpleOS> mem
# E se fusionó con el bloque libre final
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            500         OCUPADO
500          1500        LIBRE
2000         6192        LIBRE      <-- ¡Fusión de E + bloque final!
----------------------------------------
```

## 🛠️ Tecnologías y bibliotecas utilizadas

### Bibliotecas estándar de C++

```cpp
#include <thread>              // std::thread - Hilos del sistema operativo
#include <mutex>               // std::mutex - Exclusión mutua
#include <condition_variable>  // std::condition_variable - Sincronización
#include <atomic>              // std::atomic - Operaciones atómicas
#include <vector>              // std::vector - Contenedor dinámico
#include <queue>               // std::queue - Cola FIFO
#include <unordered_map>       // std::unordered_map - Tabla hash
#include <memory>              // std::unique_ptr, std::shared_ptr - Smart pointers
#include <chrono>              // std::chrono - Manejo de tiempo
#include <random>              // std::random_device, std::mt19937 - Números aleatorios
#include <algorithm>           // std::sort - Algoritmos STL
#include <iostream>            // std::cout, std::cin - Entrada/salida
#include <sstream>             // std::istringstream - Parsing de strings
#include <iomanip>             // std::setw - Formateo de salida
#include <csignal>             // signal() - Captura de señales
```

### Características de C++ utilizadas

- **C++17**: Estándar moderno con características avanzadas
- **RAII**: Gestión automática de recursos con destructores
- **Smart Pointers**: `unique_ptr` y `shared_ptr` para gestión de memoria segura
- **Move Semantics**: Transferencia eficiente de recursos
- **Lambda Expressions**: Para predicados en algoritmos
- **Thread-Safe Containers**: Protección con mutex para acceso concurrente

### Compilación

```bash
# Flags de compilación utilizados
-std=c++17        # Estándar C++17
-Wall             # Todas las advertencias
-Wextra           # Advertencias adicionales
-O2               # Optimización nivel 2
-pthread          # Soporte para hilos POSIX
```

## 🐛 Troubleshooting

### Problemas de compilación

**Error: `pthread` no encontrado**
```bash
# Solución: Instalar build-essential
sudo apt update
sudo apt install build-essential

# Verificar instalación
g++ --version
```

**Error: C++17 no soportado**
```bash
# Verificar versión del compilador (debe ser 7.0+)
g++ --version

# Si es antigua, actualizar
sudo apt install g++-9
# O
sudo apt install g++-10

# Usar el compilador actualizado
g++-10 -std=c++17 -Wall -Wextra -O2 -pthread \
    main.cpp MemoryManager.cpp ProcessScheduler.cpp Shell.cpp \
    -o os_sim
```

**Warnings sobre parámetros no usados**
```bash
# Son normales y no afectan el funcionamiento
# Para eliminarlos, reemplazar en Shell.cpp:
void cmd_ps(const std::vector<std::string>& args)
# Por:
void cmd_ps(const std::vector<std::string>& /*args*/)
```

### Problemas de ejecución

**Síntoma: Procesos no aparecen ejecutándose**
```bash
# Verificar que aparezca el mensaje:
[SCHEDULER] Scheduler iniciado

# Si no aparece, verificar compilación sin errores
make clean
make
```

**Síntoma: Segmentation fault al ejecutar**
```bash
# Compilar en modo debug
g++ -g -std=c++17 -pthread main.cpp MemoryManager.cpp \
    ProcessScheduler.cpp Shell.cpp -o os_sim_debug

# Ejecutar con gdb
gdb ./os_sim_debug
(gdb) run
# Cuando falle:
(gdb) backtrace
```

**Síntoma: Los procesos no terminan**
```bash
# Es normal - los procesos duran entre 1-5 segundos
# Ver ProcessScheduler.cpp línea 131:
std::uniform_int_distribution<> dis(1000, 5000);

# Para procesos más rápidos, cambiar a:
std::uniform_int_distribution<> dis(500, 2000);
```

**Síntoma: Error de memoria al crear muchos procesos**
```bash
# Aumentar memoria disponible en main.cpp:
const size_t TOTAL_MEMORY = 16384;  // En lugar de 8192

# O liberar memoria de procesos terminados manualmente:
SimpleOS> kill <pid>
```

### Comportamiento esperado

**Los mensajes se imprimen desordenados**
```
✅ Esto es CORRECTO
Los procesos ejecutan en hilos reales y compiten por std::cout
El orden impredecible demuestra verdadera concurrencia
```

**El comando tarda en responder**
```
✅ Esto es NORMAL
El scheduler puede estar ejecutando procesos
Los mutex protegen estructuras compartidas (puede haber espera)
```

**Al salir con exit, tarda unos segundos**
```
✅ Esto es ESPERADO
El sistema espera a que todos los procesos terminen ordenadamente
Los hilos hacen join() antes de liberar recursos
```

## 📊 Limitaciones conocidas

1. **Memoria simulada**: Máximo 8192 bytes por defecto (configurable)
2. **No hay persistencia**: El estado se pierde al salir
3. **Simulación de trabajo**: Los procesos usan `sleep()` en lugar de trabajo real de CPU
4. **Sin prioridades**: FCFS simple sin niveles de prioridad
5. **Sin sistema de archivos**: No implementado en esta versión
6. **Sin memoria virtual**: Solo memoria física contigua
7. **Sin paginación**: Asignación contigua únicamente

## 🚀 Posibles extensiones futuras

- **Algoritmo Round-Robin**: Planificador apropiativo con quantum de tiempo
- **Prioridades de procesos**: Scheduling con múltiples niveles
- **Sistema de archivos**: Implementación básica con inodos
- **Memoria virtual**: Simulación de paginación y TLB
- **Sincronización avanzada**: Semáforos, barreras, variables de condición múltiples
- **Interfaz gráfica**: Visualización en tiempo real con ncurses
- **Estadísticas**: Métricas de rendimiento del sistema
- **Persistencia**: Guardar estado en archivo JSON

## 📝 Créditos y licencia

**Desarrollado como proyecto educativo** para la comprensión práctica de conceptos de Sistemas Operativos.

**Autor**: Proyecto académico  
**Propósito**: Educativo - Simulación de conceptos de SO  
**Lenguaje**: C++17  
**Entorno**: WSL/Linux  

---

**Simple OS Simulator** - Un proyecto completo que implementa los componentes fundamentales de un sistema operativo real usando C++ moderno, hilos reales del sistema operativo y sincronización thread-safe. Ideal para estudiantes de sistemas operativos que buscan entender cómo funcionan internamente la gestión de memoria, planificación de procesos y sincronización mediante código real y funcional.
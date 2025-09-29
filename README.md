# Simple OS Simulator

**Simple OS Simulator** es un prototipo completo de Sistema Operativo desarrollado en C++ para entornos WSL/Linux. Este proyecto proporciona una implementación práctica e interactiva de los componentes fundamentales de un sistema operativo moderno, incluyendo gestión de memoria, planificación de procesos, sincronización y una interfaz de línea de comandos completa.

## Tabla de Contenidos

- [Características principales](#características-principales)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Conceptos teóricos implementados](#conceptos-teóricos-implementados)
- [Arquitectura del sistema](#arquitectura-del-sistema)
- [Componentes del simulador](#componentes-del-simulador)
- [Instalación y configuración](#instalación-y-configuración)
- [Manual de comandos](#manual-de-comandos)
- [Ejemplos de uso](#ejemplos-de-uso)
- [Escenarios de demostración](#escenarios-de-demostración)
- [Troubleshooting](#troubleshooting)

## Características principales

- **Shell interactivo completo**: Interfaz de línea de comandos intuitiva con validación de entrada, manejo de errores y comandos organizados por categorías funcionales
- **Gestión avanzada de procesos**: Sistema completo de creación, monitoreo y terminación de procesos con estados bien definidos y ciclo de vida completo
- **Planificador FCFS robusto**: Algoritmo First-Come, First-Served con soporte para multithreading real usando `std::thread`
- **Gestor de memoria First-Fit**: Sistema de gestión de memoria contigua con fusión automática de bloques libres y prevención de fragmentación
- **Sincronización thread-safe**: Implementación robusta usando `std::mutex` y `std::condition_variable` para protección de recursos críticos
- **Arquitectura modular**: Componentes independientes y reutilizables con interfaces bien definidas
- **Manejo de señales**: Captura de Ctrl+C para terminación ordenada del sistema
- **Compilación multiplataforma**: Compatible con g++, clang++ y compiladores modernos de C++

## Estructura del proyecto

```
Simple-OS-Simulator/
├── src/                          # Código fuente principal
│   ├── main.cpp                  # Punto de entrada del simulador
│   ├── MemoryManager.h           # Declaración del gestor de memoria
│   ├── MemoryManager.cpp         # Implementación del gestor de memoria
│   ├── ProcessScheduler.h        # Declaración del planificador de procesos
│   ├── ProcessScheduler.cpp      # Implementación del planificador FCFS
│   ├── Shell.h                   # Declaración del shell interactivo
│   └── Shell.cpp                 # Implementación del intérprete de comandos
├── build/                        # Archivos de compilación (generado)
│   └── *.o                       # Archivos objeto compilados
├── docs/                         # Documentación del proyecto
│   ├── arquitectura.md           # Documentación de la arquitectura
│   └── diagramas/                # Diagramas del sistema
├── scripts/                      # Scripts de utilidad
│   ├── compile.sh                # Script de compilación automática
│   └── run_tests.sh              # Script de pruebas automatizadas
├── Makefile                      # Sistema de compilación make
├── README.md                     # Documentación principal
└── .gitignore                    # Configuración de Git
```

## Conceptos teóricos implementados

### Planificación de procesos (FCFS - First-Come, First-Served)

Algoritmo de planificación no apropiativo donde los procesos se ejecutan en orden de llegada:

- **Simplicidad**: Implementación directa usando una cola FIFO
- **Equidad temporal**: Los procesos que llegan primero se ejecutan primero
- **No preemption**: Los procesos ejecutan hasta completarse
- **Multithreading real**: Cada proceso ejecuta en su propio `std::thread`

**Algoritmo de planificación:**
```cpp
while (!cola_procesos.empty()) {
    proceso = cola_procesos.front();
    cola_procesos.pop();
    ejecutar_proceso(proceso);  // En hilo separado
}
```

### Gestión de memoria (First-Fit)

Sistema de asignación de memoria contigua optimizado para velocidad:

- **Algoritmo rápido**: Selecciona el primer bloque libre suficientemente grande
- **Gestión de fragmentación**: Fusión automática de bloques libres adyacentes
- **Seguimiento preciso**: Registro detallado de todos los bloques de memoria
- **Visualización completa**: Mapas visuales del estado actual de la memoria

**Estados de la memoria:**
```cpp
enum BlockState {
    FREE,     // Bloque libre disponible
    OCCUPIED  // Bloque asignado a un proceso
};
```

### Sincronización (Mutex y Condition Variables)

Primitivas de sincronización thread-safe para proteger recursos críticos:

- **Exclusión mutua garantizada**: Solo un hilo puede acceder al recurso protegido
- **Prevención de condiciones de carrera**: Acceso serializado a estructuras compartidas
- **Variables de condición**: Sincronización eficiente entre hilos del scheduler
- **RAII**: Gestión automática de locks usando `std::lock_guard`

##  Arquitectura del sistema

El simulador sigue una **arquitectura por capas** donde cada componente tiene responsabilidades específicas y bien definidas:

```
┌─────────────────────────────────────────────┐
│             Capa de Presentación            │
│    ┌─────────────────────────────────┐      │
│    │       Shell Interactivo         │      │
│    │   (Interfaz de Usuario)         │      │
│    └─────────────────────────────────┘      │
└─────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────┐
│            Capa de Gestión                  │
│  ┌─────────────────┐ ┌─────────────────┐    │
│  │   Planificador  │ │   Gestión de    │    │
│  │      FCFS       │ │    Procesos     │    │
│  └─────────────────┘ └─────────────────┘    │
└─────────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────┐
│       Capa de Recursos del Sistema          │
│ ┌─────────────┐ ┌─────────────┐ ┌─────────┐ │
│ │   Gestor    │ │ Sincroniza- │ │ Sistema │ │
│ │    de       │ │    ción     │ │   de    │ │
│ │  Memoria    │ │  (Mutex)    │ │Señales  │ │
│ └─────────────┘ └─────────────┘ └─────────┘ │
└─────────────────────────────────────────────┘
```

### Flujo de ejecución de comandos

El siguiente diagrama muestra la **interacción entre componentes** durante la creación y ejecución de un proceso:

```
Usuario → Shell → Planificador → GestorMemoria → Proceso → Sincronización
   │        │          │             │            │            │
   │        │          │             │            │            ▼
   │        │          │             │            │      Mutex acquired
   │        │          │             │            ▼            │
   │        │          │             │    std::thread created  │
   │        │          │             ▼            │            │
   │        │          │      Memory allocated    │            │
   │        │          ▼             │            │            │
   │        │    Process queued      │            │            │
   │        ▼          │             │            │            │
   │   Command parsed  │             │            │            │
   ▼        │          │             │            │            │
Input       │          ▼             ▼            ▼            ▼
received    ▼    Scheduler loop  →  Execution  →  Completion → Cleanup
```

## 🔧 Componentes del simulador

### Shell (Shell.h/cpp)

**Funcionalidad**: Interfaz de usuario basada en texto que implementa un intérprete de comandos completo.

**Características principales**:
- Análisis y validación de comandos con manejo robusto de errores
- Más de 10 comandos organizados en categorías funcionales
- Mensajes informativos y de error detallados
- Integración seamless con todos los subsistemas del OS

**Comandos soportados**: Gestión de memoria, procesos, visualización y control del sistema

### Planificador (ProcessScheduler.h/cpp)

**Funcionalidad**: Implementa el algoritmo FCFS con soporte completo para multithreading.

**Parámetros configurables**:
- **Cola de procesos**: Estructura `std::queue` con acceso thread-safe
- **Gestión de hilos**: Un `std::thread` por proceso con sincronización
- **Estados de proceso**: NEW, READY, RUNNING, TERMINATED

**Algoritmo de planificación**:
```cpp
void scheduler_loop() {
    while (scheduler_running) {
        if (!ready_queue.empty()) {
            auto process = ready_queue.front();
            ready_queue.pop();
            execute_process(process);  // En hilo separado
        }
        cleanup_finished_processes();
    }
}
```

### Gestor de memoria (MemoryManager.h/cpp)

**Funcionalidad**: Administra un espacio de memoria simulado usando el algoritmo First-Fit.

**Configuración**:
- **Tamaño total**: 8192 bytes (configurable en main.cpp)
- **Unidad mínima**: 1 byte
- **Algoritmo**: First-Fit con fusión automática de bloques libres

**Estados de la memoria**:
```cpp
struct Block {
    size_t size;        // Tamaño del bloque
    bool is_free;       // Estado: true=libre, false=ocupado
    size_t start_addr;  // Dirección de inicio simulada
};
```

### Gestión de procesos (ProcessScheduler.h/cpp)

**Funcionalidad**: Define la estructura y comportamiento de los procesos simulados.

**Atributos del proceso**:
```cpp
struct Process {
    int pid;                    // Identificador único
    std::string name;           // Nombre del proceso
    size_t memory_required;     // Memoria necesaria
    size_t memory_address;      // Dirección base asignada
    std::unique_ptr<std::thread> thread_ptr;  // Hilo del proceso
};
```

**Estados del proceso**:
- `NEW`: Proceso creado, esperando memoria
- `READY`: En cola de listos para ejecución
- `RUNNING`: Ejecutándose en CPU (hilo activo)
- `TERMINATED`: Proceso completado y limpiado

### Sincronización (Implementada en todos los componentes)

**Funcionalidad**: Proporciona mecanismos thread-safe usando primitivas estándar de C++.

**Componentes principales**:
- **std::mutex**: Para exclusión mutua en acceso a estructuras compartidas
- **std::condition_variable**: Para comunicación eficiente entre hilos
- **std::atomic**: Para contadores thread-safe (PIDs)
- **std::lock_guard**: RAII para gestión automática de locks

## ⚙️ Instalación y configuración

### Prerrequisitos

- **Sistema Operativo**: WSL Ubuntu, Linux, o cualquier distribución compatible
- **Compilador**: g++ 7.0+ o clang++ con soporte completo para C++17
- **Bibliotecas**: pthread (incluida en la mayoría de sistemas Linux)
- **Herramientas**: make (opcional pero recomendado)

### Pasos de instalación

1. **Clonar o descargar el proyecto**:
```bash
# Crear directorio y copiar archivos
mkdir simple-os-simulator
cd simple-os-simulator
# Copiar todos los archivos .h, .cpp, Makefile
```

2. **Instalar dependencias** (Ubuntu/WSL):
```bash
# Actualizar repositorios
sudo apt update

# Instalar herramientas de desarrollo
sudo apt install build-essential g++ make

# Verificar instalación
g++ --version
make --version
```

3. **Compilar el proyecto**:
```bash
# Opción 1: Usando Makefile (recomendado)
make clean
make

# Opción 2: Compilación directa
g++ -std=c++17 -Wall -Wextra -O2 -pthread \
    main.cpp MemoryManager.cpp ProcessScheduler.cpp Shell.cpp \
    -o os_sim

# Opción 3: Script de compilación
chmod +x compile.sh
./compile.sh
```

4. **Verificar la instalación**:
```bash
./os_sim
# Debe mostrar el splash screen del sistema
```

### Configuración inicial

Al ejecutar por primera vez, el simulador:

1. **Inicializa el gestor de memoria** con 8192 bytes disponibles
2. **Configura el planificador FCFS** con cola de procesos vacía
3. **Establece el shell interactivo** con todos los comandos disponibles
4. **Configura la captura de señales** para terminación ordenada

## 📖 Manual de comandos

### Gestión de procesos

| Comando | Sintaxis | Descripción | Ejemplo |
|---------|----------|-------------|---------|
| `exec` | `exec <nombre> <memoria>` | Crea y ejecuta un nuevo proceso con memoria especificada | `exec editor 512` |
| `ps` | `ps` | Lista todos los procesos activos con su estado actual | `ps` |
| `kill` | `kill <pid>` | Termina forzosamente un proceso específico por PID | `kill 1` |

### Gestión de memoria

| Comando | Sintaxis | Descripción | Ejemplo |
|---------|----------|-------------|---------|
| `alloc` | `alloc <tamaño>` | Asigna un bloque de memoria del tamaño especificado | `alloc 1024` |
| `free` | `free <dirección>` | Libera el bloque de memoria en la dirección especificada | `free 0` |
| `mem` | `mem` | Muestra el mapa completo del estado actual de la memoria | `mem` |

### Comandos del sistema

| Comando | Sintaxis | Descripción | Ejemplo |
|---------|----------|-------------|---------|
| `help` | `help` | Muestra la ayuda completa con todos los comandos disponibles | `help` |
| `clear` | `clear` | Limpia la pantalla del terminal | `clear` |
| `exit` | `exit` o `quit` | Sale del simulador de forma ordenada | `exit` |

## 🎯 Ejemplos de uso

### Ejemplo básico: Gestión de procesos

```bash
SimpleOS> # Crear varios procesos con diferentes requerimientos de memoria
SimpleOS> exec navegador 2048
[SCHEDULER] Proceso creado: navegador (PID: 1, Memoria: 2048 bytes en dirección 0)
[SCHEDULER] Ejecutando proceso navegador (PID: 1)

SimpleOS> exec editor 1024
[SCHEDULER] Proceso creado: editor (PID: 2, Memoria: 1024 bytes en dirección 2048)

SimpleOS> exec calculadora 512
[SCHEDULER] Proceso creado: calculadora (PID: 3, Memoria: 512 bytes en dirección 3072)

SimpleOS> # Ver el estado de todos los procesos
SimpleOS> ps
=== Estado de Procesos ===
Procesos en cola de listos: 0
Procesos en ejecución: 3

Procesos en ejecución:
PID    Nombre        Memoria    Dirección
------------------------------------------------
1      navegador     2048       0
2      editor        1024       2048
3      calculadora   512        3072

SimpleOS> # Ver el estado de la memoria
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            2048        OCUPADO
2048         1024        OCUPADO
3072         512         OCUPADO
3584         4608        LIBRE
----------------------------------------
Total: 8192 | Usado: 3584 | Libre: 4608
```

### Ejemplo intermedio: Gestión de memoria

```bash
SimpleOS> # Asignar bloques de memoria manualmente
SimpleOS> alloc 1500
[MEMORY] Asignados 1500 bytes en dirección 0

SimpleOS> alloc 800
[MEMORY] Asignados 800 bytes en dirección 1500

SimpleOS> alloc 2000
[MEMORY] Asignados 2000 bytes en dirección 2300

SimpleOS> # Ver el estado de la memoria fragmentada
SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            1500        OCUPADO
1500         800         OCUPADO
2300         2000        OCUPADO
4300         3892        LIBRE
----------------------------------------
Total: 8192 | Usado: 4300 | Libre: 3892

SimpleOS> # Liberar el bloque del medio para crear fragmentación
SimpleOS> free 1500
[MEMORY] Liberados 800 bytes en dirección 1500

SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            1500        OCUPADO
1500         800         LIBRE
2300         2000        OCUPADO
4300         3892        LIBRE
----------------------------------------
Total: 8192 | Usado: 3500 | Libre: 4692

SimpleOS> # Intentar asignar un bloque que quepa en el hueco
SimpleOS> alloc 600
[MEMORY] Asignados 600 bytes en dirección 1500

SimpleOS> # Ver cómo se reutilizó el espacio fragmentado
SimpleOS> mem
```

### Ejemplo avanzado: Ciclo completo del sistema

```bash
SimpleOS> # Demostración completa del sistema operativo
SimpleOS> help
=== COMANDOS DISPONIBLES ===
alloc <tamaño>           Asignar memoria
exec <nombre> <memoria>  Crear y ejecutar proceso
free <dirección>         Liberar bloque de memoria
ps                       Mostrar procesos en ejecución
mem                      Mostrar estado de memoria
kill <pid>               Terminar proceso
clear                    Limpiar pantalla
help                     Mostrar esta ayuda
exit/quit               Salir del sistema

SimpleOS> # Crear un proceso de larga duración
SimpleOS> exec servidor_web 3000
[SCHEDULER] Proceso creado: servidor_web (PID: 1, Memoria: 3000 bytes en dirección 0)
[SCHEDULER] Ejecutando proceso servidor_web (PID: 1)
[PROCESO 1] Iniciando ejecución de servidor_web

SimpleOS> # Crear procesos cliente mientras el servidor ejecuta
SimpleOS> exec cliente1 500
SimpleOS> exec cliente2 750
SimpleOS> exec cliente3 400

SimpleOS> # Monitorear el sistema
SimpleOS> ps
SimpleOS> mem

SimpleOS> # Simular terminación de algunos procesos
SimpleOS> kill 2
[SCHEDULER] Terminando proceso cliente1 (PID: 2)

SimpleOS> # Ver cómo se libera la memoria automáticamente
SimpleOS> mem

SimpleOS> # Salir del sistema ordenadamente
SimpleOS> exit
[SHELL] Cerrando sistema operativo...
[SCHEDULER] Scheduler detenido
[MEMORY] Destruyendo gestor de memoria
[SISTEMA] Sistema operativo terminado correctamente
```

## 🧪 Escenarios de demostración

### Escenario 1: Multithreading y concurrencia

Este escenario demuestra cómo el sistema maneja múltiples procesos concurrentes:

```bash
SimpleOS> # Crear múltiples procesos que ejecuten simultáneamente
SimpleOS> exec proceso_intensivo_1 1000
SimpleOS> exec proceso_intensivo_2 1200
SimpleOS> exec proceso_intensivo_3 800
SimpleOS> exec monitor_sistema 400

SimpleOS> ps
# Observar cómo todos los procesos aparecen como RUNNING
# Cada uno ejecuta en su propio hilo del sistema operativo

# Mientras ejecutan, el sistema muestra mensajes como:
# [PROCESO 1] proceso_intensivo_1 trabajando... (Memoria: 0)
# [PROCESO 2] proceso_intensivo_2 trabajando... (Memoria: 1000)
# [PROCESO 3] proceso_intensivo_3 trabajando... (Memoria: 2200)
# [PROCESO 4] monitor_sistema trabajando... (Memoria: 3000)
```

### Escenario 2: Fragmentación y coalescencia de memoria

Demuestra el algoritmo First-Fit y la fusión automática de bloques:

```bash
SimpleOS> # Crear fragmentación controlada
SimpleOS> alloc 1000    # Bloque A: 0-999
SimpleOS> alloc 500     # Bloque B: 1000-1499  
SimpleOS> alloc 800     # Bloque C: 1500-2299
SimpleOS> alloc 1200    # Bloque D: 2300-3499

SimpleOS> mem
# Mostrar memoria completamente asignada

SimpleOS> # Liberar bloques alternados para crear huecos
SimpleOS> free 1000     # Liberar bloque B
SimpleOS> free 2300     # Liberar bloque D

SimpleOS> mem
# Mostrar fragmentación: [OCUPADO][LIBRE][OCUPADO][LIBRE][LIBRE]

SimpleOS> # Intentar asignar bloque grande
SimpleOS> alloc 2000
# Fallará porque no hay bloque contiguo suficientemente grande

SimpleOS> # Liberar bloque C para permitir fusión
SimpleOS> free 1500

SimpleOS> mem
# Mostrar fusión automática: bloques libres se combinan
# [OCUPADO][---LIBRE GRANDE---][LIBRE]

SimpleOS> # Ahora el bloque grande sí se puede asignar
SimpleOS> alloc 2000
[MEMORY] Asignados 2000 bytes en dirección 1000

SimpleOS> mem
# Mostrar cómo se reutilizó el espacio fusionado
```

### Escenario 3: Manejo de señales y terminación ordenada

Demuestra la captura de señales y limpieza de recursos:

```bash
SimpleOS> # Iniciar varios procesos de larga duración
SimpleOS> exec backup_sistema 2000
SimpleOS> exec indexador_archivos 1500
SimpleOS> exec monitor_red 800

SimpleOS> ps
# Ver múltiples procesos ejecutando

# Presionar Ctrl+C
^C
[SISTEMA] Señal recibida (2). Cerrando sistema...
[SHELL] Cerrando sistema operativo...
[SCHEDULER] Terminando proceso backup_sistema (PID: 1)
[SCHEDULER] Terminando proceso indexador_archivos (PID: 2)  
[SCHEDULER] Terminando proceso monitor_red (PID: 3)
[MEMORY] Liberados 2000 bytes en dirección 0
[MEMORY] Liberados 1500 bytes en dirección 2000
[MEMORY] Liberados 800 bytes en dirección 3500
[SCHEDULER] Scheduler detenido
[MEMORY] Destruyendo gestor de memoria
[MAIN] Sistema operativo terminado correctamente
```

## 🛠️ Troubleshooting

### Problemas de compilación

**Error**: `pthread` no encontrado
```bash
# Solución: Instalar build-essential
sudo apt install build-essential

# O especificar la biblioteca manualmente
g++ ... -pthread -lpthread
```

**Error**: `C++17` no soportado
```bash
# Verificar versión del compilador
g++ --version

# Actualizar si es necesario
sudo apt install g++-9 g++-10
```

### Problemas de ejecución

**Síntoma**: Los procesos no aparecen como ejecutándose
```bash
# Causa probable: Scheduler no iniciado
# Verificar que aparezca: [SCHEDULER] Scheduler iniciado
# Si no aparece, revisar que no haya errores de compilación
```

**Síntoma**: Memory leaks o crashes
```bash
# Ejecutar con debugging
g++ -g -DDEBUG ... -o os_sim_debug
gdb ./os_sim_debug

# O usar valgrind si está disponible
valgrind --leak-check=full ./os_sim
```

**Síntoma**: Comandos no reconocidos
```bash
# Verificar sintaxis exacta en help
SimpleOS> help

# Los comandos son case-sensitive
# Usar exactamente: exec, alloc, free, ps, mem, kill, help, clear, exit
```

### Limitaciones conocidas

1. **Memoria simulada**: Limitada a 8192 bytes por defecto (configurable en main.cpp)
2. **Procesos**: No hay límite teórico, pero depende de la memoria del sistema host
3. **Tiempo real**: Los procesos simulan trabajo con sleep(), no trabajo real de CPU
4. **Persistencia**: El estado del sistema se pierde al salir (no hay guardado en disco)

---

**Simple OS Simulator** - Un proyecto educativo completo para el aprendizaje profundo de sistemas operativos, desarrollado con estándares profesionales de C++ moderno. Ideal para estudiantes de ciencias de la computación, ingeniería de sistemas y profesionales que buscan comprender la implementación práctica de los conceptos fundamentales de los sistemas operativos.
# CLI-con-wsl
hacer un prototipo de CLI
# Simple OS Simulator

Un prototipo simple de Sistema Operativo implementado en C++ para entornos WSL/Linux. Este proyecto simula los componentes básicos de un sistema operativo incluyendo gestión de memoria, planificación de procesos y una interfaz de línea de comandos.

## 🏗️ Arquitectura del Sistema

### Componentes Principales

1. **MemoryManager** - Gestión de Memoria
   - Implementa algoritmo **First-Fit** para asignación de memoria
   - Simula memoria física como vector de bloques
   - Soporte para fusión automática de bloques libres adyacentes
   - Thread-safe usando mutex

2. **ProcessScheduler** - Planificador de Procesos
   - Algoritmo **FCFS** (First-Come, First-Served)
   - Cada proceso ejecuta en su propio hilo (std::thread)
   - Sincronización thread-safe con mutex y condition_variable
   - Gestión automática del ciclo de vida de procesos

3. **Shell** - Interfaz de Línea de Comandos
   - Intérprete de comandos interactivo
   - Comandos para gestión de memoria y procesos
   - Manejo de errores y validación de entrada

## 🔧 Requisitos del Sistema

- **SO**: WSL Ubuntu, Linux, o cualquier distribución compatible
- **Compilador**: g++ con soporte C++17
- **Bibliotecas**: pthread (generalmente incluida)
- **Herramientas**: make (opcional pero recomendado)

## 📦 Instalación

### 1. Clonar/Descargar los archivos

Asegúrate de tener todos estos archivos en el mismo directorio:
```
simple-os/
├── main.cpp
├── MemoryManager.h
├── MemoryManager.cpp
├── ProcessScheduler.h
├── ProcessScheduler.cpp
├── Shell.h
├── Shell.cpp
├── Makefile
└── README.md
```

### 2. Instalar dependencias (Ubuntu/WSL)

```bash
# Actualizar paquetes
sudo apt update

# Instalar herramientas de desarrollo
sudo apt install build-essential g++ make

# O usar el Makefile
make install-deps
```

### 3. Compilar el proyecto

```bash
# Opción 1: Usando Makefile (recomendado)
make

# Opción 2: Compilación manual
g++ main.cpp MemoryManager.cpp ProcessScheduler.cpp Shell.cpp -lpthread -o os_sim -std=c++17 -O2

# Opción 3: Compilación en modo debug
make debug
```

### 4. Ejecutar

```bash
./os_sim
```

## 🖥️ Uso del Sistema

### Comandos Disponibles

| Comando | Descripción | Ejemplo |
|---------|-------------|---------|
| `alloc <tamaño>` | Asignar memoria | `alloc 1024` |
| `exec <nombre> <memoria>` | Crear proceso | `exec editor 512` |
| `free <dirección>` | Liberar memoria | `free 0` |
| `ps` | Mostrar procesos | `ps` |
| `mem` | Estado de memoria | `mem` |
| `kill <pid>` | Terminar proceso | `kill 1` |
| `clear` | Limpiar pantalla | `clear` |
| `help` | Mostrar ayuda | `help` |
| `exit` o `quit` | Salir | `exit` |

### Ejemplo de Sesión

```bash
SimpleOS> help
=== COMANDOS DISPONIBLES ===
alloc <tamaño>           Asignar memoria
exec <nombre> <memoria>  Crear y ejecutar proceso
...

SimpleOS> mem
=== Estado de la Memoria ===
Dirección    Tamaño      Estado
----------------------------------------
0            8192        LIBRE
----------------------------------------
Total: 8192 | Usado: 0 | Libre: 8192

SimpleOS> alloc 1024
[MEMORY] Asignados 1024 bytes en dirección 0

SimpleOS> exec calculadora 512
[SCHEDULER] Proceso creado: calculadora (PID: 1, Memoria: 512 bytes en dirección 1024)
[SCHEDULER] Ejecutando proceso calculadora (PID: 1)
[PROCESO 1] Iniciando ejecución de calculadora

SimpleOS> ps
=== Estado de Procesos ===
Procesos en cola de listos: 0
Procesos en ejecución: 1

Procesos en ejecución:
PID    Nombre        Memoria    Dirección
------------------------------------------------
1      calculadora   512        1024
```

## 🔍 Detalles Técnicos

### Algoritmos Implementados

**Gestión de Memoria - First-Fit:**
- Busca el primer bloque libre que sea suficientemente grande
- Divide bloques cuando es necesario
- Fusiona bloques libres adyacentes automáticamente

**Planificación de Procesos - FCFS:**
- Los procesos se ejecutan en orden de llegada
- No hay preemption (no expulsivo)
- Cada proceso tiene tiempo de ejecución aleatorio (1-5 segundos)

### Estructuras de Datos

```cpp
// Bloque de memoria
struct Block {
    size_t size;        // Tamaño del bloque
    bool is_free;       // Estado (libre/ocupado)
    size_t start_addr;  // Dirección de inicio
};

// Proceso
struct Process {
    int pid;                    // ID del proceso
    std::string name;           // Nombre del proceso
    size_t memory_required;     // Memoria requerida
    size_t memory_address;      // Dirección asignada
    std::unique_ptr<std::thread> thread_ptr; // Hilo del proceso
};
```

### Bibliotecas Utilizadas

- **`<thread>`** - Para manejo de hilos (procesos)
- **`<mutex>`** - Para sincronización thread-safe
- **`<condition_variable>`** - Para comunicación entre hilos
- **`<atomic>`** - Para operaciones atómicas (PIDs)
- **`<vector>`** - Para almacenar bloques de memoria
- **`<queue>`** - Para la cola de procesos listos
- **`<unordered_map>`** - Para almacenar procesos en ejecución

## 🚀 Características Avanzadas

### Thread Safety
- Todos los componentes son thread-safe
- Uso de mutex para proteger estructuras compartidas
- Operaciones atómicas para contadores

### Manejo de Señales
- Captura Ctrl+C para terminación ordenada
- Limpieza automática de recursos

### Gestión de Errores
- Validación de entrada en todos los comandos
- Manejo de excepciones
- Mensajes informativos para el usuario

## 🧪 Pruebas Sugeridas

1. **Fragmentación de Memoria:**
   ```bash
   alloc 1000
   alloc 2000
   alloc 1000
   free 1000    # Liberar el bloque del medio
   alloc 500    # Debería usar el hueco creado
   ```

2. **Múltiples Procesos:**
   ```bash
   exec proceso1 512
   exec proceso2 256
   exec proceso3 1024
   ps           # Ver todos ejecutándose
   ```

3. **Agotamiento de Memoria:**
   ```bash
   alloc 4000
   alloc 4000
   alloc 1000   # Debería fallar
   ```

## 🔧 Personalización

### Modificar Tamaño de Memoria
En `main.cpp`, cambiar:
```cpp
const size_t TOTAL_MEMORY = 8192; // Cambiar por el valor deseado
```

### Cambiar Tiempo de Ejecución de Procesos
En `ProcessScheduler.cpp`, función `process_execution()`:
```cpp
std::uniform_int_distribution<> dis(1000, 5000); // min, max en ms
```

### Agregar Nuevos Comandos
1. Declarar función en `Shell.h`
2. Implementar en `Shell.cpp`
3. Agregar caso en `process_command()`

## 🐛 Troubleshooting

### Error de Compilación
```bash
# Verificar g++ instalado
g++ --version

# Verificar dependencias
make check

# Limpiar y recompilar
make clean
make
```

### El programa no responde
- Usar Ctrl+C para terminar ordenadamente
- Los procesos pueden tardar hasta 5 segundos en terminar

### Problemas de memoria
- El simulador usa memoria real para sus estructuras
- Para procesos muy grandes, considerar aumentar límites del sistema

## 📚 Conceptos de SO Implementados

- **Gestión de Memoria**: First-Fit, fragmentación, coalescencia
- **Planificación**: FCFS, colas de procesos
- **Concurrencia**: Hilos, mutex, variables de condición
- **Sincronización**: Acceso exclusivo a recursos compartidos
- **Interfaz de Usuario**: Shell, parsing de comandos

## 🎯 Posibles Extensiones

- Implementar otros algoritmos (Best-Fit, Worst-Fit)
- Agregar planificación Round-Robin
- Simular E/O con archivos
- Implementar memoria virtual
- Agregar sistema de archivos simple
- Interfaz gráfica con ncurses

---

**Desarrollado como prototipo educativo para entender conceptos de Sistemas Operativos**
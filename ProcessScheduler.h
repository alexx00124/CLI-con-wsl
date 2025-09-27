#ifndef PROCESS_SCHEDULER_H
#define PROCESS_SCHEDULER_H

#include "MemoryManager.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>
#include <memory>
#include <unordered_map>

// Estructura que representa un proceso
struct Process {
    int pid;                    // ID del proceso
    std::string name;           // Nombre del proceso
    size_t memory_required;     // Memoria requerida
    size_t memory_address;      // Dirección de memoria asignada
    std::unique_ptr<std::thread> thread_ptr;  // Puntero al hilo del proceso
    
    Process(int p, const std::string& n, size_t mem) 
        : pid(p), name(n), memory_required(mem), memory_address(0) {}
};

class ProcessScheduler {
private:
    MemoryManager& memory_manager;              // Referencia al gestor de memoria
    std::queue<std::shared_ptr<Process>> ready_queue;  // Cola de procesos listos (FCFS)
    std::unordered_map<int, std::shared_ptr<Process>> running_processes; // Procesos en ejecución
    
    mutable std::mutex scheduler_mutex;         // Mutex para acceso thread-safe al scheduler (mutable para const functions)
    std::condition_variable cv;                 // Variable de condición para sincronización
    std::atomic<int> next_pid;                  // Contador atómico para PIDs
    std::atomic<bool> scheduler_running;        // Flag para controlar el scheduler
    
    std::thread scheduler_thread;               // Hilo principal del scheduler

public:
    // Constructor
    ProcessScheduler(MemoryManager& mm);
    
    // Destructor
    ~ProcessScheduler();
    
    // Crea un nuevo proceso y lo añade a la cola de listos
    int crear_proceso(const std::string& name, size_t memory_required);
    
    // Inicia el scheduler
    void start_scheduler();
    
    // Para el scheduler
    void stop_scheduler();
    
    // Muestra información de los procesos
    void display_processes() const;
    
    // Termina un proceso por PID
    bool terminate_process(int pid);

private:
    // Función principal del scheduler (ejecuta algoritmo FCFS)
    void scheduler_loop();
    
    // Ejecuta un proceso específico
    void execute_process(std::shared_ptr<Process> process);
    
    // Simula la ejecución de un proceso
    void process_execution(std::shared_ptr<Process> process);
    
    // Limpia procesos terminados
    void cleanup_finished_processes();
};

#endif // PROCESS_SCHEDULER_H
#include "ProcessScheduler.h"
#include <iostream>
#include <chrono>
#include <random>

ProcessScheduler::ProcessScheduler(MemoryManager& mm) 
    : memory_manager(mm), next_pid(1), scheduler_running(false) {
    std::cout << "[SCHEDULER] Inicializando planificador de procesos\n";
}

ProcessScheduler::~ProcessScheduler() {
    stop_scheduler();
    std::cout << "[SCHEDULER] Destruyendo planificador de procesos\n";
}

// Crea un nuevo proceso y lo añade a la cola de listos
int ProcessScheduler::crear_proceso(const std::string& name, size_t memory_required) {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    
    int pid = next_pid++;
    //guardando su nombre y la memoria que pide.
    auto process = std::make_shared<Process>(pid, name, memory_required);
    
    //mira si hay memoria disponible
    process->memory_address = memory_manager.alloc(memory_required);
    
    if (process->memory_address == 0) {
        std::cout << "[SCHEDULER] Error: No se pudo asignar memoria para el proceso " 
                  << name << " (PID: " << pid << ")\n";
        return -1; // Error en la creación
    }
    
    // Añadir el proceso a la cola de listos
    ready_queue.push(process);
    
    std::cout << "[SCHEDULER] Proceso creado: " << name << " (PID: " << pid 
              << ", Memoria: " << memory_required << " bytes en dirección " 
              << process->memory_address << ")\n";
    
    // Notificar al scheduler que hay un nuevo proceso
    cv.notify_one();
    
    return pid;
}

// Inicia el hilo del scheduler
void ProcessScheduler::start_scheduler() {
    if (!scheduler_running.load()) {
        scheduler_running = true;
        scheduler_thread = std::thread(&ProcessScheduler::scheduler_loop, this);
        std::cout << "[SCHEDULER] Scheduler iniciado\n";
    }
}

// Para el scheduler y espera a que termine
void ProcessScheduler::stop_scheduler() {
    if (scheduler_running.load()) {
        scheduler_running = false;
        cv.notify_all();
        
        if (scheduler_thread.joinable()) {
            scheduler_thread.join();
        }
        
        // Esperar a que terminen todos los procesos en ejecución
        for (auto& pair : running_processes) {
            if (pair.second->thread_ptr && pair.second->thread_ptr->joinable()) {
                pair.second->thread_ptr->join();
            }
            // Liberar memoria del proceso
            memory_manager.free(pair.second->memory_address);
        }
        running_processes.clear();
        
        std::cout << "[SCHEDULER] Scheduler detenido\n";
    }
}

// Función principal
void ProcessScheduler::scheduler_loop() {
    while (scheduler_running.load()) {
        std::unique_lock<std::mutex> lock(scheduler_mutex);
        
        // Limpiar procesos terminados
        cleanup_finished_processes();
        
        // Esperar hasta que haya un proceso en la cola o se detenga 
        cv.wait(lock, [this] { 
            return !ready_queue.empty() || !scheduler_running.load(); 
        });
        
        if (!scheduler_running.load()) break;
        
        // Ejecutar procesos de la cola 
        while (!ready_queue.empty()) {
            auto process = ready_queue.front();
            ready_queue.pop();
            
            std::cout << "[SCHEDULER] Ejecutando proceso " << process->name 
                      << " (PID: " << process->pid << ")\n";
            
            execute_process(process);
        }
        
        lock.unlock();
        
        // Pequeña pausa para evitar errores
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Ejecuta un proceso y crea su hilo
void ProcessScheduler::execute_process(std::shared_ptr<Process> process) {
    // Crear el hilo para el proceso
    process->thread_ptr = std::make_unique<std::thread>(
        &ProcessScheduler::process_execution, this, process
    );
    
    // Añadir a la lista de procesos en ejecución
    running_processes[process->pid] = process;
}

// Simula la ejecución de un proceso
void ProcessScheduler::process_execution(std::shared_ptr<Process> process) {
    std::cout << "[PROCESO " << process->pid << "] Iniciando ejecución de " 
              << process->name << "\n";
    
    // Simular tiempo de ejecución variable (1-5 segundos)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 5000);
    int execution_time = dis(gen);
    
    // Simular trabajo del proceso
    auto start_time = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start_time < std::chrono::milliseconds(execution_time)) {
        // Simular trabajo (imprimir estado cada segundo)
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "[PROCESO " << process->pid << "] " << process->name 
                  << " trabajando... (Memoria: " << process->memory_address << ")\n";
    }
    
    std::cout << "[PROCESO " << process->pid << "] " << process->name 
              << " terminado después de " << execution_time << "ms\n";
    
    // Liberar memoria del proceso
    memory_manager.free(process->memory_address);
}

// Limpia procesos que ya terminaron
void ProcessScheduler::cleanup_finished_processes() {
    auto it = running_processes.begin();
    while (it != running_processes.end()) {
        if (it->second->thread_ptr && it->second->thread_ptr->joinable()) {
            // Verificar si el hilo ha terminado (sin bloquear)
            if (it->second->thread_ptr->get_id() == std::thread::id()) {
                it = running_processes.erase(it);
            } else {
                ++it;
            }
        } else {
            // El hilo ha terminado, hacer join y eliminar
            if (it->second->thread_ptr) {
                it->second->thread_ptr->join();
            }
            it = running_processes.erase(it);
        }
    }
}

// Muestra información de todos los procesos
void ProcessScheduler::display_processes() const {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    
    std::cout << "\n=== Estado de Procesos ===\n";
    std::cout << "Procesos en cola de listos: " << ready_queue.size() << "\n";
    std::cout << "Procesos en ejecución: " << running_processes.size() << "\n";
    
    if (!running_processes.empty()) {
        std::cout << "\nProcesos en ejecución:\n";
        std::cout << "PID\tNombre\t\tMemoria\t\tDirección\n";
        std::cout << "------------------------------------------------\n";
        for (const auto& pair : running_processes) {
            const auto& proc = pair.second;
            std::cout << proc->pid << "\t" << proc->name << "\t\t"
                      << proc->memory_required << "\t\t" << proc->memory_address << "\n";
        }
    }
    std::cout << "\n";
}

// Termina un proceso específico por PID
bool ProcessScheduler::terminate_process(int pid) {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    
    auto it = running_processes.find(pid);
    if (it != running_processes.end()) {
        std::cout << "[SCHEDULER] Terminando proceso " << it->second->name 
                  << " (PID: " << pid << ")\n";
        
        // Para este prototipo, simplemente esperamos a que termine naturalmente
        if (it->second->thread_ptr && it->second->thread_ptr->joinable()) {
            it->second->thread_ptr->join();
        }
        
        memory_manager.free(it->second->memory_address);
        running_processes.erase(it);
        return true;
    }
    
    std::cout << "[SCHEDULER] Error: Proceso con PID " << pid << " no encontrado\n";
    return false;
}
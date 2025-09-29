#include "MemoryManager.h"
#include "ProcessScheduler.h"
#include "Shell.h"
#include <iostream>
#include <csignal>
#include <memory>

// Variables globales para el manejo de señales
Shell* global_shell = nullptr;

// Manejador de señales (Ctrl+C)
void signal_handler(int senal) {
    std::cout << "\n[SISTEMA] Señal recibida (" << senal  << "). Cerrando sistema...\n";
    if (global_shell) {
        global_shell->stop();
    }
}

int main() {
    try {
        std::cout << "Iniciando Simple OS Simulator...\n";
        
        // Configurar manejador de señales
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        // Inicializar componentes del sistema operativo
        const size_t TOTAL_MEMORY = 8192; // 8KB (1024 bytes)
        
        std::cout << "[MAIN] Creando gestor de memoria...\n";
        MemoryManager memory_manager(TOTAL_MEMORY);
        
        std::cout << "[MAIN] Creando planificador de procesos...\n";
        ProcessScheduler process_scheduler(memory_manager);
        
        std::cout << "[MAIN] Creando shell del sistema...\n";
        Shell shell(memory_manager, process_scheduler);
        global_shell = &shell;
        
        // Mostrar información del sistema
        std::cout << "\n[MAIN] Sistema operativo inicializado exitosamente\n";
        std::cout << "[MAIN] Memoria total disponible: " << TOTAL_MEMORY << " bytes\n";
        std::cout << "[MAIN] Algoritmo de asignación de memoria: First-Fit\n";
        std::cout << "[MAIN] Algoritmo de planificación: FCFS (First-Come, First-Served)\n";
        
        // Ejecuta 
        shell.run();
        
        std::cout << "[MAIN] Sistema operativo terminado correctamente\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Excepción no manejada: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[ERROR] Excepción desconocida" << std::endl;
        return 1;
    }
    
    return 0;
}
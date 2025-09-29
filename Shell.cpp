#include "Shell.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

Shell::Shell(MemoryManager& mm, ProcessScheduler& ps) 
    : memory_manager(mm), process_scheduler(ps), running(false) {
    std::cout << "[SHELL] Inicializando shell del sistema operativo\n";
}

Shell::~Shell() {
    std::cout << "[SHELL] Cerrando shell\n";
}

// Bucle principal del shell
void Shell::run() {
    running = true;
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║                      SIMPLE OS SIMULATOR                     ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║                           Versión 1.0                        ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "Escribe help para ver los comandos disponibles.\n";
    std::cout << std::endl;
    
    // Iniciar el scheduler
    process_scheduler.start_scheduler();
    
    std::string input;
    while (running) {
        show_prompt();
        
        // Leer comando del usuario
        if (std::getline(std::cin, input)) {
            // Eliminar espacios al inicio y final
            input.erase(0, input.find_first_not_of(" \t"));
            input.erase(input.find_last_not_of(" \t") + 1);
            
            if (!input.empty()) {
                process_command(input);
            }
        }
    }
    
    // Parar el scheduler al salir
    process_scheduler.stop_scheduler();
}

void Shell::stop() {
    running = false;
}

// Procesa un comando ingresado
void Shell::process_command(const std::string& command) {
    std::vector<std::string> tokens = tokenize(command);
    
    if (tokens.empty()) return;
    
    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "alloc") {
        cmd_alloc(tokens);
    } else if (cmd == "exec") {
        cmd_exec(tokens);
    } else if (cmd == "free") {
        cmd_free(tokens);
    } else if (cmd == "ps") {
        cmd_ps(tokens);
    } else if (cmd == "mem") {
        cmd_mem(tokens);
    } else if (cmd == "kill") {
        cmd_kill(tokens);
    } else if (cmd == "help") {
        cmd_help(tokens);
    } else if (cmd == "clear") {
        cmd_clear(tokens);
    } else if (cmd == "exit" || cmd == "quit") {
        std::cout << "[SHELL] Cerrando sistema operativo...\n";
        stop();
    } else {
        std::cout << "[SHELL] Comando no reconocido: " << cmd 
                  << ". Escribe 'help' para ver los comandos disponibles.\n";
    }
}

// Divide una string en tokens
std::vector<std::string> Shell::tokenize(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Comando: alloc <tamaño> - Asignar memoria
void Shell::cmd_alloc(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        std::cout << "[SHELL] Uso: alloc <tamaño_en_bytes>\n";
        std::cout << "        Ejemplo: alloc 1024\n";
        return;
    }
    
    try {
        size_t size = std::stoull(args[1]);
        if (size == 0) {
            std::cout << "[SHELL] Error: El tamaño debe ser mayor que 0\n";
            return;
        }
        
        size_t addr = memory_manager.alloc(size);
        if (addr != 0) {
            std::cout << "[SHELL] Memoria asignada exitosamente en dirección: " << addr << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "[SHELL] Error: Tamaño inválido\n";
    }
}

// Comando: exec <nombre> <memoria> - Ejecutar proceso
void Shell::cmd_exec(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        std::cout << "[SHELL] Uso: exec <nombre_proceso> <memoria_requerida>\n";
        std::cout << "        Ejemplo: exec calculadora 512\n";
        return;
    }
    
    std::string name = args[1];
    
    try {
        size_t memory = std::stoull(args[2]);
        if (memory == 0) {
            std::cout << "[SHELL] Error: La memoria requerida debe ser mayor que 0\n";
            return;
        }
        
        int pid = process_scheduler.crear_proceso(name, memory);
        if (pid > 0) {
            std::cout << "[SHELL] Proceso '" << name << "' creado con PID: " << pid << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "[SHELL] Error: Memoria inválida\n";
    }
}

// Comando: free <dirección> - Liberar memoria
void Shell::cmd_free(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        std::cout << "[SHELL] Uso: free <dirección_memoria>\n";
        std::cout << "        Ejemplo: free 0\n";
        return;
    }
    
    try {
        size_t addr = std::stoull(args[1]);
        if (memory_manager.free(addr)) {
            std::cout << "[SHELL] Memoria liberada exitosamente\n";
        }
    } catch (const std::exception& e) {
        std::cout << "[SHELL] Error: Dirección inválida\n";
    }
}

// Comando: ps - Mostrar procesos
void Shell::cmd_ps(const std::vector<std::string>& /*args*/) {
    process_scheduler.display_processes();
}

// Comando: mem - Mostrar estado de memoria
void Shell::cmd_mem(const std::vector<std::string>& /*args*/) {
    memory_manager.display_memory();
}

// Comando: kill <pid> - Terminar proceso
void Shell::cmd_kill(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        std::cout << "[SHELL] Uso: kill <pid>\n";
        std::cout << "        Ejemplo: kill 1\n";
        return;
    }
    
    try {
        int pid = std::stoi(args[1]);
        if (process_scheduler.terminate_process(pid)) {
            std::cout << "[SHELL] Proceso " << pid << " terminado\n";
        }
    } catch (const std::exception& e) {
        std::cout << "[SHELL] Error: PID inválido\n";
    }
}

// Comando: help - Mostrar ayuda
void Shell::cmd_help(const std::vector<std::string>& /*args*/) {
    std::cout << "\n=== COMANDOS DISPONIBLES ===\n";
    std::cout << std::left;
    std::cout << std::setw(25) << "alloc <tamaño>" << "Asignar memoria\n";
    std::cout << std::setw(25) << "exec <nombre> <memoria>" << "Crear y ejecutar proceso\n";
    std::cout << std::setw(25) << "free <dirección>" << "Liberar bloque de memoria\n";
    std::cout << std::setw(25) << "ps" << "Mostrar procesos en ejecución\n";
    std::cout << std::setw(25) << "mem" << "Mostrar estado de memoria\n";
    std::cout << std::setw(25) << "kill <pid>" << "Terminar proceso\n";
    std::cout << std::setw(25) << "clear" << "Limpiar pantalla\n";
    std::cout << std::setw(25) << "help" << "Mostrar esta ayuda\n";
    std::cout << std::setw(25) << "exit/quit" << "Salir del sistema\n";
    std::cout << "\nEjemplos:\n";
    std::cout << "  alloc 1024          # Asignar 1024 bytes\n";
    std::cout << "  exec editor 512     # Crear proceso 'editor' con 512 bytes\n";
    std::cout << "  free 0              # Liberar memoria en dirección 0\n";
    std::cout << "  kill 1              # Terminar proceso con PID 1\n\n";
}

// Comando: clear - Limpiar pantalla
void Shell::cmd_clear(const std::vector<std::string>& /*args*/) {
    // Comando ANSI para limpiar pantalla
    std::cout << "\033[2J\033[1;1H";
}

// Muestra el prompt del shell
void Shell::show_prompt() const {
    std::cout << "SimpleOS> ";
}
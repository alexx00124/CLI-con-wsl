#ifndef SHELL_H
#define SHELL_H

#include "MemoryManager.h"
#include "ProcessScheduler.h"
#include <string>
#include <vector>

class Shell {
private:
    MemoryManager& memory_manager;
    ProcessScheduler& process_scheduler;
    bool running;

public:
    // Constructor
    Shell(MemoryManager& mm, ProcessScheduler& ps);
    
    // Destructor
    ~Shell();
    
    // Inicia el bucle principal del shell
    void run();
    
    // Para el shell
    void stop();

private:
    // Procesa un comando ingresado por el usuario
    void process_command(const std::string& command);
    
    // Divide una línea de comando en tokens
    std::vector<std::string> tokenize(const std::string& str);
    
    // Comandos específicos
    void cmd_alloc(const std::vector<std::string>& args);
    void cmd_exec(const std::vector<std::string>& args);
    void cmd_free(const std::vector<std::string>& args);
    void cmd_ps(const std::vector<std::string>& args);
    void cmd_mem(const std::vector<std::string>& args);
    void cmd_kill(const std::vector<std::string>& args);
    void cmd_help(const std::vector<std::string>& args);
    void cmd_clear(const std::vector<std::string>& args);
    
    // Muestra el prompt del shell
    void show_prompt() const;
};

#endif // SHELL_H
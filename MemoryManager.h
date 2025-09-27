#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <mutex>
#include <iostream>

// Estructura que representa un bloque de memoria
struct Block {
    size_t size;        // Tamaño del bloque
    bool is_free;       // Estado: true = libre, false = ocupado
    size_t start_addr;  // Dirección de inicio (simulada)
    
    Block(size_t s, bool free, size_t addr) : size(s), is_free(free), start_addr(addr) {}
};

class MemoryManager {
private:
    std::vector<Block> memory_blocks;  // Vector que simula la memoria
    size_t total_memory;               // Memoria total disponible
    mutable std::mutex memory_mutex;   // Mutex para acceso thread-safe (mutable para const functions)

public:
    // Constructor: inicializa la memoria con un bloque libre grande
    MemoryManager(size_t total_size);
    
    // Destructor
    ~MemoryManager();
    
    // Reserva memoria usando algoritmo First-Fit
    // Retorna la dirección de inicio del bloque asignado (0 si falla)
    size_t alloc(size_t size);
    
    // Libera un bloque de memoria dado su dirección de inicio
    bool free(size_t start_addr);
    
    // Muestra el estado actual de la memoria
    void display_memory() const;
    
    // Obtiene estadísticas de memoria
    void get_memory_stats(size_t& total, size_t& used, size_t& free) const;

private:
    // Función auxiliar para fusionar bloques libres adyacentes
    void merge_free_blocks();
};

#endif // MEMORY_MANAGER_H
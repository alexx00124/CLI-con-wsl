#include "MemoryManager.h"
#include <algorithm>

// Constructor: inicializa la memoria con un solo bloque libre
MemoryManager::MemoryManager(size_t total_size) : total_memory(total_size) {
    // Crear un bloque inicial que representa toda la memoria disponible
    memory_blocks.emplace_back(total_size, true, 0);
    std::cout << "[MEMORY] Inicializando gestor de memoria con " << total_size << " bytes\n";
}

MemoryManager::~MemoryManager() {
    std::cout << "[MEMORY] Destruyendo gestor de memoria\n";
}

// Implementación del algoritmo First-Fit para asignación de memoria
size_t MemoryManager::alloc(size_t size) {
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    // Buscar el primer bloque libre que sea lo suficientemente grande
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
        if (it->is_free && it->size >= size) {
            size_t allocated_addr = it->start_addr;
            
            // Si el bloque es exactamente del tamaño requerido
            if (it->size == size) {
                it->is_free = false;
            } else {
                // Dividir el bloque: parte asignada + parte libre restante
                size_t remaining_size = it->size - size;
                size_t remaining_start = it->start_addr + size;
                
                // Modificar el bloque actual (ahora ocupado)
                it->size = size;
                it->is_free = false;
                
                // Crear un nuevo bloque libre con el espacio restante
                memory_blocks.insert(it + 1, Block(remaining_size, true, remaining_start));
            }
            
            std::cout << "[MEMORY] Asignados " << size << " bytes en dirección " 
                      << allocated_addr << "\n";
            return allocated_addr;
        }
    }
    
    // No se encontró espacio suficiente
    std::cout << "[MEMORY] Error: No hay espacio suficiente para " << size << " bytes\n";
    return 0;  // 0 indica fallo en la asignación
}

// Libera un bloque de memoria
bool MemoryManager::free(size_t start_addr) {
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    // Buscar el bloque con la dirección especificada
    for (auto& block : memory_blocks) {
        if (block.start_addr == start_addr && !block.is_free) {
            block.is_free = true;
            std::cout << "[MEMORY] Liberados " << block.size << " bytes en dirección " 
                      << start_addr << "\n";
            
            // Fusionar bloques libres adyacentes
            merge_free_blocks();
            return true;
        }
    }
    
    std::cout << "[MEMORY] Error: No se encontró bloque en dirección " << start_addr << "\n";
    return false;
}

// Muestra el estado actual de todos los bloques de memoria
void MemoryManager::display_memory() const {
    std::lock_guard<std::mutex> lock(memory_mutex);
    
    std::cout << "\n=== Estado de la Memoria ===\n";
    std::cout << "Dirección\tTamaño\t\tEstado\n";
    std::cout << "----------------------------------------\n";
    
    for (const auto& block : memory_blocks) {
        std::cout << block.start_addr << "\t\t" << block.size << "\t\t"
                  << (block.is_free ? "LIBRE" : "OCUPADO") << "\n";
    }
    
    size_t total, used, free;
    get_memory_stats(total, used, free);
    std::cout << "----------------------------------------\n";
    std::cout << "Total: " << total << " | Usado: " << used << " | Libre: " << free << "\n\n";
}

// Obtiene estadísticas de uso de memoria
void MemoryManager::get_memory_stats(size_t& total, size_t& used, size_t& free) const {
    total = total_memory;
    used = 0;
    free = 0;
    
    for (const auto& block : memory_blocks) {
        if (block.is_free) {
            free += block.size;
        } else {
            used += block.size;
        }
    }
}

// Función auxiliar para fusionar bloques libres adyacentes
void MemoryManager::merge_free_blocks() {
    // Ordenar bloques por dirección de inicio
    std::sort(memory_blocks.begin(), memory_blocks.end(),
              [](const Block& a, const Block& b) {
                  return a.start_addr < b.start_addr;
              });
    
    // Fusionar bloques libres consecutivos
    for (auto it = memory_blocks.begin(); it != memory_blocks.end() - 1;) {
        if (it->is_free && (it + 1)->is_free && 
            (it->start_addr + it->size == (it + 1)->start_addr)) {
            
            // Fusionar el bloque actual con el siguiente
            it->size += (it + 1)->size;
            memory_blocks.erase(it + 1);
        } else {
            ++it;
        }
    }
}
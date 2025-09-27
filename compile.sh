#!/bin/bash

echo "🔨 Compilando Simple OS Simulator..."

# Limpiar archivos anteriores
rm -f *.o os_sim

# Compilar con manejo de errores
g++ -std=c++17 -Wall -Wextra -O2 -pthread \
    main.cpp MemoryManager.cpp ProcessScheduler.cpp Shell.cpp \
    -o os_sim

if [ $? -eq 0 ]; then
    echo "✅ Compilación exitosa!"
    echo "Ejecutar con: ./os_sim"
    echo ""
    echo "Para probar rápidamente:"
    echo "  ./os_sim"
    echo ""
    echo "Comandos básicos una vez dentro:"
    echo "  help     # Ver todos los comandos"
    echo "  mem      # Ver estado de memoria"
    echo "  alloc 1024    # Asignar 1024 bytes"
    echo "  exec test 512 # Crear proceso"
    echo "  ps       # Ver procesos"
    echo "  exit     # Salir"
else
    echo "❌ Error en la compilación"
    exit 1
fi

# Makefile para Simple OS Simulator

# Compilador y flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
TARGET = os_sim

# Archivos fuente
SOURCES = main.cpp MemoryManager.cpp ProcessScheduler.cpp Shell.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Archivos header
HEADERS = MemoryManager.h ProcessScheduler.h Shell.h

# Regla principal
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJECTS)
	@echo "Enlazando $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(CXXFLAGS)
	@echo "✅ Compilación exitosa!"
	@echo "Ejecutar con: ./$(TARGET)"

# Compilar archivos objeto
%.o: %.cpp $(HEADERS)
	@echo "Compilando $<..."
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Limpiar archivos compilados
clean:
	@echo "Limpiando archivos compilados..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "✅ Limpieza completada"

# Compilar en modo debug
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)

# Verificar dependencias
check:
	@echo "Verificando compilador..."
	@which $(CXX) > /dev/null || (echo "❌ g++ no encontrado" && exit 1)
	@echo "✅ g++ encontrado: $$($(CXX) --version | head -n1)"
	@echo "✅ Todas las dependencias están disponibles"

# Instalar dependencias en Ubuntu/Debian
install-deps:
	@echo "Instalando dependencias para Ubuntu/WSL..."
	sudo apt update
	sudo apt install -y build-essential g++ make
	@echo "✅ Dependencias instaladas"

# Ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make         - Compilar el proyecto"
	@echo "  make clean   - Limpiar archivos compilados"
	@echo "  make debug   - Compilar en modo debug"
	@echo "  make run     - Compilar y ejecutar"
	@echo "  make check   - Verificar dependencias"
	@echo "  make install-deps - Instalar dependencias (Ubuntu/WSL)"
	@echo "  make help    - Mostrar esta ayuda"

.PHONY: all clean debug run check install-deps help
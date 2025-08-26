CXX := g++
CXXFLAGS := -O3 -std=c++11 -I.

TARGET := main.out
BUILD_DIR := build

# Liste tous les .cpp dans project/ et modules/
SRCS := $(shell find project -name "*.cpp") \
        $(shell find modules -name "*.cpp") \
        main.cpp

# Crée les chemins vers les .o dans build/
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Règle par défaut
all: $(TARGET)

# Lien final
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Compilation de chaque .cpp en .o
# Utilise la variable $< (source) et $@ (target)
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
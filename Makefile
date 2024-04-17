COMPILER = g++

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
OPTIMIZATION_FLAGS = -O2 -mavx2 -msse4

TASK_NAME = mandelbrot

OBJ_DIR = ./obj
SRC_DIR = ./src

CPP := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ = $(CPP:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all : build 

.PHONY : build
build : $(TASK_NAME)

$(TASK_NAME) : $(OBJ) 
	@ mkdir -p $(@D)
	@$(COMPILER) $(FLAGS) $(OPTIMIZATION_FLAGS) $(OBJ) -o $(TASK_NAME) $(SFML_FLAGS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(COMPILER) $(FLAGS) $(OPTIMIZATION_FLAGS)  $(SFML_FLAGS) -c $< -o $@

.PHONY : cleanup
cleanup :
	@rm -rf $(TASK_NAME) $(OBJ_DIR)
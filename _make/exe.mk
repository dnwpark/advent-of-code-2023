# # Inputs
# 
# EXE_NAME = app
# 
# SRCS = $(wildcard *.cpp) 
# OUTPUT_DIR = ..

CC = x86_64-w64-mingw32-g++.exe

CFLAGS_ALL = -Wall -Wextra -Werror -std=c++2b -O3 -s -DNDEBUG -DSRC_DIR=\"$(EXE_NAME)\"
OBJ_DIR_ALL = _obj
OBJS_ALL = $(SRCS:%.cpp=$(OBJ_DIR_ALL)/%.o)
EXE_ALL_NAME = $(EXE_NAME).exe
EXE_ALL = $(OUTPUT_DIR)/$(EXE_ALL_NAME)

# all
all: $(EXE_ALL)

$(EXE_ALL): $(OBJS_ALL)
	$(CC) -o $(EXE_ALL) $(OBJS_ALL) -lmingw32

$(OBJ_DIR_ALL)/%.o: %.cpp
	@mkdir -p ./$(OBJ_DIR_ALL)
	$(CC) $(CFLAGS) $(CFLAGS_ALL) -MMD -c $< -o $@

# clean
clean:
	$(RM) $(EXE_ALL)
	$(RM) $(OBJ_DIR_ALL)/*.o

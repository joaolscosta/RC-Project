# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -pedantic -Wno-sizeof-pointer-memaccess -Wno-sizeof-array-argument -Wno-int-conversion -Wno-format-overflow -Wno-incompatible-pointer-types -Wunused-variable -Wreturn-type -Wimplicit-function-declaration -Wmaybe-uninitialized

# Source files
SRC_DIR = src
USER_SOURCE = $(SRC_DIR)/actions.c $(SRC_DIR)/user.c
AS_SOURCE = $(SRC_DIR)/actions.c $(SRC_DIR)/AS.c

# Target executables
USER_TARGET = user
AS_TARGET = AS

# Main targets
all: $(USER_TARGET) $(AS_TARGET)

$(USER_TARGET): $(USER_SOURCE)
	$(CC) $(CFLAGS) -o $@ $^

$(AS_TARGET): $(AS_SOURCE)
	$(CC) $(CFLAGS) -o $@ $^

# Phony target to clean the project
.PHONY: clean
clean:
	rm -f $(USER_TARGET) $(AS_TARGET)

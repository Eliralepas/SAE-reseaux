TARGET_EXEC ?= main

BUILD_DIR ?= ./build
DEBUG_DIR ?= ./debug
TARGET_DIR ?= ./bin
SRC_DIRS ?= ./source

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEBUG_OBJS := $(SRCS:%=$(DEBUG_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
DEBUG_DEPS := $(DEBUG_OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= $(INC_FLAGS) -MMD -MP -O3
DEBUG_CFLAGS ?= $(INC_FLAGS) -MMD -MP -g -O0

$(TARGET_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(DEBUG_DIR)/$(TARGET_EXEC): $(DEBUG_OBJS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(DEBUG_OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

# Règle pour copier fichier.txt vers le répertoire bin
$(TARGET_DIR)/fichier.txt: $(SRC_DIRS)/fichier.txt
	$(MKDIR_P) $(TARGET_DIR)
	cp $< $@

# Règle pour copier fichier.txt pour le debug
$(DEBUG_DIR)/fichier.txt: $(SRC_DIRS)/fichier.txt
	$(MKDIR_P) $(DEBUG_DIR)
	cp $< $@

# Ajoutez la copie de fichier.txt comme dépendance aux cibles principales
$(TARGET_DIR)/$(TARGET_EXEC): $(TARGET_DIR)/fichier.txt
$(DEBUG_DIR)/$(TARGET_EXEC): $(DEBUG_DIR)/fichier.txt

.PHONY: clean debug

clean:
	$(RM) -r $(BUILD_DIR) $(DEBUG_DIR) $(TARGET_DIR)

debug: $(DEBUG_DIR)/$(TARGET_EXEC)

-include $(DEPS)
-include $(DEBUG_DEPS)

MKDIR_P ?= mkdir -p

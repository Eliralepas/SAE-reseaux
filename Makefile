TARGET_EXEC ?= main

BUILD_DIR ?= ./build
TARGET_DIR ?= ./bin
SRC_DIRS ?= ./source

SRCS := $(shell find $(SRC_DIRS) -name "*.c")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS ?= $(INC_FLAGS) -MMD -MP -O3

$(TARGET_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour copier fichier.txt vers le répertoire bin
$(TARGET_DIR)/fichier.txt: $(SRC_DIRS)/fichier.txt
	$(MKDIR_P) $(TARGET_DIR)
	cp $< $@

# Ajoutez la copie de fichier.txt comme dépendance à la cible principale
$(TARGET_DIR)/$(TARGET_EXEC): $(TARGET_DIR)/fichier.txt

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p



CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc

CFLAGS := -Wall -Wextra -Iinclude

BUILD := build
SRC := src

TARGET := $(BUILD)/app.elf

OBJS := \
	$(BUILD)/app.o \
	$(BUILD)/lcd.o \
	$(BUILD)/bme280.o

all: $(TARGET)

#Création du dossier build si absent
$(BUILD):
	mkdir -p $(BUILD)

# Link final
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# Compilation des sources
$(BUILD)/app.o: $(SRC)/app.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/lcd.o: $(SRC)/lcd.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/bme280.o: $(SRC)/bme280.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^
clean:
	rm -rf $(BUILD)

.PHONY: all clean

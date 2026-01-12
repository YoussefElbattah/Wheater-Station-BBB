CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc

CFLAGS := -Wall -Wextra -Iinclude

LDFLAGS := -lmosquitto

BUILD := build
SRC := src

TARGET := $(BUILD)/app.elf

OBJS := \
	$(BUILD)/app.o \
	$(BUILD)/lcd.o \
	$(BUILD)/bme280.o\
	$(BUILD)/mqtt.o

all: $(TARGET)

#Création du dossier build si absent
$(BUILD):
	mkdir -p $(BUILD)

# Link final
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compilation des sources
$(BUILD)/app.o: $(SRC)/app.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/lcd.o: $(SRC)/lcd.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/bme280.o: $(SRC)/bme280.c | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/mqtt.o: $(SRC)/mqtt.c | $(BUILD)
	        $(CC) $(CFLAGS) -c -o $@ $^
clean:
	rm -rf $(BUILD)

.PHONY: all clean

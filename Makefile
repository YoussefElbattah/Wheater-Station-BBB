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
	$(BUILD)/mqtt.o \
	$(BUILD)/display.o \
	$(BUILD)/weather.o \
	$(BUILD)/wireless.o

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

$(BUILD)/display.o: $(SRC)/display.c | $(BUILD)
	        $(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/weather.o: $(SRC)/weather.c | $(BUILD)
	        $(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/wireless.o: $(SRC)/wireless.c | $(BUILD)
	        $(CC) $(CFLAGS) -c -o $@ $^
clean:
	rm -rf $(BUILD)

.PHONY: all clean

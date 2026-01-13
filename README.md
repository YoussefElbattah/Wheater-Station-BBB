# Linux Temperature Monitoring Station – BeagleBone Black

This project implements an embedded Linux temperature monitoring system based on the BeagleBone Black.  
It demonstrates a clean and realistic embedded Linux architecture, from **hardware description using the Device Tree**, to **kernel drivers**, and a **userspace application** interacting exclusively through **sysfs**.

The system continuously measures temperature using a **BME280 sensor**, interprets the data using simple deterministic logic, and displays the system state on a **16x2 character LCD**, and simultaneously sends the data over MQTT through a WAN connection.

---

## Problem Statement
Many small industrial or remote environments require continuous monitoring of temperature conditions, but do not have the luxury of full-size PCs, cloud connectivity, or complex networking systems.
They need simple, reliable, and autonomous embedded devices capable of:

- Measuring environmental data locally
- Displaying the information immediately to workers on site
- Storing or publishing the values remotely when connectivity is available
- Operating even if the Internet connection goes down
- Using trusted, open, maintainable Linux technologies rather than proprietary IoT platforms

This project implements a complete **embedded Linux IoT temperature monitoring station**
running on a **BeagleBone Black**, equipped with:

- BME280 temperature sensor  
- 16x2 character LCD  
- Userspace application split into logical modules  
- Reads kernel sysfs (no direct GPIO/I²C from userspace)  
- MQTT WAN publishing 

---

## Project Overview

1. Describe hardware properly in Device Tree  
2. Let Linux kernel drivers handle all peripherals  
3. Access hardware only through sysfs from userspace  
4. Apply simple temperature logic (min, max, status)  
5. Provide local LCD output  
6. Publish JSON telemetry to a remote MQTT broker  
7. Keep credentials **outside the code** using a config file Architecture Diagram

![Architecture diagram](diagram/architecture_diagram.png)

---

### Sequence Diagram

![Sequence diagram](diagram/sequence_diagram.png)

---

### Global data flow

**Hardware → Device Tree → Kernel subsystems → Kernel drivers → sysfs → Userspace application → MQTT Broker → Remote Clients**

---

## Hardware

**Board**
- BeagleBone Black (AM335x)

**Peripherals**
- BME280 environmental sensor (temperature source)
- LCD 16x2 character display (4-bit mode)

The BME280 sensor is connected to the **I²C1 bus** at address `0x76`.  
The LCD is driven through GPIOs using a **custom Linux platform driver**.

---

## Architecture

The system is structured as follows:
- temperature acquisition is handled by the standard Linux **IIO BME280 driver**
- display control is handled by a **custom LCD platform driver**
- userspace reads sensor values and sends display commands through sysfs
- userspace sends temperature data using MQTT to a broker

---

## GPIO and Pin Connections

### LCD 16x2 (4-bit mode)

| LCD Signal | BeagleBone Black GPIO |
|----------|------------------------|
| RS | gpio2_2 |
| RW | gpio2_3 |
| EN | gpio2_4 |
| D4 | gpio1_12 |
| D5 | gpio0_26 |
| D6 | gpio1_14 |
| D7 | gpio0_27 |

All GPIOs are configured through **am33xx-pinmux** in the Device Tree.

---

### BME280 (I²C1)

| Signal | BeagleBone Black |
|------|------------------|
| SDA | I2C1_SDA |
| SCL | I2C1_SCL |
| Address | `0x76` |

---

## Device Tree Description

The hardware configuration is defined in `am335x-boneblack-custom.dtsi`.

### Pinmux

The `am33xx-pinmux` node configures:
- GPIO pins used by the LCD via `lcd_pinctrl`
- I²C1 pins via `i2c1_pins`

### LCD Node

The LCD device:
- uses the compatible string `org,bone-lcd`
- references `lcd_pinctrl`
- declares all control and data GPIOs
- is bound to a custom platform driver

### BME280 Node

The BME280 device:
- is attached to the `i2c1` bus
- uses the compatible string `bosch,bme280`
- is handled by the standard Linux IIO driver

---

## Kernel Space

### Responsibilities
- Owns LCD GPIOs through a custom platform LCD driver  
- Owns BME280 via the IIO subsystem  
- Exposes simple file interfaces

### Subsystems
- I²C subsystem for sensor communication
- GPIO subsystem for LCD control

### Drivers
- **BME280 IIO driver** (standard Linux driver)
- **LCD 16x2 platform driver** (custom)

The BME280 driver exposes raw temperature data via **sysfs (IIO)**.  
The LCD driver exposes sysfs entries to:
- clear the display
- write text
- move the cursor

---

## Userspace Application

The userspace application is written in **C**.

### Responsibilities
| Module     | Responsibility |
|------------|----------------|
| `weather.c` | Reads sysfs temperature and computes business logic |
| `display.c` | Writes formatted output to LCD |
| `mqtt.c`    | Loads config, connects to MQTT, publishes JSON payload |
| `wireless.c`| Makes SURE MQTT can talk to the broker (connect + manage internet) |
| `app.c`     | Main control loop orchestrating sensors, display & network |

---

## Temperature Status Definition

Temperature status is derived from coarse thresholds based on **human thermal comfort** in indoor environments.

- **LOW**    : temperature < 15 °C  
- **MEDIUM** : 15 °C ≤ temperature ≤ 28 °C  
- **HIGH**   : temperature > 28 °C  

Thresholds are intentionally wide to ensure **stable behavior** on a character LCD and to keep the system fully **deterministic and explainable**.

---

## Display Behavior (LCD 16x2)

### Line 1
```
T:28.3C ST:MED
```

### Line 2
```
LO/HI 19.0/30.0
```

---

## 🌍 WAN MQTT Connectivity

The system can transmit telemetry to:
- LAN MQTT brokers
- Public brokers via port-forwarding
- DDNS-based hosts (DuckDNS, NoIP, etc.)

Consumers include:
- MQTT phone apps
- Mosquitto clients
- Telegraf → InfluxDB → Grafana

---

## 🔐 MQTT Configuration File

Credentials are stored in:

```
/etc/bbb_mqtt.conf
```

Format:
```
host=my-broker.duckdns.org
port=40000
user=bbb_user
pass=bbb_pass
topic=bbb/weather
keepalive=60
```

Ensure permissions:
```bash
sudo chmod 600 /etc/bbb_mqtt.conf
```

Location is defined in `mqtt.h`:

```c
#define CONF_FILE "/etc/bbb_mqtt.conf"
```

---

## 🧪 JSON Payload Example

```json
{
  "current_temp":23.4,
  "min":21.1,
  "max":29.6,
  "status_code":1,
  "status":"MEDIUM"
}
```

MQTT topic example:
```
bbb/weather
```

---

## Build and Run

### Building and Deploying the LCD Kernel Module (Quick Guide)

This section shows the **minimal steps** required to obtain
`lcd_platform_driver.ko` on a **BeagleBone Black** using a **host-based build**.

---

### 1. Prepare the Module Source (Host)

Copy `lcd_platform_driver.c` from the project `drivers/` directory to the host machine.

Directory layout:

```
lcd_driver/
├── lcd_platform_driver.c
└── Makefile
```

Create the following `Makefile` next to `lcd_platform_driver.c`:

```make
obj-m := lcd_platform_driver.o
```

---

### 2. Build the Module on the Host

From the module directory on the host machine:

```bash
make -C <KERNEL_SOURCE_DIR> \
     M=$(pwd) \
     ARCH=arm \
     CROSS_COMPILE=arm-linux-gnueabihf- \
     modules
```

Result:

```
lcd_platform_driver.ko
```

---

### 3. Transfer to the BeagleBone Black

From the host machine:

```bash
scp lcd_platform_driver.ko <user>@<bbb-ip>:/home/<user>/
```

---

### 4. Load and Verify on the Target

On the BeagleBone Black:

```bash
sudo insmod lcd_platform_driver.ko
dmesg | tail
```

Verify the LCD sysfs interface:

```bash
ls /sys/class/bone_lcd/lcd_16x2/
```

### Verify BME280

```bash
ls /sys/bus/iio/devices/iio:device0/
cat in_temp_input
```

---

### Build and run the application

```bash
make all
./build/app.elf
```

---

## 📡 Test MQTT output

From any device (LAN or WAN):
```bash
mosquitto_sub -h <your-host> -p <port> -t bbb/weather -u <user> -P <pass>
```

Or use:
- MQTT Dashboard (Android)
- MQTT Explorer

---
## What This Project Demonstrates

- Device Tree Hardware declaration  
- Custom Linux platform driver  
- Integration of IIO sensors  
- sysfs-based hardware/userspace interaction  
- Secure external config management  
- Modular embedded application design  
- WAN IoT telemetry

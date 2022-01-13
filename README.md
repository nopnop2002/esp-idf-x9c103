# esp-idf-x9c103
Three-Wire Digitally Controlled Potentiometer driver for ESP-IDF.

ESP32 has two 8-bit DAC (digital to analog converter) channels, connected to GPIO25 (Channel 1) and GPIO26 (Channel 2).   
However, the maximum voltage is up to VDD.   
If you control a voltage up to 5V, you can use this.   

- X9C102 = 1kOhms   
- X9C103 = 10kOhms   
- X9C503 = 50kOhms   
- X9C104 = 100kOhms   

I ported from [here](https://github.com/lucyamy/LapX9C10X).   

# Installation for ESP32

```Shell
git clone https://github.com/nopnop2002/esp-idf-x9c103
cd esp-idf-x9c103
idf.py set-target esp32
idf.py menuconfig
idf.py flash
```

# Installation for ESP32-S2

```Shell
git clone https://github.com/nopnop2002/esp-idf-x9c103
cd esp-idf-x9c103
idf.py set-target esp32s2
idf.py menuconfig
idf.py flash
```

# Installation for ESP32-C3

```Shell
git clone https://github.com/nopnop2002/esp-idf-x9c103
cd esp-idf-x9c103
idf.py set-target esp32c3
idf.py menuconfig
idf.py flash
```


# Configuration   

![config-top](https://user-images.githubusercontent.com/6020549/149045495-e4735ac4-b28f-488e-8768-5fccb94ae3e7.jpg)
![config-x9c103-1](https://user-images.githubusercontent.com/6020549/149045498-888247f2-06dd-42c4-9da0-46f737ee57b5.jpg)
![config-x9c103-2](https://user-images.githubusercontent.com/6020549/149048774-ac062afb-dd17-48fe-b419-24c1838a12be.jpg)


# Wirering

|Circuit|X9C||ESP32|ESP32-S2|ESP32-C3|
|:-:|:-:|:-:|:-:|:-:|:-:|
||INC|--|GPIO33|GPIO38|GPIO3|
||U/D|--|GPIO32|GPIO33|GPIO4|
|High terminals|Vh/Rh|--||||
||Vss|--|GND|GND|GND|
|Wiper terminals|Vw/Rw|--||||
|Low terminals|Vl/Rl|--||||
||CS|--|GPIO5|GPIO34|GPIO5|
||Vcc|--|5V|5V|5V|

__You can change it to any pin using menuconfig.__   

If you connect to Low terminals = Gnd and High terminals = 5V, 0 to 5V is output to Wiper terminals in 99 steps.   


# API
See [here](https://github.com/lucyamy/LapX9C10X).


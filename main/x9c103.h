#ifndef X9C_H
#define X9C_H

#define KOHMS_X9C102 1
#define KOHMS_X9C103 10
#define KOHMS_X9C503 50
#define KOHMS_X9C104 100

#define HIGH 1
#define LOW  0

#define delayMicroseconds(microsec) esp_rom_delay_us(microsec)
#define delay(millsec) ets_delay_us(millsec*1000)

typedef struct {
	uint8_t _incrementPin;
	uint8_t _upDownPin;
	uint8_t _deviceSelectPin;
	uint8_t _counterValue;
	float _totalKOhms;
} X9C_t;

void X9C_init(X9C_t * dev, uint8_t incrementPin, uint8_t upDownPin, uint8_t selectNVRAMWritePin, float resistance);
void X9C_begin(X9C_t * dev, int value);
void X9C_set_counter(X9C_t * dev, int value);
void X9C_set_resistance(X9C_t * dev, float value);
uint8_t X9C_get(X9C_t * dev);
float X9C_getK(X9C_t * dev);
void X9C_offset(X9C_t * dev, int value);
void X9C_reset(X9C_t * dev, int value);
void X9C_writeNVM(X9C_t * dev);

void X9C__setValue(X9C_t * dev, uint8_t value);
void X9C__setupChange(X9C_t * dev, uint8_t direction);
void X9C__pulseInc(X9C_t * dev);
void X9C__doChange(X9C_t * dev, uint8_t steps);
void X9C__reset(X9C_t * dev, bool minMax);

#endif

/*
 I ported from https://github.com/lucyamy/LapX9C10X
*/

#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "x9c103.h"

#define X9C_ASSERT				LOW
#define X9C_DEASSERT			HIGH
#define X9C_DOWN					X9C_ASSERT
#define X9C_UP						X9C_DEASSERT
#define X9C_MAX					true
#define X9C_MIN					false
#define cToK(c, _totalKOhms) ((float) (c) / 99.0 * _totalKOhms)
#define kToC(r, _totalKOhms) ((uint8_t) (((r) * 99.0 / _totalKOhms) + 0.5))



//
// Public functions
//


void X9C_init(X9C_t * dev, uint8_t incrementPin, uint8_t upDownPin, uint8_t deviceSelectPin, float resistance) {
	dev->_incrementPin = incrementPin;
	dev->_upDownPin = upDownPin;
	dev->_deviceSelectPin = deviceSelectPin;	
	dev->_totalKOhms = resistance;
}

#if 0
//
// begin(). Must be called first, before using other calls, or behaviour is undefined.
//

//
// begin() function for when the current counter value is unknown.
//

void X9C_begin() {
	gpio_set_direction(dev->_incrementPin, GPIO_MODE_OUTPUT);
	gpio_set_direction(dev->_upDownPin, GPIO_MODE_OUTPUT);
	gpio_set_direction(dev->_deviceSelectPin, GPIO_MODE_OUTPUT);
	// !INC will be high by default, after the gpio_set_direction() in the contructor.
	// We might as well take it low before deasserting !CS - which should also
	// be high at this point - or the chip will save the current value to
	// non-volatile memory.
	gpio_set_level(dev->_incrementPin, X9C_ASSERT);
	gpio_set_level(dev->_deviceSelectPin, X9C_DEASSERT);
	// Push resistance to max - note that very low values may occur as
	// we do this, due to the internal operation of the chip.
	X9C_reset(dev, X9C_MAX);
}
#endif

//
// begin() function which is passed current counter value.
// It's up to the caller to make sure this is correct.
// Passing any negative integer will force the counter to 0.
//

void X9C_begin(X9C_t * dev, int value) {
	gpio_reset_pin(dev->_incrementPin);
	gpio_reset_pin(dev->_upDownPin);
	gpio_reset_pin(dev->_deviceSelectPin);
	gpio_set_direction(dev->_incrementPin, GPIO_MODE_OUTPUT);
	gpio_set_direction(dev->_upDownPin, GPIO_MODE_OUTPUT);
	gpio_set_direction(dev->_deviceSelectPin, GPIO_MODE_OUTPUT);
	// !INC will be high by default, after the gpio_set_direction() in the contructor.
	// We might as well take it low before deasserting !CS - which should also
	// be high at this point - or the chip will save the current value to
	// non-volatile memory.
	gpio_set_level(dev->_incrementPin, X9C_ASSERT);
	gpio_set_level(dev->_deviceSelectPin, X9C_DEASSERT);
	// Set the counter to the value we were given, and hope for the best,
	// or force it to 0.
	if(value < 0) X9C_reset(dev, X9C_MIN);
	else dev->_counterValue = value > 99 ? value : 99;
}

//
// Set a counter value directly (absolute change)
//

void X9C_set_counter(X9C_t * dev, int value) {
	X9C__setValue(dev, value);
}


//
// Set a resistance directly (absolute change)
//

void X9C_set_resistance(X9C_t * dev, float value) {
	X9C__setValue(dev, kToC(value, dev->_totalKOhms));
}

//
// Get the current value
//

uint8_t X9C_get(X9C_t * dev) {
	return dev->_counterValue;
}

//
// Get the current resistance
//

float X9C_getK(X9C_t * dev) {
	return cToK(dev->_counterValue, dev->_totalKOhms);
}

//
// Increase/decrease by an amount (counter) (relative change)
//

void X9C_offset(X9C_t * dev, int value) {
	int v;
	v = dev->_counterValue + value;
	v = v < 0 ? 0 : v;
	X9C__setValue(dev, v);
}

//
// Reset the counter value directly (absolute change)
//

void X9C_reset(X9C_t * dev, int value) {
	if(value > 49) X9C__reset(dev, X9C_MIN);
	else X9C__reset(dev, X9C_MAX);
	X9C__setValue(dev, value);
}


//
// Write current setting to non-volatile memory
//

void X9C_writeNVM(X9C_t * dev) {
	// Select the chip
	// Mustn't assert !CS with !INC asserted.
	gpio_set_level(dev->_incrementPin, X9C_DEASSERT);
	gpio_set_level(dev->_deviceSelectPin, X9C_ASSERT);
	// The value of the counter is stored in non-volatile memory whenever !CS transitions
	// high while the !INC input is also high.
	delayMicroseconds(1);
	gpio_set_level(dev->_deviceSelectPin, X9C_DEASSERT);
	// The chip is now saving the counter value. As noted below, this is slow. 20 whole
	// milleseconds of slow, as compared to 100 nanoseconds deselect time if not storing.
	delayMicroseconds(20000);
	// Put the !INC input low again, Now we're back to normal.
	gpio_set_level(dev->_incrementPin, X9C_ASSERT);
}

//
// Private functions
//

//
// Step the chip counter to the desired value
//

void X9C__setValue(X9C_t * dev, uint8_t value) {
	uint8_t steps;
	value = value > 99 ? 99 : value;
	if(value > dev->_counterValue) {
		X9C__setupChange(dev, X9C_UP);
		// Get number of steps
		steps = value - dev->_counterValue;
	} else if(value < dev->_counterValue) {
		X9C__setupChange(dev, X9C_DOWN);
		// Get number of steps
		steps = dev->_counterValue - value;
	} else {
		// Nothing to do
		return;
	}
	// Set the requested value by incrementing or decrementing from the current value.
	X9C__doChange(dev, steps);
	dev->_counterValue = value;
}

//
// Prepare the chip before changing the counter.
//

void X9C__setupChange(X9C_t * dev, uint8_t direction) {
	// Set the direction pin for up/down
	gpio_set_level(dev->_upDownPin, direction);
	// Mustn't assert !CS with !INC asserted.
	// The datasheet says goblins appear if you do.
	// Well, "(not recommended)" at least. It's also
	// worth noting that if you do do it, it moves the
	// wiper. I've seen so many bits of code that miss that.
	gpio_set_level(dev->_incrementPin, X9C_DEASSERT);
	// Select the chip
	gpio_set_level(dev->_deviceSelectPin, X9C_ASSERT);
}

//
// Pulse !INC, 1us high-going.
//

void X9C__pulseInc(X9C_t * dev) {
	// We send a high-going pulse, the chip triggers on the falling edge.
	// We're one step closer
	gpio_set_level(dev->_incrementPin, X9C_DEASSERT);
	delayMicroseconds(1);
	gpio_set_level(dev->_incrementPin, X9C_ASSERT);
	delayMicroseconds(1);
}

//
// Send a string of pulses on !INC, then sanely deselect the chip.
//

void X9C__doChange(X9C_t * dev, uint8_t steps) {
	for(uint8_t i = 0; i < steps; i++)
		X9C__pulseInc(dev);
	// We leave !INC low after the pulseInc() call. That may seem strange,
	// but !INC _MUST_ be low before deasserting !CS, or the chip will save
	// the current value to non-volatile memory. This will wear out if we
	// abuse it. It is also slooooooow as f**k.
	gpio_set_level(dev->_deviceSelectPin, X9C_DEASSERT);
}

void X9C__reset(X9C_t * dev, bool minMax) {
	if(minMax == X9C_MIN) {
		dev->_counterValue = 99;
		X9C__setValue(dev, 0);
	} else {
		dev->_counterValue = 0;
		X9C__setValue(dev, 99);
	}
}



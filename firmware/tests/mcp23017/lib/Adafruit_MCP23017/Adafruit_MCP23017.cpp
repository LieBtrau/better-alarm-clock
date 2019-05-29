/*************************************************** 
 This is a library for the MCP23017 i2c port expander

 These displays use I2C to communicate, 2 pins are required to
 interface
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifdef __AVR
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif
#include "Adafruit_MCP23017.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// minihelper to keep Arduino backward compatibility
static inline void wiresend(uint8_t x)
{
#if ARDUINO >= 100
	Wire.write((uint8_t)x);
#else
	Wire.send(x);
#endif
}

static inline uint8_t wirerecv(void)
{
#if ARDUINO >= 100
	return Wire.read();
#else
	return Wire.receive();
#endif
}

/**
 * Bit number associated to a give Pin
 */
uint8_t Adafruit_MCP23017::bitForPin(uint8_t pin)
{
	return pin % 8;
}

/**
 * Register address, port dependent, for a given PIN
 */
uint8_t Adafruit_MCP23017::regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr)
{
	return (pin < 8) ? portAaddr : portBaddr;
}

/**
 * Reads a given register
 */
uint8_t Adafruit_MCP23017::readRegister(uint8_t addr)
{
	// read the current GPINTEN
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(addr);
	Wire.endTransmission();
	Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 1);
	return wirerecv();
}

/**
 * Writes a given register
 */
void Adafruit_MCP23017::writeRegister(uint8_t regAddr, uint8_t regValue)
{
	// Write the register
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(regAddr);
	wiresend(regValue);
	Wire.endTransmission();
}

/**
 * Helper to update a single bit of an A/B register.
 * - Reads the current register value
 * - Writes the new register value
 */
void Adafruit_MCP23017::updateRegisterBit(uint8_t pin, uint8_t pValue, uint8_t portAaddr, uint8_t portBaddr)
{
	uint8_t regValue;
	uint8_t regAddr = regForPin(pin, portAaddr, portBaddr);
	uint8_t bit = bitForPin(pin);
	regValue = readRegister(regAddr);

	// set the value for the particular bit
	bitWrite(regValue, bit, pValue);

	writeRegister(regAddr, regValue);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the MCP23017 given its HW selected address, see datasheet for Address selection.
 */
void Adafruit_MCP23017::begin(uint8_t addr)
{
	if (addr > 7)
	{
		addr = 7;
	}
	i2caddr = addr;

	Wire.begin();

	// set defaults!
	// all inputs on port A and B
	writeRegister(MCP23017_IODIRA, 0xff);
	writeRegister(MCP23017_IODIRB, 0xff);
}

/**
 * Initializes the default MCP23017, with 000 for the configurable part of the address
 */
void Adafruit_MCP23017::begin(void)
{
	begin(0);
}

/**
 * Sets the pin mode to either INPUT or OUTPUT
 */
void Adafruit_MCP23017::pinMode(uint8_t p, uint8_t d)
{
	switch (d)
	{
	case INPUT:
		updateRegisterBit(p, 1, MCP23017_IODIRA, MCP23017_IODIRB);
		updateRegisterBit(p, 0, MCP23017_GPPUA, MCP23017_GPPUB);
		break;
	case INPUT_PULLUP:
		updateRegisterBit(p, 1, MCP23017_IODIRA, MCP23017_IODIRB);
		updateRegisterBit(p, 1, MCP23017_GPPUA, MCP23017_GPPUB);
		break;
	case OUTPUT:
		updateRegisterBit(p, 0, MCP23017_IODIRA, MCP23017_IODIRB);
		break;
	}
}

/**
 * Reads all 16 pins (port A and B) into a single 16 bits variable.
 */
uint16_t Adafruit_MCP23017::readGPIOAB()
{
	uint16_t ba = 0;
	uint8_t a;

	// read the current GPIO output latches
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(MCP23017_GPIOA);
	Wire.endTransmission();

	Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 2);
	a = wirerecv();
	ba = wirerecv();
	ba <<= 8;
	ba |= a;

	return ba;
}

uint8_t Adafruit_MCP23017::readPinMode(uint8_t b)
{

	// read the current GPIO output latches
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	if (b == 0)
		wiresend(MCP23017_IODIRA);
	else
	{
		wiresend(MCP23017_IODIRB);
	}
	Wire.endTransmission();

	Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 1);
	return wirerecv();
}

/**
 * Read a single port, A or B, and return its current 8 bit value.
 * Parameter b should be 0 for GPIOA, and 1 for GPIOB.
 */
uint8_t Adafruit_MCP23017::readGPIO(uint8_t b)
{
	// read the current GPIO output latches
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	if (b == 0)
		wiresend(MCP23017_GPIOA);
	else
	{
		wiresend(MCP23017_GPIOB);
	}
	Wire.endTransmission();

	Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 1);
	return wirerecv();
}

void Adafruit_MCP23017::writeGPIO(uint8_t b, uint8_t data)
{
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(b == 0 ? MCP23017_GPIOA : MCP23017_GPIOB);
	wiresend(data);
	Wire.endTransmission();
}

void Adafruit_MCP23017::writePinMode(uint8_t b, uint8_t data)
{
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(b == 0 ? MCP23017_IODIRA : MCP23017_IODIRB);
	wiresend(data);
	Wire.endTransmission();
}

/**
 * Writes all the pins in one go. This method is very useful if you are implementing a multiplexed matrix and want to get a decent refresh rate.
 */
void Adafruit_MCP23017::writeGPIOAB(uint16_t ba)
{
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(MCP23017_GPIOA);
	wiresend(ba & 0xFF);
	wiresend(ba >> 8);
	Wire.endTransmission();
}

void Adafruit_MCP23017::digitalWrite(uint8_t pin, uint8_t d)
{
	uint8_t gpio;
	uint8_t bit = bitForPin(pin);

	// read the current GPIO output latches
	uint8_t regAddr = regForPin(pin, MCP23017_OLATA, MCP23017_OLATB);
	gpio = readRegister(regAddr);

	// set the pin and direction
	bitWrite(gpio, bit, d);

	// write the new GPIO
	regAddr = regForPin(pin, MCP23017_GPIOA, MCP23017_GPIOB);
	writeRegister(regAddr, gpio);
}

void Adafruit_MCP23017::writePullUps(uint8_t b, uint8_t data)
{
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	wiresend(b == 0 ? MCP23017_GPPUA : MCP23017_GPPUB);
	wiresend(data);
	Wire.endTransmission();
}

uint8_t Adafruit_MCP23017::digitalRead(uint8_t pin)
{
	uint8_t bit = bitForPin(pin);
	uint8_t regAddr = regForPin(pin, MCP23017_GPIOA, MCP23017_GPIOB);
	return (readRegister(regAddr) >> bit) & 0x1;
}

/**
 * Configures the interrupt system. both port A and B are assigned the same configuration.
 * Mirroring will OR both INTA and INTB pins.
 * Opendrain will set the INT pin to value or open drain.
 * polarity will set LOW or HIGH on interrupt.
 * Default values after Power On Reset are: (false,false, LOW)
 * If you are connecting the INTA/B pin to arduino 2/3, you should configure the interupt handling as FALLING with
 * the default configuration.
 */
void Adafruit_MCP23017::setupInterrupts(uint8_t mirroring, uint8_t openDrain, uint8_t polarity)
{
	// configure the port A
	uint8_t ioconfValue = readRegister(MCP23017_IOCONA);
	bitWrite(ioconfValue, 6, mirroring);
	bitWrite(ioconfValue, 2, openDrain);
	bitWrite(ioconfValue, 1, polarity);
	writeRegister(MCP23017_IOCONA, ioconfValue);

	// Configure the port B
	ioconfValue = readRegister(MCP23017_IOCONB);
	bitWrite(ioconfValue, 6, mirroring);
	bitWrite(ioconfValue, 2, openDrain);
	bitWrite(ioconfValue, 1, polarity);
	writeRegister(MCP23017_IOCONB, ioconfValue);
}

/**
 * Set's up a pin for interrupt. uses arduino MODEs: CHANGE, FALLING, RISING.
 *
 * Note that the interrupt condition finishes when you read the information about the port / value
 * that caused the interrupt or you read the port itself. Check the datasheet can be confusing.
 *
 */
void Adafruit_MCP23017::setupInterruptPin(uint8_t pin, uint8_t mode)
{
	bool levelTriggered;
	bool defaultPinLevel;
	switch (mode)
	{
	case CHANGE:
		levelTriggered = false;
		break;
	case HIGH:
		levelTriggered = true;
		defaultPinLevel = false;
		break;
	case LOW:
		levelTriggered = true;
		defaultPinLevel = true;
		break;
	default:
		while (true)
		{
			//Unsupported by MCP23017
		}
		break;
	}
	// set the pin interrupt control (0 means change, 1 means compare against given value);
	// 1 =  Pin value is compared against the associated bit in the DEFVAL register.
	updateRegisterBit(pin, levelTriggered, MCP23017_INTCONA, MCP23017_INTCONB);
	// if the mode is not CHANGE, we need to set up a default value, different value triggers interrupt
	updateRegisterBit(pin, defaultPinLevel, MCP23017_DEFVALA, MCP23017_DEFVALB);
	// 1 =  Enables GPIO input pin for interrupt-on-change event
	updateRegisterBit(pin, HIGH, MCP23017_GPINTENA, MCP23017_GPINTENB);
}

uint8_t Adafruit_MCP23017::getLastInterruptPin()
{
	uint8_t intf;

	// try port A
	intf = readRegister(MCP23017_INTFA);
	for (int i = 0; i < 8; i++)
		if (bitRead(intf, i))
			return i;

	// try port B
	intf = readRegister(MCP23017_INTFB);
	for (int i = 0; i < 8; i++)
		if (bitRead(intf, i))
			return i + 8;

	return MCP23017_INT_ERR;
}
uint8_t Adafruit_MCP23017::getLastInterruptPinValue()
{
	uint8_t intPin = getLastInterruptPin();
	if (intPin != MCP23017_INT_ERR)
	{
		uint8_t intcapreg = regForPin(intPin, MCP23017_INTCAPA, MCP23017_INTCAPB);
		uint8_t bit = bitForPin(intPin);
		return (readRegister(intcapreg) >> bit) & (0x01);
	}

	return MCP23017_INT_ERR;
}

uint8_t Adafruit_MCP23017::readInterruptCapture(uint8_t b)
{
	// read the current GPIO output latches
	Wire.beginTransmission(MCP23017_ADDRESS | i2caddr);
	if (b == 0)
		wiresend(MCP23017_INTCAPA);
	else
	{
		wiresend(MCP23017_INTCAPB);
	}
	Wire.endTransmission();

	Wire.requestFrom(MCP23017_ADDRESS | i2caddr, 1);
	return wirerecv();
}
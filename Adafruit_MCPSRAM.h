#include <Arduino.h>

#define K640_READ 0x03
#define K640_WRITE 0x02
#define K640_RDSR 0x05
#define K640_WRSR 0x01

#define K640_SEQUENTIAL_MODE (1 << 6)

class Adafruit_MCPSRAM {
 public:
	Adafruit_MCPSRAM(int8_t mosi, int8_t miso, int8_t sck, int8_t cs);
	Adafruit_MCPSRAM(int8_t cs);
	~Adafruit_MCPSRAM() {}
		
	void begin();
	
	uint8_t readStatus();
	void write(uint16_t addr, uint8_t *buf, uint16_t num, uint8_t reg = K640_WRITE);
	void read(uint16_t addr, uint8_t *buf, uint16_t num, uint8_t reg = K640_READ);
	void erase(uint16_t addr, uint16_t length, uint8_t val = 0x00);
	
	uint8_t read8(uint16_t addr, uint8_t reg = K640_READ);
	uint16_t read16(uint16_t addr);
	
	void write8(uint16_t addr, uint8_t val, uint8_t reg = K640_WRITE);
	void write16(uint16_t addr, uint16_t val);
	
	void csHigh();
	void csLow();
	
 private:
	  boolean hwSPI;
#ifdef HAVE_PORTREG
	  PortReg *mosiport, *clkport, *csport, *misoport;
	  PortMask mosipinmask, clkpinmask, cspinmask, misopinmask;
#endif
	int8_t _cs, _mosi, _miso, _sck;
};
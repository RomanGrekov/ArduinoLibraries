#ifndef EEPROM_CLI_H
#define EEPROM_CLI_H

#include <nrf_client.h>
#include <ArduinoLog.h>
#include <Wire.h>

#define EEPROM_ADDR 0x50

#define KEY_ADDR 0x0000
#define KEY_SAVED_FLAF_ADDR (KEY_ADDR + KEY_SIZE)
#define KEY_SAVED_FLAG_VAL 0xAA
#define FIRST_RUN_ADDR (KEY_SAVED_FLAF_ADDR + 1)
#define FIRST_RUN_VAL 0xBB

class EepromCli
{
    public:
        EepromCli(Logging &log, uint8 scl, uint8 sda);
        uint8_t read_byte(uint16_t addr, uint8_t *dest);
        uint8_t write_byte(uint16_t addr, uint8_t data);
        uint8_t read_key(uint8_t *key);
        uint8_t write_key(uint8_t *key);
        uint8_t is_key_saved(bool *result);
        uint8_t set_key_saved(void);
        uint8_t set_key_unsaved(void);
        
    private:
        TwoWire _eeprom;
        Logging& _log;
        void _set_addr(uint16_t address);
};


#endif

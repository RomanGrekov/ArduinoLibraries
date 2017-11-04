#ifndef EEPROM_CLI_H
#define EEPROM_CLI_H

#include <EEPROM.h>
#include <nrf_client.h>
#include <ArduinoLog.h>


#define FIRST_RUN_ADDR 0x0
#define KEY_SAVED_FLAG_ADDR (FIRST_RUN_ADDR+1)
#define KEY_ADDR (KEY_SAVED_FLAG_ADDR+1)

#define FIRST_RUN_VAL 0xBBBB
#define KEY_SAVED_FLAG_VAL 0xAAAA

class EepromCli
{
    public:
        EepromCli(Logging &log);
        void print_conf(void);
        void init(void);
        uint16_t read_key(unsigned char *key, unsigned int keysize);
        uint16_t write_key(unsigned char *key, unsigned int keysize);
        uint16_t init_key(unsigned char *key, unsigned int keysize);
        uint16_t is_key_saved(bool *result);
        uint16_t set_key_saved(void);
        uint16_t set_key_unsaved(void);
        uint16_t read(uint16_t addr, uint16_t *data);
        uint16_t write(uint16_t addr, uint16_t data);
        
    private:
        EEPROMClass _eeprom;
        Logging& _log;

};


#endif

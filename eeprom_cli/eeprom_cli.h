#ifndef EEPROM_CLI_H
#define EEPROM_CLI_H

#include <EEPROM.h>
#include <nrf_client.h>
#include <ArduinoLog.h>

#define STRUCT_START_ADDR 0x0

#define FIRST_RUN_VAL 0xBBBB
#define KEY_SAVED_FLAG_VAL 0xAAAA

typedef struct Configuration {
    uint16_t first_run_flag;
    uint16_t key_saved_flag;
    uint16_t key[KEY_SIZE];
};

class EepromCli
{
    public:
        EepromCli(Logging &log);
        void print_conf(void);
        void init(void);
        uint16_t init_key(unsigned char *key, unsigned int keysize);
        uint16_t save_key(unsigned char *key, unsigned int keysize);
        uint16_t is_key_saved(bool *result);
        uint16_t set_key_saved(void);
        uint16_t set_key_unsaved(void);
        
    private:
        EEPROMClass _eeprom;
        Logging& _log;
        Configuration _conf;
        uint16_t _read_struct(void);
        uint16_t _write_struct(void);

};


#endif

#include "eeprom_cli.h"
#include <EEPROM.h>


EepromCli::EepromCli(Logging &log)
    : _eeprom()
    , _log(log)
{
    _eeprom.init();
    _log = log;

}

void EepromCli::init(void)
{
    uint16_t data;
    uint16_t status = read(FIRST_RUN_ADDR, &data);
    if (status == EEPROM_BAD_ADDRESS || status == EEPROM_NO_VALID_PAGE || data != FIRST_RUN_VAL){
        _log.error("Status: %X, First run val: %X"CR, status, data);
        _eeprom.format();
        for (uint16_t i=0; i < EEPROM_PAGE_SIZE; i++)
             _eeprom.write(i, 0xFFFF);
        status = _eeprom.write(FIRST_RUN_ADDR, FIRST_RUN_VAL);
        _log.trace("Updated first run status: %X"CR, status);
    }
}

void EepromCli::print_conf(void)
{
    _log.notice("FLASH START ADDR: %X"CR, EEPROM_START_ADDRESS);
    _log.notice("FLASH PAGE0 ADDR: %X"CR, EEPROM_PAGE0_BASE);
    _log.notice("FLASH PAGE1 ADDR: %X"CR, EEPROM_PAGE1_BASE);
    _log.notice("FLASH PAGE SIZE: %X"CR, EEPROM_PAGE_SIZE);
}

uint16_t EepromCli::is_key_saved(bool *result)
{
    uint16_t status;
    uint16_t data;
    status = read(KEY_SAVED_FLAG_ADDR, &data);
    if (status != 0){
        *result = false;
        return 1;
    }
    _log.trace("Key saved flag value: %X"CR, data);
    if (data != KEY_SAVED_FLAG_VAL) *result = false;
    else *result = true;
    return 0;
}

uint16_t EepromCli::set_key_saved(void)
{
    return write(KEY_SAVED_FLAG_ADDR, KEY_SAVED_FLAG_VAL);
}

uint16_t EepromCli::set_key_unsaved(void)
{
    return write(KEY_SAVED_FLAG_ADDR, 0x0);
}

uint16_t EepromCli::init_key(unsigned char *key, unsigned int keysize)
{
    uint16_t status;
    bool key_s_result;

    _log.notice("Initialize key from FLASH..."CR);
    status = is_key_saved(&key_s_result);
    if (status != 0){
        _log.error("Failed to read key saved status!"CR);
        return 1;
    }
    if (key_s_result){
        status = read_key(key, keysize);
        if (status != 0){
            _log.error("Failed to read key from FLASH!"CR);
            return 1;
        }
        return 0;
    }
    return 1;
}

uint16_t EepromCli::read_key(unsigned char *key, unsigned int keysize)
{
    uint16_t status;
    uint16_t data;

    for (uint16_t i=0; i<keysize; i++){
        status = read(KEY_ADDR + i, &data);
        key[i] = data;
        if (status != 0) return 1;
    }
    return 0;
}

uint16_t EepromCli::write_key(unsigned char *key, unsigned int keysize)
{
    uint16_t status;

    for (uint16_t i=0; i<keysize; i++){
        status = write(KEY_ADDR + i, key[i]);    
        if (status != 0) return 1;
    }
    return 0;
}

uint16_t EepromCli::read(uint16_t addr, uint16_t *data){
    _log.trace("Read addr: %X"CR, addr);
    uint16_t status = _eeprom.read(addr, data);
    if (status == EEPROM_BAD_ADDRESS || status == EEPROM_NO_VALID_PAGE){
        _log.error("Failed to read data: %X"CR, status);
        return 1;
    }
    return 0;

}

uint16_t EepromCli::write(uint16_t addr, uint16_t data){
    _log.trace("Write addr: %X"CR, addr);
    _eeprom.write(addr, 0xFFFF);
    uint16_t status = _eeprom.write(addr, data);
    if (status != EEPROM_SAME_VALUE && status != FLASH_COMPLETE && status != 0) {
        _log.error("Failed to write data: %X"CR, status);
        return 1;
    }
    return 0;
}

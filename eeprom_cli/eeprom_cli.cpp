#include "eeprom_cli.h"
#include <EEPROM.h>


EepromCli::EepromCli(Logging &log)
    : _eeprom()
    , _log(log)
{
    _eeprom.init();
    _log = log;

}

uint16_t EepromCli::_read_struct(void)
{
    uint16_t status;
    uint16_t *ptr = (uint16_t *)&_conf;
    for (uint16_t i=0; i<sizeof(_conf); i++)
    {
        status = _eeprom.read(STRUCT_START_ADDR + i, ptr+i);
        if (status == EEPROM_BAD_ADDRESS || status == EEPROM_NO_VALID_PAGE){
            _log.error("Failed to read data: %X"CR, status);
            return 1;
        }
    }
    
    return 0;
}

uint16_t EepromCli::_write_struct(void)
{
    uint16_t status;
    uint16_t *ptr = (uint16_t *)&_conf;

    _log.trace("Format flash"CR);
    _eeprom.format();

    for (uint16_t i=0; i<sizeof(_conf); i++)
    {
        status = _eeprom.write(STRUCT_START_ADDR + i, *(ptr+i));
        if (status != EEPROM_SAME_VALUE && status != FLASH_COMPLETE && status != 0) {
            _log.error("Failed to write data: %X"CR, status);
            return 1;
        }
    }
    
    return 0;
}

void EepromCli::init(void)
{
    _log.trace("Initialize flash..."CR);
    uint16_t status = _read_struct();
    if (status == EEPROM_BAD_ADDRESS || status == EEPROM_NO_VALID_PAGE || _conf.first_run_flag != FIRST_RUN_VAL){
        _log.trace("Format flash"CR);
        _eeprom.format();
        _conf.first_run_flag = FIRST_RUN_VAL;
        status = _write_struct();
        if (status != 0) _log.error("Failed to save first run flag!"CR); 
    }
    _log.trace("Ok"CR);
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

    _log.trace("Check if key saved..."CR);
    status = _read_struct();
    if (status != 0){
        _log.error("Is key saved. Failed to read struct!"CR);
        return 1;
    }

    if (_conf.key_saved_flag != KEY_SAVED_FLAG_VAL) *result = false;
    else *result = true;
    _log.trace("Ok"CR);
    return 0;
}

uint16_t EepromCli::set_key_saved(void)
{
    uint16_t status;

    _log.trace("Set key saved..."CR);
    status = _read_struct();
    if (status != 0){
        _log.error("Set key saved. Failed to read struct!"CR);
        return 1;
    }

    _conf.key_saved_flag = KEY_SAVED_FLAG_VAL;

    status = _write_struct();
    if (status !=0){
        _log.error("Set key saved. Failed to write struct!"CR);
        return 1;
    }
    _log.trace("Ok"CR);
    return 0;
}

uint16_t EepromCli::set_key_unsaved(void)
{
    uint16_t status;

    _log.trace("Set key unsaved..."CR);
    status = _read_struct();
    if (status != 0){
        _log.error("Set key unsaved. Failed to read struct!"CR);
        return 1;
    }

    _conf.key_saved_flag = 0xffff;

    status = _write_struct();
    if (status !=0){
        _log.error("Set key unsaved. Failed to save struct!"CR);
        return 1;
    }
    _log.trace("Ok"CR);
    return 0;
}

uint16_t EepromCli::init_key(unsigned char *key, unsigned int keysize)
{
    uint16_t status;

    _log.trace("Read key..."CR);
    status = _read_struct();
    if (status != 0){
        _log.error("Init key. Failed to read struct!"CR);
        return 1;
    }
    if (_conf.key_saved_flag == KEY_SAVED_FLAG_VAL){
        _log.notice("Initialize key from FLASH..."CR);
        memcpy(key, _conf.key, KEY_SIZE);
        _log.trace("Ok"CR);
        return 0;
    }
    _log.trace("Key absent in flash"CR);
    return 2;
}

uint16_t EepromCli::save_key(unsigned char *key, unsigned int keysize)
{
    uint16_t status;

    _log.trace("Save key..."CR);
    status = _read_struct();
    if (status != 0){
        _log.error("Save key. Failed to read struct!"CR);
        return 1;
    }

    memcpy(_conf.key, key, KEY_SIZE);
    status = _write_struct();
    if (status !=0){
        _log.error("Save key. Failed to save struct!"CR);
        return 1;
    }
    _conf.key_saved_flag = KEY_SAVED_FLAG_VAL;
    status = _write_struct();
    if (status !=0){
        _log.error("Save key. Save flag. Failed to save struct!"CR);
        return 1;
    }
    _log.trace("Ok"CR);
    return 0;
}


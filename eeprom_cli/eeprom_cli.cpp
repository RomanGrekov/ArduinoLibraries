#include "eeprom_cli.h"


EepromCli::EepromCli(Logging &log, uint8 scl, uint8 sda)
    : _eeprom(scl, sda, SOFT_STANDARD)
    , _log(log)
{
    _log = log;
    _eeprom.begin();

}

void EepromCli::_set_addr(uint16_t address)
{
    _eeprom.write((uint8_t)(address >> 8));
    delay(10);
    _eeprom.write((uint8_t)(address & 0b0000000011111111));
    delay(10);
}

uint8_t EepromCli::read_byte(uint16_t addr, uint8_t *dest)
{
    uint8_t error;
    byte data;

    _eeprom.beginTransmission(EEPROM_ADDR);
    _set_addr(addr);
    error = _eeprom.endTransmission();
    delay(10);
    if (error != 0){
        _log.error("EEPROM_CLI: Failed to read byte! Addr: %X. Retcode: %X"CR, addr, error);
        return error;
    }
    _eeprom.requestFrom(EEPROM_ADDR, 1);
    delay(10);
    if (_eeprom.available()){
        data = _eeprom.read();
        _log.trace("READ: addr: %X, data: %X"CR, addr, data);
        *dest = data;
    }
    else{
        _log.error("Wire is unavailable!"CR);
        return 1;
    }
    return 0;
}

uint8_t EepromCli::write_byte(uint16_t addr, uint8_t data)
{
    uint8_t error;

    _eeprom.beginTransmission(EEPROM_ADDR);
    _set_addr(addr);
    _log.trace("WRITE: addr: %X, data: %X"CR, addr, data);
    _eeprom.write(data);
    delay(10);
    error = _eeprom.endTransmission();
    delay(10);
    if (error != 0){
        _log.error("EEPROM_CLI: Failed to write byte! Addr: %X. Retcode: %X"CR, addr, error);
        return error;
    }
    return 0;
}

uint8_t EepromCli::read_key(uint8_t *key)
{
    uint8_t error;
    uint16_t addr;

    _log.notice("EEPROM_CLI: Read key...");
    for (uint16_t i=0; i<KEY_SIZE; i++){
        addr = KEY_ADDR + i;
        error = read_byte(addr, key+i);
        if (error != 0){
            _log.error(" Failed"CR);
            return 1;
        } 
    }
    _log.notice(" Ok"CR);
    
    return 0;
}

uint8_t EepromCli::write_key(uint8_t *key)
{
    uint8_t error;
    uint16_t addr;

    _log.notice("EEPROM_CLI: Write key...");
    for (uint16_t i=0; i<KEY_SIZE; i++){
        addr = KEY_ADDR + i;
        error = write_byte(addr, *(key + i));
        if (error != 0){
            _log.error(" Failed"CR);
            return 1;
        } 
    }
    _log.notice(" Ok"CR);
    
    return 0;
}

uint8_t EepromCli::is_key_saved(bool *result)
{
    uint8_t error;
    uint8_t res;

    _log.notice("EEPROM_CLI: Is key saved...");
    error = read_byte(KEY_SAVED_FLAF_ADDR, &res);
    if (error != 0){
        _log.error(" Failed"CR);
        return 1;
    } 
    _log.notice(" Ok"CR);
    
    if (res != KEY_SAVED_FLAG_VAL) *result = false;
    else *result = true;
    return 0;
}

uint8_t EepromCli::set_key_saved(void)
{
    uint8_t error;

    _log.notice("EEPROM_CLI: Set key saved...");
    error = write_byte(KEY_SAVED_FLAF_ADDR, KEY_SAVED_FLAG_VAL);
    if (error != 0){
        _log.error(" Failed"CR);
        return 1;
    } 
    _log.notice(" Ok"CR);
    return 0;
}

uint8_t EepromCli::set_key_unsaved(void)
{
    uint8_t error;

    _log.notice("EEPROM_CLI: Set key unsaved...");
    error = write_byte(KEY_SAVED_FLAF_ADDR, 0xFF);
    if (error != 0){
        _log.error(" Failed"CR);
        return 1;
    } 
    _log.notice(" Ok"CR);
}


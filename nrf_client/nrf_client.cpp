#include "nrf_client.h"

NRFClient::NRFClient(int CE, int CS, Logging &log, const uint8_t *key)
    : _log(log)
    , _radio(CE, CS)
    , _cipher()
{
    _ce = CE;
    _cs = CS;
    _log = log;
    memcpy(_key, key, KEY_SIZE);
};


void NRFClient::initialise(void){
    _log.notice("NRFClient: Radio init"CR);
    _radio.begin();
    _radio.setRetries(15,15);
    _radio.setChannel(0x4c);
    _radio.setPALevel(RF24_PA_LOW);
}

void NRFClient::init_tx(void)
{
    _log.trace("NRFClient: Init TX mode"CR);
    _radio.openWritingPipe(pipes[0]);
    _radio.openReadingPipe(1,pipes[1]);
}

void NRFClient::init_rx(void)
{
    _log.trace("NRFClient: Init RX mode"CR);
    _radio.openWritingPipe(pipes[1]);
    _radio.openReadingPipe(1,pipes[0]);
}

bool NRFClient::send_data(package &pkg, unsigned char *data, unsigned int size){
    memcpy(pkg.data, data, size);
    return send(pkg);
}

bool NRFClient::send(package &pkg, bool encrypted){
    if (encrypted)  encrypt_pkg(pkg);
    return send_pkg(pkg);
}

bool NRFClient::receive_data(package &pkg, unsigned char *expected_data, unsigned int size, unsigned long timeout){
    bool res = receive(pkg, timeout);
    if (res != true) return false;
    return _is_same(pkg.data, expected_data, size);
}

bool NRFClient::_is_same(unsigned char *arr1, unsigned char *arr2, unsigned int len){
    for (unsigned int i=0; i < len; i++){
        if (arr1[i] != arr2[i]) return false;
    }
    return true;
}

bool NRFClient::receive(package &pkg, unsigned int timeout, bool decrypted){
    bool res = receive_pkg(pkg, timeout);
    if (decrypted)  decrypt_pkg(pkg);
    return res;
}

bool NRFClient::send_pkg(package &pkg){
    bool res;
    _radio.stopListening();
    res = _radio.write(&pkg, sizeof(pkg) );
    if (! res) _log.error("NRFClient: Failed to send package"CR);
    return res;
}

bool NRFClient::receive_pkg(package &pkg, unsigned long timeout){ // If timeout = 0 then infinity
    _radio.startListening();
    //delay(10);

    if (timeout != 0){
        // Wait here until we get a response, or timeout (250ms)
        unsigned long started_waiting_at = millis();
        bool timeout_f = false;
        while ( ! _radio.available() && ! timeout_f )
            if (millis() - started_waiting_at > timeout )
                timeout_f = true;

        // Describe the results
        if ( timeout_f )
        {
            _log.error("NRFClient: Read data from NRF24 timed out"CR);
            return false;
        }
    } else{
        while( ! _radio.available() );
    }
    _log.trace("NRFClient: Reading data..."CR);
    _radio.read(&pkg, sizeof(pkg) );
    return true;
}

void NRFClient::encrypt_pkg(package &pkg){
    _log.trace("NRFClient: Raw data: %y"CR, pkg.data);
    unsigned char buffer[DATA_SIZE];
    memcpy(buffer, pkg.data, DATA_SIZE);
    _cipher.setKey(_key, _cipher.keySize());
    _cipher.encryptBlock(pkg.data, buffer);
    _log.trace("NRFClient: Encrypted data: %y"CR, pkg.data);
}

void NRFClient::decrypt_pkg(package &pkg){
    _log.trace("NRFClient: Raw data: %y"CR, pkg.data);
    unsigned char buffer[DATA_SIZE];
    memcpy(buffer, pkg.data, DATA_SIZE);
    _cipher.setKey(_key, _cipher.keySize());
    _cipher.decryptBlock(pkg.data, buffer);
    _log.trace("NRFClient: Decrypted data: %y"CR, pkg.data);
}

void NRFClient::change_key(const uint8_t *new_key){
    _log.trace("Changing key..."CR);
    memcpy(_key, new_key, KEY_SIZE);
     _log.notice("NRFClient: New key %y"CR, _key);
}


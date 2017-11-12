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


bool NRFClient::_is_same(unsigned char *arr1, unsigned char *arr2, unsigned int len){
    for (unsigned int i=0; i < len; i++){
        if (arr1[i] != arr2[i]){
            _log.error("Expected data different!"CR);
            return false;
        }
    }
    return true;
}

bool NRFClient::send(unsigned char *data, unsigned int size, bool encrypted){
    bool res;
    memcpy(_package.data, data, size);

    if (encrypted)  _encrypt_pkg();

    _radio.stopListening();
    delay(10);
    res = _radio.write(&_package, sizeof(_package) );
    if (! res) _log.error("NRFClient: Failed to send package"CR);
    return res;
}

bool NRFClient::receive_expected(unsigned char *expected_data, unsigned int size, unsigned long timeout, bool decrypted){
    unsigned char data[DATA_SIZE];
    bool res = receive(data, timeout);
    if (res != true) return false;
    return _is_same(data, expected_data, size);
}

bool NRFClient::receive(unsigned char *receive_data, unsigned int timeout, bool decrypted){
    _radio.startListening();
    delay(10);

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
    _radio.read(&_package, sizeof(_package) );

    if (decrypted)  _decrypt_pkg();
    memcpy(receive_data, _package.data, DATA_SIZE);
    return true;
}

void NRFClient::_encrypt_pkg(){
    _log.trace("NRFClient: Raw data: %y"CR, _package.data);
    unsigned char buffer[DATA_SIZE];
    memcpy(buffer, _package.data, DATA_SIZE);
    _cipher.setKey(_key, _cipher.keySize());
    _cipher.encryptBlock(_package.data, buffer);
    _log.trace("NRFClient: Encrypted data: %y"CR, _package.data);
}

void NRFClient::_decrypt_pkg(){
    _log.trace("NRFClient: Raw data: %y"CR, _package.data);
    unsigned char buffer[DATA_SIZE];
    memcpy(buffer, _package.data, DATA_SIZE);
    _cipher.setKey(_key, _cipher.keySize());
    _cipher.decryptBlock(_package.data, buffer);
    _log.trace("NRFClient: Decrypted data: %y"CR, _package.data);
}

void NRFClient::change_key(const uint8_t *new_key){
    _log.trace("Changing key..."CR);
    memcpy(_key, new_key, KEY_SIZE);
     _log.notice("NRFClient: New key %y"CR, _key);
}


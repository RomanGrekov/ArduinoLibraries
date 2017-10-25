#include "nrf_client.h"
#include <encryption.h>

NRFClient::NRFClient(int CE, int CS, Logging &log)
    : _log(log)
    , _radio(CE, CS)
{
    _ce = CE;
    _cs = CS;
    _log = log;
};

void NRFClient::initialise(void){
    _log.notice("Radio init"CR);
    _radio.begin();
    _radio.setRetries(15,15);
    _radio.setChannel(0x4c);
    _radio.setPALevel(RF24_PA_LOW);
}

void NRFClient::init_tx(void)
{
    _log.trace("Init TX mode"CR);
    _radio.openWritingPipe(pipes[0]);
    _radio.openReadingPipe(1,pipes[1]);
}

void NRFClient::init_rx(void)
{
    _log.trace("Init RX mode"CR);
    _radio.openWritingPipe(pipes[1]);
    _radio.openReadingPipe(1,pipes[0]);
}

bool NRFClient::send(package &pkg, bool encrypted){
    if (encrypted)  encrypt_pkg(pkg);
    return send_pkg(pkg);
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
    if (! res) _log.error("Failed to send package"CR);
    return res;
}

bool NRFClient::receive_pkg(package &pkg, unsigned int timeout){ // If timeout = 0 then infinity
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
            _log.error("Failed to read data from NRF24"CR);
            return false;
        }
    } else{
        while( ! _radio.available() );
    }
    _log.trace("Reading data"CR);
    _radio.read(&pkg, sizeof(pkg) );
    return true;
}

void NRFClient::encrypt_pkg(package &pkg){
    _log.trace(F("Raw data: %s"CR), pkg.data);
    unsigned char buffer[DATA_SIZE];
    memcpy(buffer, pkg.data, DATA_SIZE);
    encrypt(buffer, pkg.data);
    _log.trace(F("Encrypted data: %s"CR), pkg.data);
}

void NRFClient::decrypt_pkg(package &pkg){
    _log.trace(F("Raw data: %s"CR), pkg.data);
    unsigned char buffer[DATA_SIZE];
    memcpy(buffer, pkg.data, DATA_SIZE);
    decrypt(pkg.data, buffer);
    _log.trace(F("Decrypted data: %s"CR), pkg.data);
}

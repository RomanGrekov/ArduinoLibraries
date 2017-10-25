#ifndef NRF_CLIENT_H
#define NRF_CLIENT_H

#include <stdbool.h>
#include <nRF24L01_STM32.h>
#include <RF24_STM32.h>
#include <ArduinoLog.h>

#define DATA_SIZE 16
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

typedef void (*printer)(const char *text);

class NRFClient
{
    public:
        NRFClient(int CE, int CS, Logging &log);
        void initialise(void);
        void init_tx(void);
        void init_rx(void);
        struct package {                  // Structure of our payload
          unsigned long node;
          unsigned long counter;
          unsigned char data[DATA_SIZE];
        };
        bool send_pkg(package &pkg);
        bool receive_pkg(package &pkg, unsigned int timeout);
        bool send(package &pkg, bool encrypted = true);
        bool receive(package &pkg, unsigned int timeout, bool decrypted = true);
        void encrypt_pkg(package &pkg);
        void decrypt_pkg(package &pkg);

    private:
        int _ce;
        int _cs;
        Logging& _log;
        RF24 _radio;
};

//
//void decrypt_packet(payload_t *packet);



#endif

#ifndef NRF_CLIENT_H
#define NRF_CLIENT_H

//#include <stdbool.h>
#include <nRF24L01_STM32.h>
#include <RF24_STM32.h>
#include <ArduinoLog.h>
#include <Crypto.h>
#include <AES.h>


#define DATA_SIZE 32
#define KEY_SIZE 32
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
const uint8_t def_key[KEY_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                   0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

typedef void (*printer)(const char *text);

class NRFClient
{
    public:
        NRFClient(int CE, int CS, Logging &log, const uint8_t *key = def_key);
        void initialise(void);
        void init_tx(void);
        void init_rx(void);
        struct package {                  // Structure of our payload
          //unsigned long node;
          //unsigned long counter;
          unsigned char data[DATA_SIZE];
        };
        bool send_pkg(package &pkg);
        bool receive_pkg(package &pkg, unsigned long timeout);
        bool send(package &pkg, bool encrypted = true);
        bool receive(package &pkg, unsigned int timeout, bool decrypted = true);
        void encrypt_pkg(package &pkg);
        void decrypt_pkg(package &pkg);
        void change_key(const uint8_t *new_key);
        bool send_data(package &pkg, unsigned char *data, unsigned int size);
        bool receive_data(package &pkg, unsigned char *expected_data, unsigned int size, unsigned long timeout);

    private:
        int _ce;
        int _cs;
        Logging& _log;
        RF24 _radio;
        AES256 _cipher;
        uint8_t _key[KEY_SIZE];
        bool _is_same(unsigned char *arr1, unsigned char *arr2, unsigned int len);
};

//
//void decrypt_packet(payload_t *packet);



#endif

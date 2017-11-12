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
typedef struct Package {                  // Structure of our payload
    unsigned char data[DATA_SIZE];
};

typedef void (*printer)(const char *text);

class NRFClient
{
    public:
        NRFClient(int CE, int CS, Logging &log, const uint8_t *key = def_key);
        void initialise(void);
        void init_tx(void);
        void init_rx(void);
        void change_key(const uint8_t *new_key);
        bool send(unsigned char *data, unsigned int size, bool decrypted = true);
        bool receive(unsigned char *receive_data, unsigned int timeout, bool decrypted = true);
        bool receive_expected(unsigned char *expected_data, unsigned int size, unsigned long timeout, bool decrypted = true);

    private:
        int _ce;
        int _cs;
        Logging& _log;
        RF24 _radio;
        AES256 _cipher;
        Package _package;
        uint8_t _key[KEY_SIZE];

        void _encrypt_pkg();
        void _decrypt_pkg();
        bool _is_same(unsigned char *arr1, unsigned char *arr2, unsigned int len);
};

//
//void decrypt_packet(payload_t *packet);



#endif

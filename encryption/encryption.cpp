#include "encryption.h"
#include <Crypto.h>
#include <AES.h>

void encrypt(unsigned char *plaintext, unsigned char *ciphertext){
    AES256 cipher;
    cipher.setKey(key, cipher.keySize());
    cipher.encryptBlock(ciphertext, plaintext);
}

void decrypt(unsigned char *plaintext, unsigned char *ciphertext){
    AES256 cipher;
    cipher.setKey(key, cipher.keySize());
    cipher.decryptBlock(plaintext, ciphertext);
}


#include "encryption.h"

#define XOR_KEY 0xAB

/*
 * XOR-based cipher for demonstration purposes.
 * For production use, replace with AES-256-GCM via a library such as OpenSSL.
 * The key 0xAB (1010 1011) never produces a NUL byte when XOR'd with standard
 * printable ASCII input (0x20–0x7E), keeping the result NUL-terminated.
 */
void encrypt_token(const char *input, char *output, size_t len)
{
    size_t i;
    for (i = 0; i < len - 1 && input[i] != '\0'; i++) {
        output[i] = (char)((unsigned char)input[i] ^ XOR_KEY);
    }
    output[i] = '\0';
}

void decrypt_token(const char *input, char *output, size_t len)
{
    /* XOR cipher is symmetric – the same operation decrypts. */
    encrypt_token(input, output, len);
}

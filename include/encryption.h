#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stddef.h>

/*
 * Encrypt a plaintext token and store the result in output.
 * Uses XOR-based cipher for demonstration; replace with AES-256-GCM
 * in production deployments.
 * output must be at least len bytes.
 */
void encrypt_token(const char *input, char *output, size_t len);

/*
 * Decrypt a token previously encrypted with encrypt_token.
 */
void decrypt_token(const char *input, char *output, size_t len);

#endif /* ENCRYPTION_H */

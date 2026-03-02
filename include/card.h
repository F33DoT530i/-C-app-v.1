#ifndef CARD_H
#define CARD_H

#include "credit.h"

/* Length of a standard credit card number (digits only, no spaces) */
#define CARD_NUMBER_LEN 16

/* Formatted card number length: "XXXX XXXX XXXX XXXX\0" */
#define CARD_NUMBER_FMT_LEN 20

/* Expiry string length: "MM/YY\0" + padding */
#define CARD_EXPIRY_LEN 8

/* CVV string length: "XXX\0" + padding */
#define CARD_CVV_LEN 5

typedef struct {
    char number[CARD_NUMBER_FMT_LEN]; /* e.g. "4532 1234 5678 9010" */
    char expiry[CARD_EXPIRY_LEN];     /* e.g. "03/29"               */
    char cvv[CARD_CVV_LEN];           /* e.g. "742"                 */
    CreditLimit limit;
} CreditCard;

/*
 * card_generate - Fill 'card' with a newly generated Visa credit card
 * (16-digit number passing Luhn check, 3-year expiry, random CVV) with
 * the credit limit derived from 'score'.
 *
 * Returns 0 on success, -1 when the application is rejected due to
 * a credit score below the minimum threshold.
 */
int card_generate(int score, CreditCard *card);

/*
 * card_luhn_valid - Return 1 if 'number' (digit characters only, length 16)
 * passes the Luhn check, 0 otherwise.
 */
int card_luhn_valid(const char *number);

/*
 * card_print - Print the card details to stdout.
 */
void card_print(const CreditCard *card);

#endif /* CARD_H */

#include "card.h"
#include "credit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Luhn algorithm helpers                                               */
/* ------------------------------------------------------------------ */

/*
 * luhn_checksum - Compute the Luhn checksum digit for a 15-digit prefix
 * stored in buf[0..14] (ASCII digits).  Returns the single check digit (0-9).
 */
static int luhn_checksum(const char *prefix15)
{
    int sum = 0;
    int double_next = 1; /* rightmost digit of prefix is at an even position */

    for (int i = 14; i >= 0; i--) {
        int d = prefix15[i] - '0';
        if (double_next) {
            d *= 2;
            if (d > 9)
                d -= 9;
        }
        sum += d;
        double_next = !double_next;
    }
    return (10 - (sum % 10)) % 10;
}

int card_luhn_valid(const char *number)
{
    if (!number || strlen(number) != CARD_NUMBER_LEN)
        return 0;

    int sum = 0;
    int double_next = 0;

    for (int i = CARD_NUMBER_LEN - 1; i >= 0; i--) {
        if (number[i] < '0' || number[i] > '9')
            return 0;
        int d = number[i] - '0';
        if (double_next) {
            d *= 2;
            if (d > 9)
                d -= 9;
        }
        sum += d;
        double_next = !double_next;
    }
    return (sum % 10) == 0;
}

/* ------------------------------------------------------------------ */
/* Card generation                                                      */
/* ------------------------------------------------------------------ */

int card_generate(int score, CreditCard *card)
{
    memset(card, 0, sizeof(*card));

    /* Calculate credit limit; returns -1 when rejected */
    if (credit_calculate_limit(score, &card->limit) != 0)
        return -1;

    /* ----- Card number: Visa IIN prefix "4", 14 random digits, 1 Luhn digit */
    char raw[CARD_NUMBER_LEN + 1]; /* 16 digits + NUL */
    raw[0] = '4'; /* Visa network identifier */
    for (int i = 1; i < CARD_NUMBER_LEN - 1; i++)
        raw[i] = '0' + (rand() % 10);
    raw[CARD_NUMBER_LEN - 1] = '0' + (char)luhn_checksum(raw);
    raw[CARD_NUMBER_LEN] = '\0';

    /* Format as "XXXX XXXX XXXX XXXX" */
    snprintf(card->number, sizeof(card->number),
             "%c%c%c%c %c%c%c%c %c%c%c%c %c%c%c%c",
             raw[0],  raw[1],  raw[2],  raw[3],
             raw[4],  raw[5],  raw[6],  raw[7],
             raw[8],  raw[9],  raw[10], raw[11],
             raw[12], raw[13], raw[14], raw[15]);

    /* ----- Expiry: current month + 3 years */
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    /* Clamp to known ranges so the compiler can verify no truncation occurs. */
    unsigned int exp_month = (unsigned int)(tm_now->tm_mon % 12) + 1u; /* 1-12 */
    unsigned int exp_year  = (unsigned int)(tm_now->tm_year + 1900 + 3) % 100u;
    snprintf(card->expiry, sizeof(card->expiry), "%02u/%02u", exp_month, exp_year);

    /* ----- CVV: 3 random digits */
    snprintf(card->cvv, sizeof(card->cvv), "%03d", rand() % 1000);

    return 0;
}

void card_print(const CreditCard *card)
{
    printf("------------------------------\n");
    printf("  Credit Card Details\n");
    printf("------------------------------\n");
    printf("  Number : %s\n", card->number);
    printf("  Expiry : %s\n", card->expiry);
    printf("  CVV    : %s\n", card->cvv);
    printf("------------------------------\n");
    printf("  Credit Tier  : %s\n", credit_tier_name(card->limit.tier));
    printf("  Credit Limit : $%d\n", card->limit.assigned_limit);
    printf("  Limit Range  : $%d - $%d\n",
           card->limit.min_limit, card->limit.max_limit);
    printf("------------------------------\n");
}

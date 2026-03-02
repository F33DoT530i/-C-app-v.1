/*
 * test_card.c – Unit tests for the card generation module.
 * Build: gcc -Wall -Wextra -Iinclude -o build/test_card tests/test_card.c src/card.c src/credit.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"
#include "credit.h"

static int failures = 0;

#define ASSERT(cond, msg)                                              \
    do {                                                               \
        if (!(cond)) {                                                 \
            fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg); \
            failures++;                                                \
        } else {                                                       \
            printf("PASS: %s\n", msg);                                 \
        }                                                              \
    } while (0)

/* ------------------------------------------------------------------ */
/* card_luhn_valid tests                                                */
/* ------------------------------------------------------------------ */

static void test_luhn_known_valid(void)
{
    /* Well-known test numbers that pass Luhn */
    ASSERT(card_luhn_valid("4532015112830366") == 1, "Luhn: known Visa valid");
    ASSERT(card_luhn_valid("4111111111111111") == 1, "Luhn: test Visa valid");
    ASSERT(card_luhn_valid("5500005555555559") == 1, "Luhn: test Mastercard valid");
}

static void test_luhn_known_invalid(void)
{
    ASSERT(card_luhn_valid("4532015112830367") == 0, "Luhn: digit off -> invalid");
    ASSERT(card_luhn_valid("1234567890123456") == 0, "Luhn: sequential digits -> invalid");
}

static void test_luhn_null(void)
{
    ASSERT(card_luhn_valid(NULL) == 0, "Luhn: NULL -> invalid");
}

static void test_luhn_wrong_length(void)
{
    ASSERT(card_luhn_valid("123") == 0,                  "Luhn: too short -> invalid");
    ASSERT(card_luhn_valid("12345678901234567") == 0,    "Luhn: too long -> invalid");
}

/* ------------------------------------------------------------------ */
/* card_generate tests                                                  */
/* ------------------------------------------------------------------ */

static void test_generate_excellent(void)
{
    CreditCard card;
    int ret = card_generate(800, &card);
    ASSERT(ret == 0, "generate excellent: success");

    /* Card number format: 4 groups of 4 separated by spaces */
    ASSERT(strlen(card.number) == 19, "generate: number length 19");
    ASSERT(card.number[4]  == ' ' &&
           card.number[9]  == ' ' &&
           card.number[14] == ' ',   "generate: spaces in correct positions");

    /* Strip spaces and Luhn-check */
    char raw[CARD_NUMBER_LEN + 1];
    int j = 0;
    for (int i = 0; card.number[i]; i++)
        if (card.number[i] != ' ')
            raw[j++] = card.number[i];
    raw[j] = '\0';
    ASSERT(card_luhn_valid(raw) == 1, "generate: number passes Luhn");

    /* Starts with '4' (Visa) */
    ASSERT(card.number[0] == '4', "generate: Visa IIN prefix '4'");

    /* Expiry format MM/YY */
    ASSERT(strlen(card.expiry) == 5, "generate: expiry length 5");
    ASSERT(card.expiry[2] == '/',    "generate: expiry separator '/'");

    /* CVV: 3 digit characters */
    ASSERT(strlen(card.cvv) == 3, "generate: CVV length 3");
    for (int i = 0; i < 3; i++)
        ASSERT(card.cvv[i] >= '0' && card.cvv[i] <= '9', "generate: CVV digit");

    /* Credit limit within excellent range */
    ASSERT(card.limit.tier == CREDIT_TIER_EXCELLENT,    "generate: Excellent tier");
    ASSERT(card.limit.assigned_limit >= 10000 &&
           card.limit.assigned_limit <= 20000,           "generate: limit in Excellent range");
}

static void test_generate_poor(void)
{
    CreditCard card;
    int ret = card_generate(600, &card);
    ASSERT(ret == 0, "generate poor: success");
    ASSERT(card.limit.tier == CREDIT_TIER_POOR, "generate poor: Poor tier");
    ASSERT(card.limit.assigned_limit >= 100 &&
           card.limit.assigned_limit <= 500,    "generate poor: limit in Poor range");
}

static void test_generate_rejected(void)
{
    CreditCard card;
    int ret = card_generate(400, &card);
    ASSERT(ret == -1, "generate rejected: returns -1");
}

/* ------------------------------------------------------------------ */

int main(void)
{
    srand(42);

    printf("--- card_luhn_valid ---\n");
    test_luhn_known_valid();
    test_luhn_known_invalid();
    test_luhn_null();
    test_luhn_wrong_length();

    printf("--- card_generate ---\n");
    test_generate_excellent();
    test_generate_poor();
    test_generate_rejected();

    printf("\n%s – %d failure(s)\n", failures == 0 ? "ALL PASSED" : "SOME FAILED", failures);
    return failures == 0 ? 0 : 1;
}

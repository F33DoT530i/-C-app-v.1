#include <stdio.h>
#include <string.h>

#include "credit_card.h"
#include "payment_account.h"
#include "encryption.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(expr) do { \
    tests_run++; \
    if (expr) { \
        tests_passed++; \
        printf("  PASS: %s\n", #expr); \
    } else { \
        printf("  FAIL: %s (line %d)\n", #expr, __LINE__); \
    } \
} while (0)

/* ------------------------------------------------------------------ */
static void test_payment_account_init(void)
{
    PaymentAccount account;

    printf("Test: payment_account_init\n");

    TEST(payment_account_init(&account, 1, PAYMENT_TYPE_BANK,
                              "Test Bank", "secret123") == 0);
    TEST(account.account_id == 1);
    TEST(account.type       == PAYMENT_TYPE_BANK);
    TEST(strcmp(account.account_name, "Test Bank") == 0);
    TEST(account.is_active  == 1);
    /* Token must be stored encrypted (not in plaintext). */
    TEST(strcmp(account.encrypted_token, "secret123") != 0);

    /* NULL argument handling. */
    TEST(payment_account_init(NULL,    1, PAYMENT_TYPE_BANK, "T", "t") == -1);
    TEST(payment_account_init(&account, 1, PAYMENT_TYPE_BANK, NULL, "t") == -1);
    TEST(payment_account_init(&account, 1, PAYMENT_TYPE_BANK, "T", NULL) == -1);
}

/* ------------------------------------------------------------------ */
static void test_credit_card_init(void)
{
    CreditCard card;

    printf("Test: credit_card_init\n");

    TEST(credit_card_init(&card, 1, "1234567890123456") == 0);
    TEST(card.card_id      == 1);
    TEST(strcmp(card.card_number, "1234567890123456") == 0);
    TEST(card.linked_count == 0);

    TEST(credit_card_init(NULL,  1, "1234567890123456") == -1);
    TEST(credit_card_init(&card, 1, NULL)               == -1);
}

/* ------------------------------------------------------------------ */
static void test_link_unlink(void)
{
    CreditCard     card;
    PaymentAccount acc1, acc2;

    printf("Test: link and unlink payment accounts\n");

    credit_card_init(&card, 1, "1234567890123456");

    payment_account_init(&acc1, 1, PAYMENT_TYPE_BANK,   "Chase Bank",   "tok1");
    payment_account_init(&acc2, 2, PAYMENT_TYPE_PAYPAL, "my@email.com", "tok2");

    TEST(credit_card_link_account(&card, &acc1) == 0);
    TEST(card.linked_count == 1);

    TEST(credit_card_link_account(&card, &acc2) == 0);
    TEST(card.linked_count == 2);

    /* Duplicate link must be rejected. */
    TEST(credit_card_link_account(&card, &acc1) == -1);
    TEST(card.linked_count == 2);

    /* Unlink first account. */
    TEST(credit_card_unlink_account(&card, 1) == 0);
    TEST(card.linked_count == 1);
    TEST(card.linked_accounts[0].account_id == 2);

    /* Unlink non-existent account. */
    TEST(credit_card_unlink_account(&card, 99) == -1);

    /* NULL safety. */
    TEST(credit_card_link_account(NULL,  &acc1) == -1);
    TEST(credit_card_link_account(&card, NULL)  == -1);
    TEST(credit_card_unlink_account(NULL, 1)    == -1);
}

/* ------------------------------------------------------------------ */
static void test_get_linked_accounts(void)
{
    CreditCard     card;
    PaymentAccount acc, out[MAX_LINKED_ACCOUNTS];

    printf("Test: credit_card_get_linked_accounts\n");

    credit_card_init(&card, 1, "1234567890123456");
    payment_account_init(&acc, 1, PAYMENT_TYPE_CASHAPP, "$handle", "tok");
    credit_card_link_account(&card, &acc);

    TEST(credit_card_get_linked_accounts(&card, out, MAX_LINKED_ACCOUNTS) == 1);
    TEST(out[0].account_id == 1);
    TEST(out[0].type       == PAYMENT_TYPE_CASHAPP);

    /* Invalid arguments. */
    TEST(credit_card_get_linked_accounts(NULL,  out, MAX_LINKED_ACCOUNTS) == -1);
    TEST(credit_card_get_linked_accounts(&card, NULL, MAX_LINKED_ACCOUNTS) == -1);
    TEST(credit_card_get_linked_accounts(&card, out, 0)                   == -1);
}

/* ------------------------------------------------------------------ */
static void test_payment_type_conversion(void)
{
    printf("Test: payment type conversion\n");

    TEST(string_to_payment_type("bank")    == PAYMENT_TYPE_BANK);
    TEST(string_to_payment_type("paypal")  == PAYMENT_TYPE_PAYPAL);
    TEST(string_to_payment_type("cashapp") == PAYMENT_TYPE_CASHAPP);
    TEST(string_to_payment_type("other")   == PAYMENT_TYPE_UNKNOWN);
    TEST(string_to_payment_type(NULL)      == PAYMENT_TYPE_UNKNOWN);

    TEST(strcmp(payment_type_to_string(PAYMENT_TYPE_BANK),    "Bank Account") == 0);
    TEST(strcmp(payment_type_to_string(PAYMENT_TYPE_PAYPAL),  "PayPal")       == 0);
    TEST(strcmp(payment_type_to_string(PAYMENT_TYPE_CASHAPP), "Cash App")     == 0);
}

/* ------------------------------------------------------------------ */
static void test_encryption(void)
{
    char        encrypted[256] = {0};
    char        decrypted[256] = {0};
    const char *plaintext      = "my_secret_token_123";

    printf("Test: token encryption / decryption\n");

    encrypt_token(plaintext, encrypted, sizeof(encrypted));
    TEST(strcmp(encrypted, plaintext) != 0);  /* must differ from plaintext */

    decrypt_token(encrypted, decrypted, sizeof(decrypted));
    TEST(strcmp(decrypted, plaintext) == 0);  /* round-trip must restore original */
}

/* ------------------------------------------------------------------ */
static void test_max_linked_accounts(void)
{
    CreditCard     card;
    PaymentAccount acc;
    char           name[32];
    int            i;

    printf("Test: MAX_LINKED_ACCOUNTS limit\n");

    credit_card_init(&card, 1, "1234567890123456");

    for (i = 0; i < MAX_LINKED_ACCOUNTS; i++) {
        snprintf(name, sizeof(name), "Account %d", i);
        payment_account_init(&acc, i + 1, PAYMENT_TYPE_BANK, name, "token");
        TEST(credit_card_link_account(&card, &acc) == 0);
    }

    /* One more must be rejected. */
    payment_account_init(&acc, MAX_LINKED_ACCOUNTS + 1,
                         PAYMENT_TYPE_PAYPAL, "Extra", "token");
    TEST(credit_card_link_account(&card, &acc) == -1);
    TEST(card.linked_count == MAX_LINKED_ACCOUNTS);
}

/* ------------------------------------------------------------------ */
int main(void)
{
    printf("=== C-App Payment Account Tests ===\n\n");

    test_payment_account_init();
    test_credit_card_init();
    test_link_unlink();
    test_get_linked_accounts();
    test_payment_type_conversion();
    test_encryption();
    test_max_linked_accounts();

    printf("\n=== Results: %d/%d tests passed ===\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}

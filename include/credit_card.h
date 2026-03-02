#ifndef CREDIT_CARD_H
#define CREDIT_CARD_H

#include "payment_account.h"

#define CARD_NUMBER_LEN 16

typedef struct {
    int            card_id;
    char           card_number[CARD_NUMBER_LEN + 1];
    PaymentAccount linked_accounts[MAX_LINKED_ACCOUNTS];
    int            linked_count;
} CreditCard;

/* Initialise a CreditCard.  Returns 0 on success, -1 on invalid arguments. */
int credit_card_init(CreditCard *card, int card_id, const char *card_number);

/*
 * Link an external payment account to this card.
 * Fails when the card is already at MAX_LINKED_ACCOUNTS or the account is
 * already linked.  Returns 0 on success, -1 on error.
 */
int credit_card_link_account(CreditCard *card, PaymentAccount *account);

/*
 * Remove a previously linked account by account_id.
 * Returns 0 on success, -1 when not found.
 */
int credit_card_unlink_account(CreditCard *card, int account_id);

/* Print a summary of all linked accounts to stdout. */
void credit_card_print_linked_accounts(const CreditCard *card);

/*
 * Copy up to max_count linked accounts into the caller-supplied array.
 * Returns the number of accounts copied, or -1 on invalid arguments.
 */
int credit_card_get_linked_accounts(const CreditCard *card,
                                    PaymentAccount *accounts, int max_count);

#endif /* CREDIT_CARD_H */

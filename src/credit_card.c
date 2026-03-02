#include "credit_card.h"

#include <stdio.h>
#include <string.h>

int credit_card_init(CreditCard *card, int card_id, const char *card_number)
{
    if (card == NULL || card_number == NULL) {
        return -1;
    }

    card->card_id      = card_id;
    card->linked_count = 0;

    strncpy(card->card_number, card_number, sizeof(card->card_number) - 1);
    card->card_number[sizeof(card->card_number) - 1] = '\0';

    memset(card->linked_accounts, 0, sizeof(card->linked_accounts));

    return 0;
}

int credit_card_link_account(CreditCard *card, PaymentAccount *account)
{
    int i;

    if (card == NULL || account == NULL) {
        return -1;
    }

    if (card->linked_count >= MAX_LINKED_ACCOUNTS) {
        fprintf(stderr,
                "Error: maximum linked accounts (%d) reached for card %d\n",
                MAX_LINKED_ACCOUNTS, card->card_id);
        return -1;
    }

    /* Reject duplicate account IDs. */
    for (i = 0; i < card->linked_count; i++) {
        if (card->linked_accounts[i].account_id == account->account_id) {
            fprintf(stderr,
                    "Error: account %d is already linked to card %d\n",
                    account->account_id, card->card_id);
            return -1;
        }
    }

    card->linked_accounts[card->linked_count] = *account;
    card->linked_count++;

    return 0;
}

int credit_card_unlink_account(CreditCard *card, int account_id)
{
    int i;

    if (card == NULL) {
        return -1;
    }

    for (i = 0; i < card->linked_count; i++) {
        if (card->linked_accounts[i].account_id == account_id) {
            /* Shift remaining entries left. */
            int j;
            for (j = i; j < card->linked_count - 1; j++) {
                card->linked_accounts[j] = card->linked_accounts[j + 1];
            }
            card->linked_count--;
            return 0;
        }
    }

    fprintf(stderr, "Error: account %d not found on card %d\n",
            account_id, card->card_id);
    return -1;
}

void credit_card_print_linked_accounts(const CreditCard *card)
{
    int i;

    if (card == NULL) {
        return;
    }

    printf("Credit Card: **** **** **** %s\n",
           card->card_number + CARD_NUMBER_LEN - 4);
    printf("Linked accounts (%d):\n", card->linked_count);

    if (card->linked_count == 0) {
        printf("  (none)\n");
        return;
    }

    for (i = 0; i < card->linked_count; i++) {
        const PaymentAccount *acc = &card->linked_accounts[i];
        printf("  [%d] %s – %s\n",
               acc->account_id,
               payment_type_to_string(acc->type),
               acc->account_name);
    }
}

int credit_card_get_linked_accounts(const CreditCard *card,
                                    PaymentAccount *accounts, int max_count)
{
    int count;
    int i;

    if (card == NULL || accounts == NULL || max_count <= 0) {
        return -1;
    }

    count = card->linked_count < max_count ? card->linked_count : max_count;
    for (i = 0; i < count; i++) {
        accounts[i] = card->linked_accounts[i];
    }

    return count;
}

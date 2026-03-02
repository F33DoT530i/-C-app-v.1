#include <stdio.h>
#include <string.h>

#include "credit_card.h"
#include "payment_account.h"

/* Simple in-memory store for demonstration. */
#define MAX_CARDS 10

static CreditCard cards[MAX_CARDS];
static int        card_count    = 0;
static int        next_acct_id  = 1;

static CreditCard *find_card(int card_id)
{
    int i;
    for (i = 0; i < card_count; i++) {
        if (cards[i].card_id == card_id) {
            return &cards[i];
        }
    }
    return NULL;
}

/* Simulates the /link-payment-account API endpoint. */
static int api_link_payment_account(int card_id, const char *type_str,
                                    const char *account_name, const char *token)
{
    PaymentAccount account;
    PaymentType    type;
    CreditCard    *card = find_card(card_id);

    if (card == NULL) {
        fprintf(stderr, "Error: card %d not found\n", card_id);
        return -1;
    }

    type = string_to_payment_type(type_str);
    if (type == PAYMENT_TYPE_UNKNOWN) {
        fprintf(stderr, "Error: unknown payment type '%s'\n", type_str);
        return -1;
    }

    if (account_name == NULL || account_name[0] == '\0') {
        fprintf(stderr, "Error: account name cannot be empty\n");
        return -1;
    }

    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "Error: authentication token cannot be empty\n");
        return -1;
    }

    if (payment_account_init(&account, next_acct_id, type,
                             account_name, token) != 0) {
        return -1;
    }

    if (credit_card_link_account(card, &account) != 0) {
        return -1;
    }

    printf("Linked %s account '%s' (id: %d) to card %d\n",
           payment_type_to_string(type), account_name,
           next_acct_id, card_id);
    next_acct_id++;
    return 0;
}

/* Simulates the /fetch-linked-accounts API endpoint. */
static int api_fetch_linked_accounts(int card_id)
{
    CreditCard *card = find_card(card_id);

    if (card == NULL) {
        fprintf(stderr, "Error: card %d not found\n", card_id);
        return -1;
    }

    credit_card_print_linked_accounts(card);
    return 0;
}

static void print_usage(void)
{
    printf("Commands:\n");
    printf("  link   <card_id> <type> <account_name> <token>\n");
    printf("         type: bank | paypal | cashapp\n");
    printf("  fetch  <card_id>\n");
    printf("  unlink <card_id> <account_id>\n");
    printf("  cards\n");
    printf("  quit\n");
}

int main(void)
{
    char line[512];
    char cmd[64];

    /* Initialise two demo cards. */
    credit_card_init(&cards[0], 1, "1234567890123456");
    credit_card_init(&cards[1], 2, "9876543210987654");
    card_count = 2;

    printf("C-App Payment Account Linker\n");
    printf("Type 'help' for usage information.\n\n");

    while (fgets(line, sizeof(line), stdin) != NULL) {
        cmd[0] = '\0';
        sscanf(line, "%63s", cmd);

        if (cmd[0] == '\0') {
            continue;
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "help") == 0) {
            print_usage();
        } else if (strcmp(cmd, "cards") == 0) {
            int i;
            printf("Available cards:\n");
            for (i = 0; i < card_count; i++) {
                printf("  [%d] **** **** **** %s\n",
                       cards[i].card_id,
                       cards[i].card_number + CARD_NUMBER_LEN - 4);
            }
        } else if (strcmp(cmd, "link") == 0) {
            int  card_id;
            char type[32], name[MAX_NAME_LEN], token[MAX_TOKEN_LEN];
            if (sscanf(line, "%*s %d %31s %127s %255s",
                       &card_id, type, name, token) != 4) {
                fprintf(stderr,
                        "Usage: link <card_id> <type> <account_name> <token>\n");
            } else {
                api_link_payment_account(card_id, type, name, token);
            }
        } else if (strcmp(cmd, "fetch") == 0) {
            int card_id;
            if (sscanf(line, "%*s %d", &card_id) != 1) {
                fprintf(stderr, "Usage: fetch <card_id>\n");
            } else {
                api_fetch_linked_accounts(card_id);
            }
        } else if (strcmp(cmd, "unlink") == 0) {
            int        card_id, account_id;
            CreditCard *card;
            if (sscanf(line, "%*s %d %d", &card_id, &account_id) != 2) {
                fprintf(stderr, "Usage: unlink <card_id> <account_id>\n");
            } else {
                card = find_card(card_id);
                if (card == NULL) {
                    fprintf(stderr, "Error: card %d not found\n", card_id);
                } else if (credit_card_unlink_account(card, account_id) == 0) {
                    printf("Unlinked account %d from card %d\n",
                           account_id, card_id);
                }
            }
        } else {
            fprintf(stderr, "Unknown command: %s\n", cmd);
        }
    }

    printf("Goodbye!\n");
    return 0;
}


#include "payment_account.h"
#include "encryption.h"

#include <stdio.h>
#include <string.h>

const char *payment_type_to_string(PaymentType type)
{
    switch (type) {
        case PAYMENT_TYPE_BANK:    return "Bank Account";
        case PAYMENT_TYPE_PAYPAL:  return "PayPal";
        case PAYMENT_TYPE_CASHAPP: return "Cash App";
        default:                   return "Unknown";
    }
}

PaymentType string_to_payment_type(const char *str)
{
    if (str == NULL)                  return PAYMENT_TYPE_UNKNOWN;
    if (strcmp(str, "bank") == 0)     return PAYMENT_TYPE_BANK;
    if (strcmp(str, "paypal") == 0)   return PAYMENT_TYPE_PAYPAL;
    if (strcmp(str, "cashapp") == 0)  return PAYMENT_TYPE_CASHAPP;
    return PAYMENT_TYPE_UNKNOWN;
}

int payment_account_init(PaymentAccount *account, int account_id,
                         PaymentType type, const char *account_name,
                         const char *token)
{
    if (account == NULL || account_name == NULL || token == NULL) {
        return -1;
    }

    account->account_id = account_id;
    account->type       = type;
    account->is_active  = 1;

    strncpy(account->account_name, account_name, MAX_NAME_LEN - 1);
    account->account_name[MAX_NAME_LEN - 1] = '\0';

    /* Encrypt the authentication token before storing it. */
    encrypt_token(token, account->encrypted_token, MAX_TOKEN_LEN);

    return 0;
}

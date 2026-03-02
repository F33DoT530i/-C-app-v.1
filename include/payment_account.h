#ifndef PAYMENT_ACCOUNT_H
#define PAYMENT_ACCOUNT_H

#define MAX_LINKED_ACCOUNTS 10
#define MAX_TOKEN_LEN       256
#define MAX_NAME_LEN        128

typedef enum {
    PAYMENT_TYPE_BANK    = 0,
    PAYMENT_TYPE_PAYPAL,
    PAYMENT_TYPE_CASHAPP,
    PAYMENT_TYPE_UNKNOWN
} PaymentType;

typedef struct {
    int         account_id;
    PaymentType type;
    char        account_name[MAX_NAME_LEN];
    char        encrypted_token[MAX_TOKEN_LEN];
    int         is_active;
} PaymentAccount;

/* Convert between PaymentType and its string representation. */
const char *payment_type_to_string(PaymentType type);
PaymentType string_to_payment_type(const char *str);

/*
 * Initialise a PaymentAccount.  The token is encrypted before storage.
 * Returns 0 on success, -1 on invalid arguments.
 */
int payment_account_init(PaymentAccount *account, int account_id,
                         PaymentType type, const char *account_name,
                         const char *token);

#endif /* PAYMENT_ACCOUNT_H */

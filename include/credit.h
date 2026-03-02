#ifndef CREDIT_H
#define CREDIT_H

/* Credit score tiers */
typedef enum {
    CREDIT_TIER_EXCELLENT = 0, /* 750+          */
    CREDIT_TIER_GOOD,          /* 700-749        */
    CREDIT_TIER_FAIR,          /* 650-699        */
    CREDIT_TIER_POOR,          /* below 650      */
    CREDIT_TIER_REJECTED       /* below threshold*/
} CreditTier;

/* Credit limit range assigned to a user */
typedef struct {
    int min_limit;
    int max_limit;
    int assigned_limit; /* randomly selected within [min_limit, max_limit] */
    CreditTier tier;
} CreditLimit;

/* Minimum credit score accepted (applications below are rejected) */
#define CREDIT_SCORE_MIN_ACCEPTED 500

/* Score tier boundaries */
#define CREDIT_SCORE_EXCELLENT 750
#define CREDIT_SCORE_GOOD      700
#define CREDIT_SCORE_FAIR      650

/*
 * credit_get_tier - Return the credit tier for a given score.
 * Returns CREDIT_TIER_REJECTED when score < CREDIT_SCORE_MIN_ACCEPTED.
 */
CreditTier credit_get_tier(int score);

/*
 * credit_calculate_limit - Populate 'out' with the limit range and a
 * randomly assigned limit for the given credit score.
 * Returns 0 on success, -1 if the application is rejected.
 */
int credit_calculate_limit(int score, CreditLimit *out);

/*
 * credit_tier_name - Return a human-readable name for the tier.
 */
const char *credit_tier_name(CreditTier tier);

#endif /* CREDIT_H */

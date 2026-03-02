#include "credit.h"
#include <stdlib.h>
#include <string.h>

CreditTier credit_get_tier(int score)
{
    if (score < CREDIT_SCORE_MIN_ACCEPTED)
        return CREDIT_TIER_REJECTED;
    if (score >= CREDIT_SCORE_EXCELLENT)
        return CREDIT_TIER_EXCELLENT;
    if (score >= CREDIT_SCORE_GOOD)
        return CREDIT_TIER_GOOD;
    if (score >= CREDIT_SCORE_FAIR)
        return CREDIT_TIER_FAIR;
    return CREDIT_TIER_POOR;
}

/*
 * Limit ranges per tier (inclusive on both ends):
 *   Excellent  750+  : $10,000 – $20,000
 *   Good       700-749: $5,000 – $10,000
 *   Fair       650-699: $1,000 – $5,000
 *   Poor       500-649: $100   – $500
 *   Rejected   <500   : application denied
 */
static void tier_range(CreditTier tier, int *min, int *max)
{
    switch (tier) {
    case CREDIT_TIER_EXCELLENT:
        *min = 10000; *max = 20000; break;
    case CREDIT_TIER_GOOD:
        *min = 5000;  *max = 10000; break;
    case CREDIT_TIER_FAIR:
        *min = 1000;  *max = 5000;  break;
    case CREDIT_TIER_POOR:
        *min = 100;   *max = 500;   break;
    default:
        *min = 0; *max = 0; break;
    }
}

int credit_calculate_limit(int score, CreditLimit *out)
{
    CreditTier tier = credit_get_tier(score);

    memset(out, 0, sizeof(*out));
    out->tier = tier;

    if (tier == CREDIT_TIER_REJECTED)
        return -1;

    tier_range(tier, &out->min_limit, &out->max_limit);

    /* Assign a limit uniformly at random within the tier's range. */
    int range = out->max_limit - out->min_limit;
    out->assigned_limit = out->min_limit + rand() % (range + 1);

    return 0;
}

const char *credit_tier_name(CreditTier tier)
{
    switch (tier) {
    case CREDIT_TIER_EXCELLENT: return "Excellent";
    case CREDIT_TIER_GOOD:      return "Good";
    case CREDIT_TIER_FAIR:      return "Fair";
    case CREDIT_TIER_POOR:      return "Poor";
    case CREDIT_TIER_REJECTED:  return "Rejected";
    default:                    return "Unknown";
    }
}

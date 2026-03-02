/*
 * test_credit.c – Unit tests for the credit scoring module.
 * Build: gcc -Wall -Wextra -Iinclude -o build/test_credit tests/test_credit.c src/credit.c
 */
#include <stdio.h>
#include <stdlib.h>
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
/* credit_get_tier tests                                                */
/* ------------------------------------------------------------------ */

static void test_tier_excellent(void)
{
    ASSERT(credit_get_tier(850) == CREDIT_TIER_EXCELLENT, "score 850 -> Excellent");
    ASSERT(credit_get_tier(750) == CREDIT_TIER_EXCELLENT, "score 750 -> Excellent");
}

static void test_tier_good(void)
{
    ASSERT(credit_get_tier(749) == CREDIT_TIER_GOOD, "score 749 -> Good");
    ASSERT(credit_get_tier(700) == CREDIT_TIER_GOOD, "score 700 -> Good");
}

static void test_tier_fair(void)
{
    ASSERT(credit_get_tier(699) == CREDIT_TIER_FAIR, "score 699 -> Fair");
    ASSERT(credit_get_tier(650) == CREDIT_TIER_FAIR, "score 650 -> Fair");
}

static void test_tier_poor(void)
{
    ASSERT(credit_get_tier(649) == CREDIT_TIER_POOR, "score 649 -> Poor");
    ASSERT(credit_get_tier(500) == CREDIT_TIER_POOR, "score 500 -> Poor");
}

static void test_tier_rejected(void)
{
    ASSERT(credit_get_tier(499) == CREDIT_TIER_REJECTED, "score 499 -> Rejected");
    ASSERT(credit_get_tier(300) == CREDIT_TIER_REJECTED, "score 300 -> Rejected");
}

/* ------------------------------------------------------------------ */
/* credit_calculate_limit tests                                         */
/* ------------------------------------------------------------------ */

static void test_limit_excellent(void)
{
    CreditLimit lim;
    int ret = credit_calculate_limit(800, &lim);
    ASSERT(ret == 0,                              "excellent: no rejection");
    ASSERT(lim.tier == CREDIT_TIER_EXCELLENT,     "excellent: correct tier");
    ASSERT(lim.min_limit == 10000,                "excellent: min limit $10,000");
    ASSERT(lim.max_limit == 20000,                "excellent: max limit $20,000");
    ASSERT(lim.assigned_limit >= 10000 &&
           lim.assigned_limit <= 20000,           "excellent: assigned within range");
}

static void test_limit_good(void)
{
    CreditLimit lim;
    int ret = credit_calculate_limit(720, &lim);
    ASSERT(ret == 0,                           "good: no rejection");
    ASSERT(lim.tier == CREDIT_TIER_GOOD,       "good: correct tier");
    ASSERT(lim.min_limit == 5000,              "good: min limit $5,000");
    ASSERT(lim.max_limit == 10000,             "good: max limit $10,000");
    ASSERT(lim.assigned_limit >= 5000 &&
           lim.assigned_limit <= 10000,        "good: assigned within range");
}

static void test_limit_fair(void)
{
    CreditLimit lim;
    int ret = credit_calculate_limit(670, &lim);
    ASSERT(ret == 0,                          "fair: no rejection");
    ASSERT(lim.tier == CREDIT_TIER_FAIR,      "fair: correct tier");
    ASSERT(lim.min_limit == 1000,             "fair: min limit $1,000");
    ASSERT(lim.max_limit == 5000,             "fair: max limit $5,000");
    ASSERT(lim.assigned_limit >= 1000 &&
           lim.assigned_limit <= 5000,        "fair: assigned within range");
}

static void test_limit_poor(void)
{
    CreditLimit lim;
    int ret = credit_calculate_limit(600, &lim);
    ASSERT(ret == 0,                          "poor: no rejection");
    ASSERT(lim.tier == CREDIT_TIER_POOR,      "poor: correct tier");
    ASSERT(lim.min_limit == 100,              "poor: min limit $100");
    ASSERT(lim.max_limit == 500,              "poor: max limit $500");
    ASSERT(lim.assigned_limit >= 100 &&
           lim.assigned_limit <= 500,         "poor: assigned within range");
}

static void test_limit_rejected(void)
{
    CreditLimit lim;
    int ret = credit_calculate_limit(400, &lim);
    ASSERT(ret == -1,                         "rejected: returns -1");
    ASSERT(lim.tier == CREDIT_TIER_REJECTED,  "rejected: correct tier");
}

/* ------------------------------------------------------------------ */
/* credit_tier_name tests                                               */
/* ------------------------------------------------------------------ */

static void test_tier_names(void)
{
    ASSERT(credit_tier_name(CREDIT_TIER_EXCELLENT)[0] == 'E', "tier name Excellent");
    ASSERT(credit_tier_name(CREDIT_TIER_GOOD)[0]      == 'G', "tier name Good");
    ASSERT(credit_tier_name(CREDIT_TIER_FAIR)[0]      == 'F', "tier name Fair");
    ASSERT(credit_tier_name(CREDIT_TIER_POOR)[0]      == 'P', "tier name Poor");
    ASSERT(credit_tier_name(CREDIT_TIER_REJECTED)[0]  == 'R', "tier name Rejected");
}

/* ------------------------------------------------------------------ */

int main(void)
{
    srand(42);

    printf("--- credit_get_tier ---\n");
    test_tier_excellent();
    test_tier_good();
    test_tier_fair();
    test_tier_poor();
    test_tier_rejected();

    printf("--- credit_calculate_limit ---\n");
    test_limit_excellent();
    test_limit_good();
    test_limit_fair();
    test_limit_poor();
    test_limit_rejected();

    printf("--- credit_tier_name ---\n");
    test_tier_names();

    printf("\n%s – %d failure(s)\n", failures == 0 ? "ALL PASSED" : "SOME FAILED", failures);
    return failures == 0 ? 0 : 1;
}

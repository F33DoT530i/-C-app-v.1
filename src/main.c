#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "credit.h"
#include "card.h"

int main(void)
{
    srand((unsigned int)time(NULL));

    printf("=== Credit Card Generator ===\n\n");
    printf("Enter your credit score (300-850): ");

    int score = 0;
    if (scanf("%d", &score) != 1 || score < 300 || score > 850) {
        fprintf(stderr, "Error: invalid credit score. Must be between 300 and 850.\n");
        return 1;
    }

    CreditCard card;
    int result = card_generate(score, &card);

    if (result != 0) {
        printf("\nApplication rejected: credit score %d is below the minimum accepted threshold (%d).\n",
               score, CREDIT_SCORE_MIN_ACCEPTED);
        printf("Please improve your credit score and try again.\n");
        return 2;
    }

    printf("\nCredit card successfully generated:\n\n");
    card_print(&card);

    return 0;
}

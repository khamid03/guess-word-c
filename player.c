
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "player.h"
#include "chlng.h"

player_t* player_new(void) {
    player_t* p = (player_t*)malloc(sizeof(player_t));
    if (!p) return NULL;

    p->solved = 0;
    p->total = 0;
    p->finished = false;
    p->chlng = chlng_new();
    return p;
}

void player_reset(player_t* p) {
    if (!p) return;
    p->solved = 0;
    p->total = 0;
    p->finished = false;
    chlng_reset(p->chlng);
}

void player_del(player_t* p) {
    if (!p) return;
    chlng_del(p->chlng);
    free(p);
}

int player_fetch_chlng(player_t* p) {
    if (!p) return -1;
    chlng_reset(p->chlng);
    if (chlng_fetch_text(p->chlng) < 0) return -1;
    if (chlng_hide_word(p->chlng) < 0) return -1;

    p->total++;
    return 0;
}

int player_get_greeting(player_t* p, char** msg) {
    if (!p || !msg) return -1;
    *msg = strdup("M: Guess the missing ____!\nM: Send your guess in the form 'R: word\\r\\n'.\n");
    return *msg ? 1 : -1;
}

int player_get_challenge(player_t* p, char** msg) {
    if (!p || !msg) return -1;
    if (player_fetch_chlng(p) < 0) return -1;

    size_t len = strlen(p->chlng->text) + 4;
    *msg = (char*)malloc(len);
    if (!*msg) return -1;
    snprintf(*msg, len, "C: %s\n", p->chlng->text);
    return 1;
}

int player_post_challenge(player_t* p, char* response, char** msg) {
    if (!p || !response || !msg) return -1;
    if (strncmp(response, "Q:", 2) == 0) {
        p->finished = true;
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "M: You mastered %d/%d challenges! Good bye!\n", p->solved, p->total);
        *msg = strdup(buffer);
        return *msg ? 1 : -1;
    }
    if (strncmp(response, "R: ", 3) != 0) {
        *msg = strdup("M: Invalid response format.\n");
        return *msg ? 1 : -1;
    }

    char* guess = response + 3;
    guess[strcspn(guess, "\r\n")] = '\0';
    char buffer[256];
    if (strcmp(guess, p->chlng->word) == 0) {
        p->solved++;
        snprintf(buffer, sizeof(buffer), "O: Congratulation- challenge passed!\n");
    } else {
        snprintf(buffer, sizeof(buffer), "F: Wrong guess '%s'- expected '%s'\n", guess, p->chlng->word);
    }
    *msg = strdup(buffer);
    return *msg ? 1 : -1;
}





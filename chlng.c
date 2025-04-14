#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chlng.h"

chlng_t* chlng_new(void) {
    chlng_t* c = (chlng_t*)malloc(sizeof(chlng_t));
    if (!c) return NULL;
    c->text = NULL;
    c->word = NULL;
    return c;
}

void chlng_reset(chlng_t* c) {
    if (!c) return;
    free(c->text);
    free(c->word);
    c->text = NULL;
    c->word = NULL;
}

void chlng_del(chlng_t* c) {
    if (!c) return;
    chlng_reset(c);
    free(c);
}

int chlng_fetch_text(chlng_t* c) {
    if (!c) return -1;
    FILE* fp = popen("fortune -s", "r");
    if (!fp) return -1;
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        pclose(fp);
        return -1;
    }
    pclose(fp);
    c->text = strdup(buffer);
    return c->text ? 0 : -1;
}

int chlng_hide_word(chlng_t* c) {
    if (!c || !c->text) return -1;

    char* words[50];
    int count = 0;
    char* token = strtok(c->text, " \t\n");
    while (token && count < 50) {
        words[count++] = token;
        token = strtok(NULL, " \t\n");
    }

    if (count == 0) return -1;
    srand(time(NULL));
    int idx = rand() % count;

    c->word = strdup(words[idx]);
    if (!c->word) return -1;
    size_t len = strlen(c->word);
    char* underscores = (char*)malloc(len + 1);
    if (!underscores) return -1;

    memset(underscores, '_', len);
    underscores[len] = '\0';
    words[idx] = underscores;
    size_t new_text_len = 1;
    for (int i = 0; i < count; i++) {
        new_text_len += strlen(words[i]) + 1;
    }

    char* new_text = (char*)malloc(new_text_len);
    if (!new_text) {
        free(underscores);
        return -1;
    }
    new_text[0] = '\0';
    for (int i = 0; i < count; i++) {
        strcat(new_text, words[i]);
        if (i < count - 1) strcat(new_text, " ");
    }
    free(c->text);
    c->text = new_text;
    free(underscores);
    return 0;
}


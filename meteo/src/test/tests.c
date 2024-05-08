#include <criterion/criterion.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include "../../headers/datastrcts.h"
#include "../../headers/util.h"
#include "../../headers/config.h"

Test(config, get_config) {
    char* cfgpath = malloc(512 * sizeof(char));
    readlink("/proc/self/exe", cfgpath, 256);
    cfgpath = dirname(cfgpath);
    strcat(cfgpath, "/testcfg.cfg");

    struct config* parsed_config = get_config(cfgpath);
    char* tmpstring = malloc(256 * sizeof(char));
    char* tmpstring2 = malloc(128 * sizeof(char));
    int iterator = 0;
    do {
        sprintf(tmpstring, "Test%d", iterator+1);
        cr_assert(strcmp(parsed_config->key, tmpstring) == 0);
        sprintf(tmpstring, "%d %d", iterator, 10 + 5 * iterator);
        
        // Config can contain non-stripped values
        strcpy(tmpstring2, parsed_config->value);
        strip(tmpstring2);
        
        cr_assert(strcmp(tmpstring2, tmpstring) == 0);
        iterator++;
        parsed_config = parsed_config->next;
    } while (parsed_config != NULL);
    free(tmpstring);
}

Test(util, strip) {
    char* strings[8] = {
        "   a   ",
        "a",
        "  a",
        "a  ",
        "",
        "   #@!#@!   ",
        "#@!#@!\n\n\n   ",
        " a \n\n b"
    };
    char* correct_strings[8] = {
        "a",
        "a",
        "a",
        "a",
        "",
        "#@!#@!",
        "#@!#@!",
        "a \n\n b"
    };

    for (int i=0; i<8; i++){
        char* strtocheck = malloc(strlen(strings[i]) * sizeof(char));
        strcpy(strtocheck, strings[i]);
        strip(strtocheck);
        cr_assert(strcmp(strtocheck, correct_strings[i]) == 0);
        free(strtocheck);
    }
}

Test(util, substring) {
    char* strings[4] = {
        "",
        "1234567",
        "1",
        "547"
    };
    char* correct_strings[4] = {
        "",
        "345",
        "1",
        "5"
    };
    
    char* strtocheck = malloc(strlen(strings[0]) * sizeof(char));
    substring(strings[0], 4, 1, strtocheck);
    cr_assert(strcmp(strtocheck, correct_strings[0]) == 0);
    free(strtocheck);
    
    strtocheck = malloc(strlen(strings[1]) * sizeof(char));
    substring(strings[1], 2, 3, strtocheck);
    cr_assert(strcmp(strtocheck, correct_strings[1]) == 0);
    free(strtocheck);
    
    strtocheck = malloc(strlen(strings[2]) * sizeof(char));
    substring(strings[2], 0, 1, strtocheck);
    cr_assert(strcmp(strtocheck, correct_strings[2]) == 0);
    free(strtocheck);
    
    strtocheck = malloc(strlen(strings[3]) * sizeof(char));
    substring(strings[3], 0, 1, strtocheck);
    cr_assert(strcmp(strtocheck, correct_strings[3]) == 0);
    free(strtocheck);
}

Test(util, findchr) {
    char* strings[6] = {
        "abcd",
        "*&^$",
        "!",
        "",
        "::abababa",
        "abcdefgh"
    };

    cr_assert(findchr(strings[0], "c") == 2);
    cr_assert(findchr(strings[1], "*") == 0);
    cr_assert(findchr(strings[2], "!") == 0);
    cr_assert(findchr(strings[3], "z") == -1);
    cr_assert(findchr(strings[4], "a") == 2);
    cr_assert(findchr(strings[5], "i") == -1);
}


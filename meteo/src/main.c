#include "../headers/meteo.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    setvbuf(stdout, NULL, _IONBF, 0);
    return main_wrapped(argc, argv);
}
#define NOBUILD_IMPLEMENTATION
#include "nobuild.h"

int main()
{
    CMD("gcc", "-o", "soundplot", "-lraylib", "-ggdb", "main.c");
}


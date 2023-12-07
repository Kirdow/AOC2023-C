#ifndef _AOC_UTIL_H
#define _AOC_UTIL_H

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

static FILE* getfile(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: run <filename.txt>\n");
        exit(1);
    }

    FILE* file = fopen(argv[1], "r");
    if (!file)
    {
        printf("Failed to open file\n");
        exit(1);
    }

    return file;
}

static bool is_str_empty(const char* str)
{
    const char* ptr = str;
    while (*ptr != '\0')
    {
        if (!isspace(*ptr)) return false;
        ++ptr;
    }

    return true;
}

static bool any_of(char c, const char* list)
{
    const char* ptr = list;
    while (*ptr != '\0')
    {
        if (*ptr == c) return true;
        ++ptr;
    }

    return false;
}

static uint64_t get_num_next(const char* ptr, const char** result, const char* until)
{
    const char* tmp = ptr;
    size_t len = 0;
    while (!any_of(*tmp, until))
    {
        if (*tmp >= 48 && *tmp <= 57)
            ++len;
        ++tmp;
    }

    char buffer[len + 1];
    tmp = ptr;
    len = 0;
    while (!any_of(*tmp, until))
    {
        if (*tmp >= 48 && *tmp <= 57)
            buffer[len++] = *tmp;
        ++tmp;
    }

    char* end;
    uint64_t num = (uint64_t)strtoll(buffer, &end, 10);
    *result = tmp;
    return num;
}

static bool skip_chars(const char** ptr, const char* list)
{
    const char* tmp = *ptr;
    while (*tmp != '\0' && any_of(*tmp, list)) ++tmp;
    *ptr = tmp;
    return !any_of(*tmp, list);
}

static bool until_chars(const char** ptr, const char* list)
{
    const char* tmp = *ptr;
    while (*tmp != '\0' && !any_of(*tmp, list)) ++tmp;
    *ptr = tmp;
    if (*tmp == '\0') return false;
    return any_of(*tmp, list);
}

static uint64_t expand(void** data, uint64_t* count, uint64_t size)
{
    if (*data == NULL)
        *data = malloc(size * (*count + 1));
    else
        *data = realloc(*data, size * (*count + 1));
    uint64_t index = *count;
    ++(*count);
    return index;
}

#endif

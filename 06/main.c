#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../shared/util.h"

#define BUFFER_SIZE 512

typedef struct {
    uint64_t time;
    uint64_t dist;
} entry_t;

typedef struct {
    entry_t* data;
    uint64_t count;
} racelist_t;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: run <filename.txt>\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file)
    {
        printf("Failed to open file\n");
        return 1;
    }

    racelist_t races = {NULL, 0};

    const char* line;
    char buffer[BUFFER_SIZE];
    if (!fgets(buffer, sizeof(buffer), file))
    {
        printf("Failed to read time line\n");
        fclose(file);
        return 1;
    }

    line = buffer;
    until_chars(&line, ":");
    ++line;
    
    while (*line != '\0')
    {
        skip_chars(&line, " ");
        uint64_t num = get_num_next(line, &line, "\n");
        skip_chars(&line, " \n");

        uint64_t index = expand((void**)&races.data, &races.count, sizeof(entry_t));

        races.data[index].time = num;
    }

    if (!fgets(buffer, sizeof(buffer), file))
    {
        printf("Failed to read distance line\n");
        fclose(file);
        return 1;
    }

    line = buffer;
    until_chars(&line, ":");
    ++line;

    for (uint64_t index = 0; index < races.count; ++index)
    {
        skip_chars(&line, " ");
        uint64_t num = get_num_next(line, &line, "\n");
        skip_chars(&line, " \n");

        races.data[index].dist = num;
    }

    fclose(file);

    uint64_t total = 0;
    for (uint64_t race_id = 0; race_id < races.count; ++race_id)
    {
        entry_t* entry = &races.data[race_id];
        uint64_t num = 0;
        while (num < entry->time)
        {
            if (num * (entry->time - num) >= entry->dist)
                break;
            ++num;
        }

        uint64_t leeway = entry->time - num * 2 + 1;
        printf("Margin for race %zu: %zu\n", race_id + 1, leeway);
        total = (race_id == 0) ? leeway : (total * leeway);
    }

    printf("Total Margin: %zu\n", total);

    return 0;
}


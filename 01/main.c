#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct {
    const char* name;
    int32_t value;
} lookup_t;

#define LOOKUP_SIZE 9

static const lookup_t lookup_values[LOOKUP_SIZE] = {
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9}
};

int32_t lookup_value(const char* loc)
{
    for (size_t i = 0; i < LOOKUP_SIZE; i++)
    {
        const lookup_t* entry = &lookup_values[i];
        if (strncmp(loc, entry->name, strlen(entry->name)) == 0)
            return entry->value;
    }

    return -1;
}

int32_t get_number(const char* line)
{
    int32_t first = -1;
    int32_t last = -1;

    const char* ptr = line;
    while (*ptr != '\0')
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            if (first == -1) first = *ptr - 48;
            last = *ptr - 48;
        }
        else
        {
            int32_t value = lookup_value(ptr);
            if (value != -1)
            {
                if (first == -1) first = value;
                last = value;
            }
        }

        ++ptr;
    }
    
    return first * 10 + last;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: run <file.txt>\n");
        return 1;
    }

    FILE *file = NULL;
    char line[BUFFER_SIZE];

    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    int32_t total = 0;
    while (fgets(line, sizeof(line), file))
    {
        int32_t num = get_number(line);
        printf("%d\n", num);
        total += num;
    }

    printf("Total: %d\n", total);

    fclose(file);
    return 0;
}

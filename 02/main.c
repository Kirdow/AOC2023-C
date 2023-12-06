#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

typedef struct {
    uint32_t game_id;
    uint32_t max_blue;
    uint32_t max_green;
    uint32_t max_red;
} game_t;

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

static uint32_t get_num_next(const char* ptr, const char** result, const char* until)
{
    const char* tmp = ptr;
    size_t len = 0;
    while (!any_of(*tmp, until))
    {
        ++len;
        ++tmp;
    }

    char buffer[len + 1];
    strncpy(buffer, ptr, sizeof(buffer));
    char* end;
    uint32_t num = (uint32_t)strtol(buffer, &end, 10);
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

static game_t get_game(const char* line)
{
    game_t game = { -1, 0, 0, 0 };
    if (strncmp(line, "Game ", 5) != 0) return game;

    line += 5;
    uint32_t id = get_num_next(line, &line, ":");
    game.game_id = id;
    skip_chars(&line, ": ");
    uint32_t red = 0, green = 0, blue = 0;
    while (*line >= 48 && *line <= 57)
    {
        uint32_t unit = get_num_next(line, &line, " ");
        skip_chars(&line, " ");
        if (strncmp(line, "red", 3) == 0)
        {
            line += 3;
            red += unit;
        }
        else if (strncmp(line, "green", 5) == 0)
        {
            line += 5;
            green += unit;
        }
        else if (strncmp(line, "blue", 4) == 0)
        {
            line += 4;
            blue += unit;
        }
        else
        {
            printf("Error parsing color: %s\n", line);
            exit(1);
        }

        if (*line == ';' || *line == '\n')
        {
            if (game.max_red < red) game.max_red = red;
            if (game.max_green < green) game.max_green = green;
            if (game.max_blue < blue) game.max_blue = blue;

            red = 0;
            green = 0;
            blue = 0;
        }

        if (*line == '\n')
            return game;

        skip_chars(&line, ",; ");
    }
    return game;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: run <file.txt>\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    char line[BUFFER_SIZE];

    uint32_t max_red = 12;
    uint32_t max_green = 13;
    uint32_t max_blue = 14;

    uint32_t total_id = 0;
    uint32_t total_power = 0;
    while (fgets(line, sizeof(line), file))
    {
        game_t game = get_game(line);
        uint32_t power = game.max_red * game.max_green * game.max_blue;
        if (game.max_red <= max_red && game.max_green <= max_green && game.max_blue <= max_blue)
        {
            printf("Possible: %d\n", game.game_id);
            total_id += game.game_id;
        }
        else
            printf("Not possible: %d\n", game.game_id);
        printf("Power: %d\n", power);
        total_power += power;
    }

    fclose(file);

    printf("Total: %d\n", total_id);
    printf("Total Power: %d\n", total_power);

    return 0;
}


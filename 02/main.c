#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../shared/util.h"

#define BUFFER_SIZE 1024

typedef struct {
    uint32_t game_id;
    uint32_t max_blue;
    uint32_t max_green;
    uint32_t max_red;
} game_t;

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
    FILE* file = getfile(argc, argv);

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


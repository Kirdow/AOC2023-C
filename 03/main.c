#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../shared/util.h"

#define BUFFER_SIZE 512

typedef enum {
    PLOT_VOID,
    PLOT_SYM,
    PLOT_VAL,
} plot_op_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t value;
    plot_op_t op;
    void* ref;
} plot_t;

typedef struct {
    uint32_t w;
    uint32_t h;
    plot_t* data;
} map_t;

static bool is_num(const map_t* map, uint32_t x, uint32_t y, uint32_t* result)
{
    if (x >= map->w || y >= map->h) return false;

    plot_t* plot = &map->data[x + y * map->w];
    if (plot->op != PLOT_VAL) return false;

    *result = plot->value;
    return true;
}

static void add_subtotal(uint32_t num, uint32_t* total, uint32_t* count)
{
    if (*count == 0) *total = num;
    else *total *= num;
    ++(*count);
}

int main(int argc, char** argv)
{
    FILE* file = getfile(argc, argv);

    map_t map = { 0, 0, NULL };
    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file))
    {
        map.w = strlen(line) - 1;
        if (map.data == NULL)
            map.data = malloc(sizeof(plot_t) * map.w);
        else
        {
            size_t next_size = sizeof(plot_t) * map.w * (map.h + 1);
            map.data = realloc(map.data, next_size);
        }

        void* ref = NULL;
        uint32_t value = 0;
        for (uint32_t x = 0; x <= map.w; x++)
        {
            plot_t* plot_ptr = &map.data[x + map.w * map.h];
            plot_ptr->x = x;
            plot_ptr->y = map.h;
            char c = line[x];
            char buffer[2] = {c, '\0'};
            if (x < map.w && c >= 48 && c <= 57)
            {
                value = value * 10 + (c - 48);
                if (!ref) ref = &map.data[x + map.w * map.h];
                continue;
            }
            
            if (ref)
            {
                plot_t* end_ptr = &map.data[x + map.w * map.h];
                size_t size = (size_t)end_ptr - (size_t)ref;
                for (plot_t* ptr = ref; ptr < end_ptr; ++ptr)
                {
                    ptr->value = value;
                    ptr->op = PLOT_VAL;
                    ptr->ref = ref;
                }
            }

            value = 0;
            ref = NULL;
            
            if (x == map.w) break;
            
            if (c == '.')
                plot_ptr->op = PLOT_VOID;
            else
            {
                plot_ptr->op = PLOT_SYM;
                plot_ptr->value = c;
            }
        }

        ++map.h;
    }

    fclose(file);

    uint64_t total = 0;
    uint64_t totalgear = 0;
    for (uint32_t y = 0; y < map.h; y++)
    {
        for (uint32_t x = 0; x < map.w; x++)
        {
            plot_t* ptr = (plot_t*)(map.data + x + y * map.h);
            if (ptr->op != PLOT_SYM)
                continue;


            uint32_t subtotal = 0;
            uint32_t subcount = 0;
            uint32_t num;
            if (is_num(&map, x - 1, y, &num))
            {
                total += num;
                add_subtotal(num, &subtotal, &subcount);
            }
            if (is_num(&map, x + 1, y, &num))
            {
                total += num;
                add_subtotal(num, &subtotal, &subcount);
            }
            
            // up
            if (is_num(&map, x, y - 1, &num))
            {
                total += num;
                add_subtotal(num, &subtotal, &subcount);
            }
            else
            {
                if (is_num(&map, x - 1, y - 1, &num))
                {
                    total += num;
                    add_subtotal(num, &subtotal, &subcount);
                }
                if (is_num(&map, x + 1, y - 1, &num))
                {
                    total += num;
                    add_subtotal(num, &subtotal, &subcount);
                }
            }

            // down
            if (is_num(&map, x, y + 1, &num))
            {
                total += num;
                add_subtotal(num, &subtotal, &subcount);
            }
            else
            {
                if (is_num(&map, x - 1, y + 1, &num))
                {
                    total += num;
                    add_subtotal(num, &subtotal, &subcount);
                }
                if (is_num(&map, x + 1, y + 1, &num))
                {
                    total += num;
                    add_subtotal(num, &subtotal, &subcount);
                }
            }

            if (subcount == 2 && (char)ptr->value == '*') totalgear += subtotal;
        }
    }

    printf("Total: %zu\n", total);
    printf("Total Gear: %zu\n", totalgear);

    free(map.data);
    return 0;
}

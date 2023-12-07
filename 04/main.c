#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../shared/util.h"

#define BUFFER_SIZE 512

typedef struct {
    uint32_t *data;
    uint32_t count;
} nums_t;

static void add_num(nums_t* nums, uint32_t num)
{
    if (nums->count == 0)
        nums->data = (uint32_t*)malloc(sizeof(uint32_t) * (nums->count + 1));
    else
        nums->data = (uint32_t*)realloc(nums->data, sizeof(uint32_t) * (nums->count + 1));
    nums->data[nums->count] = num;
    ++nums->count;
}

static void inc_num(nums_t* nums, uint32_t index)
{
    while (nums->count <= index)
        add_num(nums, 0);

    ++nums->data[index];
}

static nums_t get_nums(const char** ptr, const char* until)
{
    const char* line = *ptr;
    size_t until_len = strlen(until);
    char buffer[until_len + 2];
    strncpy(buffer + 1, until, sizeof(buffer) - 1);
    buffer[0] = ' ';
    nums_t result = {NULL, 0};
    while (true)
    {
        while (*line == ' ')
        {
            ++line;
            continue;
        }
        if (any_of(*line, until)) break;
        
        uint32_t num = (uint32_t)get_num_next(line, &line, buffer); 
        add_num(&result, num);
    }

    *ptr = line;

    return result;
}

static bool nums_has(const nums_t* nums, uint32_t num)
{
    for (uint32_t index = 0; index < nums->count; ++index)
    {
        if (nums->data[index] == num) return true;
    }

    return false;
}

static uint32_t nums_count_in(const nums_t* self, const nums_t* inside)
{
    uint32_t count = 0;

    for (uint32_t index = 0; index < self->count; ++index)
    {
        if (nums_has(inside, self->data[index]))
            ++count;
    }

    return count;
}

static uint32_t nums_score(uint32_t count)
{
    if (count == 0) return 0;
    if (count == 1) return 1;
    uint32_t score = 1;
    while (--count > 0)
        score *= 2;
    return score;
}

static uint32_t nums_copy_score(nums_t* nums, uint32_t index)
{
    if (index >= nums->count) return 0;
    return nums->data[index];
    uint32_t num = nums->data[index];
    if (num <= 1)
    {
        printf("Scores: %d\n", num);
        return num;
    }
    uint32_t count = 1;
    while (--num > 0) count *= 2;
    printf("Scores: %d\n", count);
    return count;
}

int main(int argc, char** argv)
{
    FILE* file = getfile(argc, argv);

    uint32_t total = 0;
    uint32_t totalcount = 0;

    char buffer[BUFFER_SIZE];
    nums_t multi = {NULL, 0};
    uint32_t index = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        const char* line = buffer;
        if (strncmp(line, "Card ", 5) != 0) continue;
        line += 5;
        skip_chars(&line, " ");
        uint64_t card_id = get_num_next(line, &line, ":");
        ++line;
        nums_t winning = get_nums(&line, "|");
        ++line;
        nums_t own = get_nums(&line, "\n");

        uint32_t count = nums_count_in(&own, &winning);
        uint32_t score = nums_score(count);

        uint32_t copies = (index < multi.count) ? multi.data[index] : 0;
        totalcount += copies + 1;
        for (uint32_t i = 0; i < count; i++)
        {
            for (uint32_t j = 0; j <= copies; j++)
                inc_num(&multi, i + index + 1);
        }

        free(winning.data);
        free(own.data);

        printf("Card %d\n", card_id);
        printf("Score: %d\n", score);
        printf("Card Count: %d\n", copies + 1);
        printf("---------------\n");

        total += score;
        ++index;
    }

    fclose(file);

    printf("Total Score: %d\n", total);
    printf("Total Card Count: %d\n", totalcount);
    return 0;
}

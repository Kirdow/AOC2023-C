#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "../shared/util.h"

#define BUFFER_SIZE 512

typedef struct {
    uint64_t source;
    uint64_t dest;
    uint64_t size;
} range_t;

typedef struct {
    range_t* data;
    uint64_t count;
} step_t;

typedef struct {
    step_t* data;
    uint64_t count;
} procedure_t;

typedef struct {
    uint64_t* data;
    uint64_t count;
} seeds_t;

static bool is_map_line(const char* str)
{
    const char* ptr = str;
    while (*ptr != '\0')
    {
        if (strncmp(ptr, " map:", 5) == 0) return true;
        ++ptr;
    }

    return false;
}

static void add_seed(seeds_t* seeds, uint64_t seed)
{
    uint64_t index = expand((void**)&seeds->data, &seeds->count, sizeof(uint64_t));
    seeds->data[index] = seed;
}

static void add_range(step_t* step, const range_t* data)
{
    uint64_t index = expand((void**)&step->data, &step->count, sizeof(range_t));
    step->data[index] = *data;
}

static void add_step(procedure_t* proc, const step_t* data)
{
    uint64_t index = expand((void**)&proc->data, &proc->count, sizeof(step_t));
    proc->data[index] = *data;
}

static seeds_t get_seeds(const char* line)
{
    seeds_t result = {NULL, 0};

    while (*line != '\0')
    {
        skip_chars(&line, " ");
        uint64_t num = get_num_next(line, &line, " ");
        ++line;
        uint64_t count = get_num_next(line, &line, " \n");
        ++line;
        if (result.data == NULL)
            result.data = (uint64_t*)malloc(sizeof(uint64_t) * count);
        else
            result.data = (uint64_t*)realloc(result.data, sizeof(uint64_t) * (result.count + count));
        for (uint64_t i = 0; i < count; i++)
            result.data[result.count + i] = num + i;
        result.count += count;
    }

    return result;
}

static uint64_t run_proc(const procedure_t* proc, uint64_t value)
{
    for (uint64_t proc_id = 0; proc_id < proc->count; ++proc_id)
    {
        step_t* step = &proc->data[proc_id];
        for (uint64_t step_id = 0; step_id < step->count; ++step_id)
        {
            range_t* range = &step->data[step_id];

            if (value >= range->source && value < range->source + range->size)
            {
                value = value + (range->dest - range->source);
                break;
            }
        }
        
    }

    return value;
}

int main(int argc, char** argv)
{
    FILE* file = getfile(argc, argv);

    seeds_t seeds = {NULL, 0};
    procedure_t proc = {NULL, 0};
    step_t step = {NULL, 0};

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file))
    {
        const char* line = buffer;
        if (strncmp(line, "seeds: ", 7) == 0)
        {
            line += 7;
            if (seeds.data)
            {
                printf("Seeds already exist\n");
                return 1;
            }

            printf("Fetching seeds\n");
            seeds = get_seeds(line);
            continue;
        }

        if (is_map_line(line))
        {
            if (!step.data)
                continue;

            add_step(&proc, &step);
            step.data = NULL;
            step.count = 0;
            continue;
        }

        if (is_str_empty(line))
            continue;

        uint64_t dest = get_num_next(line, &line, " ");
        ++line;
        uint64_t src = get_num_next(line, &line, " ");
        ++line;
        uint64_t count = get_num_next(line, &line, " \n");

        range_t range = {src, dest, count};
        add_range(&step, &range);
    }

    if (step.data)
        add_step(&proc, &step);

    fclose(file);

    printf("Locating (this will take a while)\n");
    time_t start_time = time(NULL);
    uint64_t result = UINT64_MAX;
    uint64_t lk = 0;
    for (uint64_t seed_id = 0; seed_id < seeds.count; ++seed_id)
    {
        uint64_t seed = seeds.data[seed_id];
        uint64_t location = run_proc(&proc, seed);
        if (location < result) result = location;
        uint64_t k = seed_id * 10000 / seeds.count;
        if (lk != k)
        {
            uint64_t pc = k / 100;
            uint64_t pp = k % 100;
            printf("\r %zu.", pc);
            if (pp <= 9) printf("0");
            time_t now_time = time(NULL);
            time_t elapsed = now_time - start_time;
            printf("%zu%% (%zus)      \r", pp, elapsed);
            fflush(stdout);
        }
        lk = k;
    }
    time_t end_time = time(NULL);
    time_t time_taken = end_time - start_time;
    printf("\r100%% (%zus)            \n", time_taken);

    printf("Smallest Location: %zu\n", result);

    return 0;
}

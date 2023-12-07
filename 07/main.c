#include "../shared/pch.h"

#define BUFFER_SIZE 512

typedef struct {
    char* data;
    uint64_t count;
    uint64_t bet;
} hand_t;

typedef struct {
    hand_t* data;
    uint64_t count;
    uint64_t win_id;
} wintype_t;

typedef struct {
    wintype_t* data;
    uint64_t count;
} table_t;

typedef struct {
    hand_t* data;
    uint64_t count;
    uint64_t capacity;
} game_t;

static void add_game(game_t* game, const hand_t* hand)
{
    if (game->count == game->capacity)
    {
        if (game->capacity == 0) game->capacity = 10;
        else game->capacity = game->capacity * 3 / 2;
        
        if (game->data == NULL)
            game->data = (hand_t*)malloc(sizeof(hand_t) * game->capacity);
        else
            game->data = (hand_t*)realloc(game->data, sizeof(hand_t) * game->capacity);
    }

    game->data[game->count++] = *hand;
}

static void add_hand(wintype_t* wintype, const hand_t* hand)
{
    if (wintype->data == NULL)
        wintype->data = (hand_t*)malloc(sizeof(hand_t) * (wintype->count + 1));
    else
        wintype->data = (hand_t*)realloc(wintype->data, sizeof(hand_t) * (wintype->count + 1));

    wintype->data[wintype->count++] = *hand;
}

static void init_wintype(table_t* table, uint64_t id)
{
    if (table->data == NULL)
        table->data = (wintype_t*)malloc(sizeof(wintype_t) * (table->count + 1));
    else
        table->data = (wintype_t*)realloc(table->data, sizeof(wintype_t) * (table->count + 1));

    wintype_t* wintype = &table->data[table->count++];
    wintype->data = NULL;
    wintype->count = 0;
    wintype->win_id = id;
}

static void init_wintypes(table_t* table, uint64_t count)
{
    for (uint64_t id = 1; id <= count; ++id)
        init_wintype(table, id);
}

static void add_hand_card(hand_t* hand, char c)
{
    char id = 0;
    if (c >= '2' && c <= '9')
        id = c - 48;
    else if (c == 'T')
        id = 10;
    else if (c == 'J')
        id = 0;
    else if (c == 'Q')
        id = 12;
    else if (c == 'K')
        id = 13;
    else if (c == 'A')
        id = 14;

    if (hand->data == NULL)
        hand->data = (char*)malloc(sizeof(char) * (hand->count + 1));
    else
        hand->data = (char*)realloc(hand->data, sizeof(char) * (hand->count + 1));

    hand->data[hand->count++] = id;
}

static hand_t parse_hand(const char* line)
{
    hand_t hand = { NULL, 0, 0 };
    while (*line != ' ') add_hand_card(&hand, *(line++));
    hand.bet = get_num_next(++line, &line, " \n");
    return hand;
}

static void print_hand(const hand_t* hand)
{
    char buffer[hand->count + 1];
    for (size_t i = 0; i < hand->count; i++)
    {
        char c = hand->data[i];
        if (c >= 2 && c <= 9) c += 48;
        else if (c < 2) c = 'J';
        else if (c == 10) c = 'T';
        else if (c == 12) c = 'Q';
        else if (c == 13) c = 'K';
        else if (c == 14) c = 'A';
        buffer[i] = c;
    }
    buffer[hand->count] = '\0';
    printf("%s %zu\n", buffer, hand->bet);
}

static void sort_counts(const char* matches, char* count, char matchcount)
{
    memcpy(count, matches, sizeof(char) * 5);

    bool changed = true;
    while (changed)
    {
        changed = false;

        for (size_t i = 1; i < matchcount; i++)
        {
            if (count[i-1] < count[i])
            {
                char tmp = count[i - 1];
                count[i - 1] = count[i];
                count[i] = tmp;

                changed = true;
            }
        }

    }
}

static uint64_t get_hand_id(const hand_t* hand)
{
    char pools[15];
    memset(pools, 0, sizeof(pools));
    print_hand(hand);

    for (uint64_t index = 0; index < hand->count; index++)
        pools[hand->data[index]]++;

    char matches[5];
    char matchcount = 0;

    memset(matches, 0, sizeof(matches));

    char joker = pools[0];
    for (uint64_t index = 2; index < 15; index++)
    {
        uint64_t value = pools[index];
        if (value != 0) matches[matchcount++] = value;
    }

    char counts[matchcount];
    sort_counts(matches, counts, matchcount);
    char count = counts[0] + joker;

    if (count == 5) return 7;
    else if (count == 4) return 6;
    else if (count == 3 && counts[1] == 2) return 5;
    else if (count == 3) return 4;
    else if (count == 2 && counts[1] == 2) return 3;
    else if (count == 2) return 2;
    else return 1;
}

static int compare(const hand_t* left, const hand_t* right)
{
    uint64_t count = (left->count < right->count) ? left->count : right->count;
    for (uint64_t i = 0; i < count; i++)
    {
        if (left->data[i] < right->data[i]) return -1;
        if (left->data[i] > right->data[i]) return 1;
    }

    return 0;
}

static void sort_wintype(wintype_t* wintype)
{
    uint64_t iterations = 0;
    printf("Sorting ID: %zu\n", wintype->win_id);
    bool changed = true;
    while (changed)
    {
        changed = false;

        for (uint64_t index = 1; index < wintype->count; index++)
        {
            hand_t* left = &wintype->data[index - 1];
            hand_t* right = &wintype->data[index];

            int result = compare(left, right);
            if (result <= 0) continue;

            hand_t tmp = *left;
            *left = *right;
            *right = tmp;

            if (!changed)
                ++iterations;
            changed = true;
        }
    }

    if (iterations > 0)
        printf("Iterations: %zu\n", iterations);
}

int main(int argc, char** argv)
{
    FILE* file = getfile(argc, argv);

    game_t game = { NULL, 0, 0 };
    table_t table = { NULL, 0 };
    init_wintypes(&table, 7);

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file))
    {
        printf("Line: %s", buffer);
        hand_t hand = parse_hand(buffer);
        add_game(&game, &hand);
        for (uint64_t index = 0; index < hand.count; index++)
        {
            if (index > 0) printf(", ");
            printf("%d", hand.data[index]);
        }

        printf(" => %zu\n", hand.bet);
    }

    fclose(file);

    for (uint64_t index = 0; index < game.count; ++index)
    {
        hand_t* hand = &game.data[index];
        uint64_t hand_id = get_hand_id(hand);
        printf("Hand ID: %zu\n", hand_id);
        add_hand(&table.data[hand_id - 1], hand);
    }

    for (uint64_t hand_id = 0; hand_id < table.count; ++hand_id)
    {
        wintype_t* wintype = &table.data[hand_id];
        sort_wintype(wintype);
    }

    uint64_t total_winnings = 0;
    uint64_t winning_index = 1;
    for (uint64_t i = 0; i < table.count; ++i)
    {
        wintype_t* wintype = &table.data[i];
        printf("Win type: %zu\n", wintype->win_id);
        for (uint64_t j = 0; j < wintype->count; ++j, ++winning_index)
        {
            hand_t* hand = &wintype->data[j];
            print_hand(hand);
            total_winnings += winning_index * hand->bet;
        }
    }

    printf("Total Winnings: %zu\n", total_winnings);
    return 0;
}

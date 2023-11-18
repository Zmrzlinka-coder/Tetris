#ifndef TETRIS_H
#define TETRIS_H
#define WIDTH 10
#define HEIGHT 20
#define GRID_SIZE 30


float TARGET_SECONDS_PER_FRAME = 0.5f;

struct Color
{
    int r;
    int g;
    int b;
    int a;
};

struct Tetrino
{
    const int *data;
    const int side;
};

struct Piece_State
{
    struct Color color;
    int tetrino_index;
    int offset_row;
    int offset_col;
    int rotation;
};

static const int TETRINO_1[] = {
    0, 0, 0, 0,
    1, 1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0};

static const int TETRINO_2[] = {
    1, 1,
    1, 1};

static const int TETRINO_3[] = {
    0, 0, 0,
    1, 1, 1,
    0, 1, 0};

static const int TETRINO_4[] = {
    0, 1, 1,
    1, 1, 0,
    0, 0, 0};

static const int TETRINO_5[] = {
    1, 1, 0,
    0, 1, 1,
    0, 0, 0};

static const int TETRINO_6[] = {
    1, 0, 0,
    1, 1, 1,
    0, 0, 0};

static const int TETRINO_7[] = {
    0, 0, 1,
    1, 1, 1,
    0, 0, 0};

static const struct Tetrino TETRINOS[] = {
    {TETRINO_1, 4},
    {TETRINO_2, 2},
    {TETRINO_3, 3},
    {TETRINO_4, 3},
    {TETRINO_5, 3},
    {TETRINO_6, 3},
    {TETRINO_7, 3}};

enum Game_Phase
{
    GAME_PHASE_START,
    GAME_PHASE_PLAY,
    GAME_PHASE_LINE,
    GAME_PHASE_GAMEOVER
};

struct Game_State
{
    int board[WIDTH * HEIGHT];
    struct Piece_State piece;
    int lines[HEIGHT];

    enum Game_Phase phase;

    float time;
    float next_drop_time;
};

#endif

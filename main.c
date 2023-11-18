#include "SDL2/SDL.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "tetris.h"

static int
tetrino_get(const struct Tetrino *tetrino, int row, int col, int rotation)
{
    int side = tetrino->side;

    switch (rotation)
    {
    case 0:
        return tetrino->data[row * side + col];
    case 1:
        return tetrino->data[(side - col - 1) * side + row];
    case 2:
        return tetrino->data[(side - row - 1) * side + (side - col - 1)];
    case 3:
        return tetrino->data[col * side + (side - row - 1)];
    }
    return 0;
}

void fill_rect(SDL_Renderer *renderer,
               int x, int y, int width, int height, struct Color color)
{

    SDL_Rect rect = {0};
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

int random_int(int min, int max)
{
    int range = max - min;
    return min + rand() % range;
}

void draw_cell(SDL_Renderer *renderer,
               int row, int col,
               int offset_x, int offset_y,struct Color color)
{

    int x = col * GRID_SIZE + offset_x;
    int y = row * GRID_SIZE + offset_y;

    // struct Color* current_color = for_piece+random_int(0,7);
    // struct Color gold;
    // gold.r = 255;
    // gold.g = 215;
    // gold.b = 0;
    // gold.a = 255;

    fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, color);
}

void draw_piece(SDL_Renderer *renderer,
                const struct Piece_State *piece,
                int offset_x, int offset_y)
{
    const struct Tetrino *tetrino = TETRINOS + piece->tetrino_index;
    for (int row = 0;
         row < tetrino->side;
         row++)
    {
        for (int col = 0;
             col < tetrino->side;
             col++)
        {
            int value = tetrino_get(tetrino, row, col, piece->rotation);
            if (value)
            {
                draw_cell(renderer,
                          row + piece->offset_row,
                          col + piece->offset_col,
                          offset_x, offset_y,piece->color);
            }
        }
    }
}

void set_field(int *board, int y, int x, int value)
{
    board[WIDTH * y + x] = value;
}

int get_field(int *field, int row, int col)
{
    int index = row * WIDTH + col;
    return field[index];
}

static void
draw_board(SDL_Renderer *renderer, int *board, int width, int height,
           int offset_x, int offset_y)
{
    struct Color dark;
    dark.r = 28;
    dark.g = 28;
    dark.b = 28;
    dark.a = 255;

    struct Color for_merged;
    for_merged.r=181;
    for_merged.g=184;
    for_merged.b=177;
    for_merged.a=255;
    fill_rect(renderer, offset_x, offset_y, WIDTH * GRID_SIZE, HEIGHT * GRID_SIZE, dark);
    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            if (get_field(board, row, col) == 1)
            {
                draw_cell(renderer, row, col, offset_x, offset_y,for_merged);
            }
        }
    }
}

void clear_lines(int *field, int *lines)
{
    int src_row = HEIGHT - 1;
    for (int dst_row = HEIGHT - 1; dst_row >= 0; dst_row--)
    {
        while (src_row >= 0 && lines[src_row] == 1)
        {
            src_row--;
        }
        if (src_row < 0)
        {
            memset(field + dst_row * WIDTH, 0, WIDTH);
        }
        else
        {
            if (src_row != dst_row)
            {
                for (int x = 0; x < WIDTH; x++)
                {
                    set_field(field, dst_row, x, field[src_row * WIDTH + x]);
                }
            }
            src_row--;
        }
    }
}

int check_row_filled(int *field, int row)
{
    for (int x = 0; x < WIDTH; x++)
    {
        if (get_field(field, row, x) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int find_lines(struct Game_State *game)
{
    int count = 0;
    for (int row = 0; row < HEIGHT; row++)
    {
        int filled = check_row_filled(game->board, row);
        game->lines[row] = filled;
        count += filled;
    }
    return count;
}
void render_game(SDL_Renderer *renderer, struct Game_State *game, int offset_x, int offset_y)
{
    draw_board(renderer, game->board, WIDTH, HEIGHT, offset_x, offset_y);

    draw_piece(renderer, &game->piece, offset_x, offset_y);
}

bool is_move_allowed(struct Game_State *game)
{
    const struct Tetrino *tetrino = TETRINOS + game->piece.tetrino_index;
    int side = tetrino->side;

    for (int row = 0; row < tetrino->side; ++row)
    {
        for (int col = 0; col < tetrino->side; ++col)
        {
            int value = tetrino_get(tetrino, row, col, game->piece.rotation);
            if (value == 1)
            {
                int board_row = game->piece.offset_row + row;
                int board_col = game->piece.offset_col + col;
                if (board_row < 0)
                {
                    return false;
                }
                if (board_row >= HEIGHT)
                {
                    return false;
                }
                if (board_col < 0)
                {
                    return false;
                }
                if (board_col >= WIDTH)
                {
                    return false;
                }
                if (get_field(game->board, board_row, board_col) == 1)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void merge_piece(struct Game_State *game)
{
    const struct Tetrino *tetrino = TETRINOS + game->piece.tetrino_index;

    for (int row = 0; row < tetrino->side; ++row)
    {
        for (int col = 0; col < tetrino->side; ++col)
        {

            int value = tetrino_get(tetrino, row, col, game->piece.rotation);
            if (value == 1)
            {
                int board_row = game->piece.offset_row + row;
                int board_col = game->piece.offset_col + col;
                set_field(game->board, board_row, board_col, value);
            }
        }
    }
}

void spawn_piece(struct Game_State *game)
{
    do
    {
        game->piece.color.r=random_int(0, 255);
        game->piece.color.g=random_int(0, 255);
        game->piece.color.b=random_int(0, 255);
        game->piece.tetrino_index = (int)random_int(0, 6);
        game->piece.offset_col = random_int(0, 9);
    game->piece.offset_row = 0;
    game->piece.rotation = 0;
    } while (!is_move_allowed(game));
}

void soft_drop(struct Game_State *game)
{

    ++game->piece.offset_row;
    if (!is_move_allowed(game))
    {
        --game->piece.offset_row;
        merge_piece(game);
        spawn_piece(game);
    }

    game->next_drop_time = game->time + TARGET_SECONDS_PER_FRAME;
}

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("SDL experiments", 100, 100, 800, 800, SDL_WINDOW_SHOWN);
    if (!window)
    {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    bool quit = false;
    int offset_x = 270;
    int offset_y = 60;
    struct Game_State game;
    game.next_drop_time = game.time + TARGET_SECONDS_PER_FRAME;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            set_field(game.board, y, x, 0);
        }
    }

    game.piece.tetrino_index = 0;
    game.piece.offset_col = WIDTH / 2;
    game.piece.rotation = 0;
    game.phase == GAME_PHASE_PLAY;

    while (!quit)
    {

        game.time = SDL_GetTicks() / 1000.0f;
        while (SDL_PollEvent(&e))
        {
            if (SDL_QUIT == e.type)
            {
                quit = true;
            }
            if (SDL_KEYDOWN == e.type)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    game.piece.rotation++;
                    if (!is_move_allowed(&game))
                    {
                        game.piece.rotation--;
                    }
                    if (game.piece.rotation > 3)
                        game.piece.rotation = 0;
                    break;
                case SDLK_DOWN:
                    game.piece.offset_row++;
                    if (!is_move_allowed(&game))
                        game.piece.offset_row--;
                    break;
                case SDLK_LEFT:
                    game.piece.offset_col--;
                    if (!is_move_allowed(&game))
                        game.piece.offset_col++;
                    break;
                case SDLK_RIGHT:
                    game.piece.offset_col++;
                    if (!is_move_allowed(&game))
                        game.piece.offset_col--;
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        int lines = find_lines(&game);
        if (lines > 0)
        {
            clear_lines(game.board, game.lines);
        }
        if (game.piece.offset_row == 0 && game.piece.offset_col == 5 && !is_move_allowed(&game))
            quit = true;

        while (game.time >= game.next_drop_time)
        {
            soft_drop(&game);
        }

        render_game(renderer, &game, offset_x, offset_y);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
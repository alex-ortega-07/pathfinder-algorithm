#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <iostream>

#include "Button.hpp"
#include "Box.hpp"


const int WIDTH = 500;
const int HEIGHT = 560;

const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

const int BOARD_BOX_SPACE = 20;
const int BOARD_WIDTH = WIDTH - 40;
const int BOARD_HEIGHT = BOARD_WIDTH;
const int BOARD_START_POINT_Y = HEIGHT - BOARD_HEIGHT - 20;

const int SPACE_BETWEEN_BT = 20;
const int BT_WIDTH = (BOARD_WIDTH - SPACE_BETWEEN_BT * 3) / 4;
const int BT_HEIGHT = 40;
const int BT_START_POINT_Y = (BOARD_START_POINT_Y - BT_HEIGHT) / 2;

const int NUM_BOXES_ROW = BOARD_WIDTH / BOARD_BOX_SPACE;

const SDL_Color BLACK = {0, 0, 0, 255};
const SDL_Color BLACK_BOX = {50, 50, 50, 255};
const SDL_Color START_POINT = {0, 255, 0, 255};
const SDL_Color END_POINT = {205, 0, 0, 255};
const SDL_Color CLOSE_SET = {240, 149, 59, 255};
const SDL_Color OPEN_SET = {255, 254, 0, 255};
const SDL_Color PATH = {107, 38, 63, 255};
const SDL_Color SELECTED = {100, 100, 200, 255};

struct Node{
    int x;
    int y;
    int f;
    int g;
    int h;
    int parent_x;
    int parent_y;
};


void draw_board(SDL_Renderer *renderer){
    int num_lines = BOARD_WIDTH / BOARD_BOX_SPACE;
    int x1 = (WIDTH - BOARD_WIDTH) / 2;

    // Horizontal lines
    for(int i = 0; i <= num_lines; ++i){
        SDL_RenderDrawLine(renderer, x1, BOARD_START_POINT_Y + BOARD_BOX_SPACE * i, BOARD_WIDTH + x1, BOARD_START_POINT_Y + BOARD_BOX_SPACE * i);
    }

    // Vertical lines
    for(int i = 0; i <= num_lines; ++i){
        SDL_RenderDrawLine(renderer, x1 + BOARD_BOX_SPACE * i, BOARD_START_POINT_Y, x1 + BOARD_BOX_SPACE * i, BOARD_START_POINT_Y + BOARD_HEIGHT);
    }
}

void draw_board_values(SDL_Renderer *renderer, Box board[NUM_BOXES_ROW][NUM_BOXES_ROW], bool &clear){
    static int row = 0;

    if(clear == true){
        for(int col = 0; col < NUM_BOXES_ROW; ++col){
            board[row][col].val = 0;
        }

        row++;

        if(row == NUM_BOXES_ROW){
            clear = false;
            row = 0;
        }
    }

    for(int row = 0; row < NUM_BOXES_ROW; ++row){
        for(int col = 0; col < NUM_BOXES_ROW; ++col){
            if(board[row][col].val == 0){
                board[row][col].disappear(renderer, BOARD_BOX_SPACE - 1);
            }

            else if(board[row][col].val == 1){
                board[row][col].color = BLACK_BOX;
            }

            else if(board[row][col].val == 2){
                board[row][col].color = START_POINT;
            }

            else if(board[row][col].val == 3){
                board[row][col].color = END_POINT;
            }

            else if(board[row][col].val == 4){
                board[row][col].color = OPEN_SET;
            }

            else if(board[row][col].val == 5){
                board[row][col].color = CLOSE_SET;
            }

            else if(board[row][col].val == 6){
                board[row][col].color = PATH;
            }

            if(board[row][col].val != 0){
                board[row][col].appear(renderer, BOARD_BOX_SPACE - 1);
            }
        }
    }
}

int ntimes(const Box board[NUM_BOXES_ROW][NUM_BOXES_ROW], int n){
    int count = 0;

    for(int row = 0; row < NUM_BOXES_ROW; ++row){
        for(int col = 0; col < NUM_BOXES_ROW; ++col){
            if(board[row][col].val == n){
                ++count;
            }
        }
    }

    return count;
}

void locbox(int box_val, int &x, int &y, const Box board[NUM_BOXES_ROW][NUM_BOXES_ROW]){
    for(int row = 0; row <= NUM_BOXES_ROW; ++row){
        for(int col = 0; col <= NUM_BOXES_ROW; ++col){
            if(board[row][col].val == box_val){
                x = col;
                y = row;

                return;
            }
        }
    }


    x = y = -1;
}

void calc_distance(int start_x, int start_y, int end_x, int end_y, int &distance){
    distance = abs(end_x - start_x) + abs(end_y - start_y);
}

int in_close_set(std::vector<Node> close_set, int x, int y){
    for(int i = 0; i < close_set.size(); ++i){
        if(close_set[i].x == x && close_set[i].y == y){
            return true;
        }
    }

    return false;
}

int in_open_set(std::vector<Node> open_set, int x, int y){
    for(int i = 0; i < open_set.size(); ++i){
        if(open_set[i].x == x && open_set[i].y == y){
            return true;
        }
    }

    return false;
}

int main(int argv, char *args[]){
    Box board[NUM_BOXES_ROW][NUM_BOXES_ROW];

    for(int row = 0; row < NUM_BOXES_ROW; ++row){
        for(int col = 0; col < NUM_BOXES_ROW; ++col){
            board[row][col].posx = (WIDTH - BOARD_WIDTH) / 2 + BOARD_BOX_SPACE * col + 1;
            board[row][col].posy = BOARD_START_POINT_Y + BOARD_BOX_SPACE * row + 1;
            board[row][col].width = board[row][col].height = BOARD_BOX_SPACE - 1;
        }
    }

    SDL_Window *win = SDL_CreateWindow("Pathfinder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if(win == NULL){
        std::cout << "Window could not be created. SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if(TTF_Init()){
        std::cout << "TTF could not be initialized. TTF_Error" << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);

    SDL_Event event;

    TTF_Font *font = TTF_OpenFont("res/font/font.ttf", 18);
    if(font == NULL){
        std::cout << "Failed to load font. SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    const char *start_bt_txt = "Start point";
    Button start_bt(20, BT_START_POINT_Y, BT_WIDTH, BT_HEIGHT, start_bt_txt, font, BLACK);

    const char *end_bt_txt = "End point";
    Button end_bt(20 + BT_WIDTH + SPACE_BETWEEN_BT, BT_START_POINT_Y, BT_WIDTH, BT_HEIGHT, end_bt_txt, font, BLACK);

    const char *wall_bt_txt = "Wall";
    Button wall_bt(20 + BT_WIDTH * 2 + SPACE_BETWEEN_BT * 2, BT_START_POINT_Y, BT_WIDTH, BT_HEIGHT, wall_bt_txt, font, BLACK);

    const char *solve_bt_txt = "Solve";
    Button solve_bt(20 + BT_WIDTH * 3 + SPACE_BETWEEN_BT * 3, BT_START_POINT_Y, BT_WIDTH, BT_HEIGHT, solve_bt_txt, font, BLACK);

    std::vector <Node> open_set;
    std::vector <Node> close_set;
    std::vector <SDL_Point> path;

    Node current_node;

    std::vector <int> f_vals_repeat_open_set;

    Uint32 frame_start;
    int frame_time;

    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);

    SDL_Color grid_color = {0, 0, 0, 255};

    SDL_Point mouse;
    SDL_Point point;

    bool start_bt_click, end_bt_click, wall_bt_click, solve_bt_click;
    start_bt_click = end_bt_click = wall_bt_click = solve_bt_click = false;

    bool g_pressed = false;
    bool show_grid = true;
    bool clear_board = false;
    bool is_solving = false;

    bool path_backtrack = false;

    int start_x, start_y;
    int end_x, end_y;
    start_x = start_y = end_x = end_y = -1;

    bool running = true;
    
    while(running){
        frame_start = SDL_GetTicks();

        SDL_GetMouseState(&mouse.x, &mouse.y);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if(keyboard_state[SDL_SCANCODE_G] && !g_pressed){
            show_grid = !show_grid;
            g_pressed = true;
        }

        if(show_grid){
            if(grid_color.r != 0 && grid_color.g != 0 && grid_color.b != 0){
                grid_color.r -= 15;
                grid_color.g -= 15;
                grid_color.b -= 15;
            }
        }

        else{
            if(grid_color.r != 255 && grid_color.g != 255 && grid_color.b != 255){
                grid_color.r += 15;
                grid_color.g += 15;
                grid_color.b += 15;
            }
        }

        if(!keyboard_state[SDL_SCANCODE_G]){
            g_pressed = false;
        }

        if(keyboard_state[SDL_SCANCODE_C] && !clear_board){
            open_set.erase(open_set.begin(), open_set.end());
            close_set.erase(close_set.begin(), close_set.end());
            path.erase(path.begin(), path.end());

            start_x = start_y = end_x = end_y = -1;
            
            clear_board = true;
        }

        SDL_SetRenderDrawColor(renderer, grid_color.r, grid_color.g, grid_color.b, grid_color.a);
        draw_board(renderer);
        
        start_bt.create(renderer);
        end_bt.create(renderer);
        wall_bt.create(renderer);
        solve_bt.create(renderer);

        if(is_solving){
            Node discover_node;

            int min_f = open_set[0].f;
            int index_min_f = 0;
            int index_min_h = 0;
            

            for(int i = 0; i < open_set.size(); ++i){
                if(open_set[index_min_f].f > open_set[i].f){
                    index_min_f = index_min_h = i;
                }

                if(open_set[index_min_f].f == open_set[i].f){
                    if(open_set[index_min_h].h > open_set[i].h){
                        index_min_h = i;
                    }
                }
            }

            current_node = open_set[index_min_h];

            open_set.erase(open_set.begin() + index_min_h);
            close_set.push_back(current_node);

            int ind_y = 0;
            int ind_x = 1;
            for(int i = 0; i < 4; ++i){
                if(i == 2){
                    ind_y = 1;
                    ind_x = 0;
                }

                if(current_node.y + ind_y == end_y && current_node.x + ind_x == end_x){

                    path_backtrack = true;

                    is_solving = false;
                    break;
                }

                if(board[current_node.y + ind_y][current_node.x + ind_x].val == 0){
                    if(current_node.y + ind_y >= 0 && current_node.y + ind_y < NUM_BOXES_ROW && current_node.x + ind_x >= 0 && current_node.x + ind_x < NUM_BOXES_ROW){
                        if(!in_close_set(close_set, current_node.x + ind_x, current_node.y + ind_y)){
                            if(in_open_set(close_set, current_node.x + ind_x, current_node.y + ind_y)){
                                for(int i = 0; i < open_set.size(); ++i){
                                    if(open_set[i].x == current_node.x + ind_x && open_set[i].y == current_node.y + ind_y){
                                        if(open_set[i].g > current_node.g + 1){
                                            open_set[i].g = current_node.g + 1;

                                            open_set[i].parent_x = current_node.x;
                                            open_set[i].parent_y = current_node.y;

                                            break;
                                        }
                                    }
                                }
                            }

                            else{
                                discover_node.x = current_node.x + ind_x;
                                discover_node.y = current_node.y + ind_y;
                                discover_node.parent_x = current_node.x;
                                discover_node.parent_y = current_node.y;

                                discover_node.g = current_node.g + 1;
                                calc_distance(discover_node.x, discover_node.y, end_x, end_y, discover_node.h);
                                discover_node.f = discover_node.g + discover_node.h;

                                open_set.push_back(discover_node);
                            }
                        }
                    }
                }

                ind_y = (ind_x == 0) ? ind_y : 0;
                ind_x = (ind_y == 0) ? ind_x : 0;

                ind_y *= -1;
                ind_x *= -1;
            }
        }

        if(path_backtrack){            
            point.x = current_node.x;
            point.y = current_node.y;

            path.push_back(point);

            for(int i = 0; i < close_set.size(); ++i){
                if(close_set[i].x == current_node.parent_x && close_set[i].y == current_node.parent_y){
                    current_node = close_set[i];
                }
            }

            if(current_node.parent_x == -1 || current_node.parent_y == -1){
                is_solving = false;
                path_backtrack = false;
            }
        }

        if(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
            }

            if(event.type == SDL_MOUSEBUTTONUP){
                start_bt_click = end_bt_click = wall_bt_click = solve_bt_click = false;
            }

            if(SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(SDL_BUTTON_LEFT)){
                if(start_bt.clicked(mouse.x, mouse.y) && !start_bt_click){
                    if(start_bt.is_color(BLACK)){
                        start_bt.set_color(SELECTED);

                        end_bt.set_color(BLACK);
                        wall_bt.set_color(BLACK);
                        solve_bt.set_color(BLACK);
                    }

                    else{
                        start_bt.set_color(BLACK);
                    }

                    start_bt_click = true;
                }

                if(end_bt.clicked(mouse.x, mouse.y) && !end_bt_click){
                    if(end_bt.is_color(BLACK)){
                        end_bt.set_color(SELECTED);

                        start_bt.set_color(BLACK);
                        wall_bt.set_color(BLACK);
                        solve_bt.set_color(BLACK);
                    }

                    else{
                        end_bt.set_color(BLACK);
                    }

                    end_bt_click = true;
                }

                if(wall_bt.clicked(mouse.x, mouse.y) && !wall_bt_click){
                    if(wall_bt.is_color(BLACK)){
                        wall_bt.set_color(SELECTED);

                        start_bt.set_color(BLACK);
                        end_bt.set_color(BLACK);
                        solve_bt.set_color(BLACK);
                    }

                    else{
                        wall_bt.set_color(BLACK);
                    }

                    wall_bt_click = true;
                }

                if(solve_bt.clicked(mouse.x, mouse.y) && !solve_bt_click){
                    if(solve_bt.is_color(BLACK)){
                        solve_bt.set_color(SELECTED);

                        start_bt.set_color(BLACK);
                        end_bt.set_color(BLACK);
                        wall_bt.set_color(BLACK);

                        open_set.erase(open_set.begin(), open_set.end());
                        close_set.erase(close_set.begin(), close_set.end());
                        path.erase(path.begin(), path.end());

                        locbox(2, start_x, start_y, board);
                        locbox(3, end_x, end_y, board);

                        if(start_x != -1 || end_x != -1){
                            Node start_point;

                            start_point.x = start_x;
                            start_point.y = start_y;
                            start_point.parent_x = -1;
                            start_point.parent_y = -1;
                            start_point.g = 0;

                            calc_distance(start_x, start_y, end_x, end_y, start_point.h);
                            start_point.f = start_point.g + start_point.h;

                            open_set.push_back(start_point);

                            is_solving = true;
                        }

                        else{
                            solve_bt.set_color(BLACK);
                        }
                    }

                    else{
                        solve_bt.set_color(BLACK);
                    }

                    solve_bt_click = true;
                }

            }

            if(mouse.x > (WIDTH - BOARD_WIDTH) / 2 && mouse.x < BOARD_WIDTH + (WIDTH - BOARD_WIDTH) / 2){
                if(mouse.y > BOARD_START_POINT_Y && mouse.y < BOARD_START_POINT_Y + BOARD_HEIGHT){
                    int row = (mouse.y - BOARD_START_POINT_Y) / BOARD_BOX_SPACE;
                    int col = (mouse.x - (WIDTH - BOARD_WIDTH) / 2) / BOARD_BOX_SPACE;

                    if(SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(SDL_BUTTON_LEFT)){
                        if(!wall_bt.is_color(BLACK)){
                            board[row][col].val = 1;
                        }

                        else if(!start_bt.is_color(BLACK)){
                            int x, y;
                            x = y = -1;

                            while(ntimes(board, 2) > 0){
                                locbox(2, x, y, board);
                                board[y][x].val = 0;
                            }

                            board[row][col].val = 2;
                        }

                        else if(!end_bt.is_color(BLACK)){
                            int x, y;
                            x = y = -1;

                            while(ntimes(board, 3) > 0){
                                locbox(3, x, y, board);
                                board[y][x].val = 0;
                            }

                            board[row][col].val = 3;
                        }
                    }

                    if(SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(SDL_BUTTON_RIGHT)){
                        board[row][col].val = 0;
                    }
                }
            }
        }

        for(int i = 0; i < open_set.size(); ++i){
            board[open_set[i].y][open_set[i].x].val = 4;
        }

        for(int i = 0; i < close_set.size(); ++i){
            board[close_set[i].y][close_set[i].x].val = 5;
        }

        for(int i = 0; i < path.size(); ++i){
            board[path[i].y][path[i].x].val = 6;
        }

        board[start_y][start_x].val = 2;
        board[end_y][end_x].val = 3;

        draw_board_values(renderer, board, clear_board);

        SDL_RenderPresent(renderer);

        frame_time = SDL_GetTicks() - frame_start;
        if(FRAME_DELAY > frame_time){
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    
    SDL_Quit();
    TTF_Quit();

    return 0;
}

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1

#define CELL_SIZE 20

int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

bool draw_grid = true;

typedef struct {
    SDL_FRect* rect;
    SDL_Color* color;
    bool alive;
    bool died;

} Cell;

Cell* makeCell(int x, int y, int h, int w, int r, int g, int b)
{
    SDL_FRect* rect = (SDL_FRect*)malloc(sizeof(SDL_FRect));
    rect->x = x;
    rect->y = y;
    rect->h = h;
    rect->w = w;
    SDL_Color* color = (SDL_Color*)malloc(sizeof(SDL_Color));
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = 255;
    Cell* cell = (Cell*)malloc(sizeof(Cell));
    cell->alive = false;
    cell->rect = rect;
    cell->color = color;
    return cell;
}

void paintCell(Cell* cell, int r, int g, int b)
{
    cell->color->r = r;
    cell->color->g = g;
    cell->color->b = b;  
}

void killCell(Cell* cell)
{
    if (cell->alive)
        paintCell(cell, 204, 252, 249);
        cell->color->a = 120;
    // else
    //    paintCell(cell, 255, 255, 255);   
    cell->alive = false;
}

void reviveCell(Cell* cell)
{
    cell->alive = true;
    paintCell(cell, 127, 127, 127); 
}

void setCellStateAlive(int x, int y, Cell*** cell_matrix)
{
    if (cell_matrix[x][y]->alive)
        return;
    cell_matrix[x][y]->alive = true;
    paintCell(cell_matrix[x][y], 127, 127, 127);
}
void changeCellState(int x, int y, Cell*** cell_matrix)
{
    cell_matrix[x][y]->alive = !(cell_matrix[x][y]->alive);
    if(cell_matrix[x][y]->alive)
    {
        cell_matrix[x][y]->color->r = 127;
        cell_matrix[x][y]->color->g = 127;
        cell_matrix[x][y]->color->b = 127;
    }
    else
    {
        cell_matrix[x][y]->color->r = 255;
        cell_matrix[x][y]->color->g = 255;
        cell_matrix[x][y]->color->b = 255;  
    }
}

// fja zavisi od draw_grid globalne promenljive!!!
void drawCell(Cell* cell, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, cell->color->r, cell->color->g, cell->color->b, cell->color->a);
    SDL_RenderFillRect(renderer, cell->rect);

    if(draw_grid)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderRect(renderer, cell->rect);
    }
}

void drawGrid(Cell*** cell_matrix, int rows, int cols, SDL_Renderer* renderer)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            drawCell(cell_matrix[i][j], renderer);
}

// za sad targetiram FullHD 1920 x 1080, za pocetak celije 20px x 20px
Cell*** makeCells(int w, int h, SDL_Renderer* renderer)
{
    Cell*** cell_matrix = (Cell***)malloc(sizeof(Cell**) * (h / CELL_SIZE)) ;
    for (int i = 0; i < h / CELL_SIZE; i++)
        cell_matrix[i] = (Cell**)malloc(sizeof(Cell*) * (w / CELL_SIZE));

    for (int i = 0; i < h; i += CELL_SIZE)
        for (int j = 0; j < w; j += CELL_SIZE)
            cell_matrix[i / CELL_SIZE][j / CELL_SIZE] = makeCell(j, i, CELL_SIZE, CELL_SIZE, 255, 255, 255);

    return cell_matrix;
}

void evolveToNextGen(Cell*** cell_matrix, int rows, int cols)
{
    bool** new_states = (bool**)malloc(sizeof(bool*) * rows);
    for (int i = 0; i < rows; i++)
        new_states[i] = (bool*)malloc(sizeof(bool) * cols);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int neigbors = 0;
            for (int k = 0; k < 8; k++)
            {
                if (i + dx[k] >= 0 && i + dx[k] < rows && j + dy[k] >= 0 && j + dy[k] < cols &&
                     (cell_matrix[i + dx[k]][j + dy[k]]->alive == true))
                    neigbors++;
            }
            if ((cell_matrix[i][j]->alive == false && neigbors == 3) || (neigbors >= 2 && neigbors <= 3 && cell_matrix[i][j]->alive == true))
                new_states[i][j] = true;
            else
                new_states[i][j] = false;
                if(cell_matrix[i][j]->alive)
                    paintCell(cell_matrix[i][j], 204, 252, 249);
        }
    }

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if(new_states[i][j] == true)
                reviveCell(cell_matrix[i][j]);
            else
                killCell(cell_matrix[i][j]);
}

int main(int argc, char* argv[]) 
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Conway's game of life", 500, 500, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    SDL_DisplayID dId;
    SDL_DisplayMode dm;
    SDL_Rect display;
    SDL_Event event;
    dId = SDL_GetPrimaryDisplay();
    SDL_GetDisplayBounds(dId, &display);

    bool running = true;
    bool evolve = false;

    const int FPS = 45;
    const int wanted_frame_time = 1000 / FPS;

    int frame_time;
    int start_of_frame_time;
    bool dragging;


    Cell*** cell_matrix = makeCells(display.w, display.h, renderer);
    while (running) 
    {
        start_of_frame_time = SDL_GetTicks();
        
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                running = false;
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE)
                evolve = !evolve;
            else if(event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_G)
                draw_grid = !draw_grid;
            else if(event.type == SDL_EVENT_MOUSE_MOTION && dragging)
            {
                int x = event.button.x;
                int y = event.button.y;
                setCellStateAlive(y / CELL_SIZE, x / CELL_SIZE, cell_matrix);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int x = event.button.x;
                int y = event.button.y;
                changeCellState(y / CELL_SIZE, x / CELL_SIZE, cell_matrix);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT)
            {
                dragging = true;
                int x = event.button.x;
                int y = event.button.y;
                setCellStateAlive(y / CELL_SIZE, x / CELL_SIZE, cell_matrix);
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_RIGHT)
                dragging = false;
        }
        if (evolve)
            evolveToNextGen(cell_matrix, display.h / CELL_SIZE, display.w / CELL_SIZE);
            
        
        drawGrid(cell_matrix, display.h / CELL_SIZE, display.w / CELL_SIZE, renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderPresent(renderer); // prikazivanje
        SDL_RenderClear(renderer); // clearovanje ekrana

        frame_time = SDL_GetTicks() - start_of_frame_time;
        if (frame_time < wanted_frame_time)
            SDL_Delay(wanted_frame_time - frame_time);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}   
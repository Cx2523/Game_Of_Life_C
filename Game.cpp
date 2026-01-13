#include <cmath>
#include <time.h>

using namespace std;

#include "Game.h"

#define TICK_INTERVAL 20
static Uint32 nextTime;

Uint32 TimeLeft() {
  Uint32 curTime = SDL_GetTicks();
  if (nextTime <= curTime) return 0;
  else return nextTime - curTime;
}

Game::Game()
{
  window = SDL_CreateWindow(
    "Game of Life --- (Stopped)",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    800,
    800,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
  );

  if (window == nullptr) {
    cout << "Failed to create SDL_CreateWidow." << endl;
    return;
  }

  renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_PRESENTVSYNC
  );

  if (renderer == nullptr) {
    cout << "Failed to create SDL_CreateRenderer." << endl;
    return;
  }
}

Game::~Game()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void Game::Run()
{
  InitCells(150, 150);
  AdjustGrid();

  isRunning = true;

  nextTime = SDL_GetTicks() + TICK_INTERVAL;

  while (isRunning) {
    SDL_Event ev;
    //SDL_WaitEvent(&ev);
    SDL_PollEvent(&ev);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Draw();

    SDL_RenderPresent(renderer);

    if (simRunning && !mouseDown)
      RunSimulation();

    if (ev.type == SDL_WINDOWEVENT) {
      if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
        isRunning = false;
      }
    }

    if (ev.type == SDL_KEYDOWN) {
      if (ev.key.keysym.sym == SDLK_ESCAPE) {
        isRunning = false;
      }

      if (ev.key.keysym.sym == SDLK_SPACE) {
        simRunning = !simRunning;
        if (simRunning) SDL_SetWindowTitle(window, "Game of Life --- (Running)");
        else SDL_SetWindowTitle(window, "Game of Life --- (Stopped)");
      }
    }

    //CalculateNextGeneration();
    //SDL_Delay(200);

    SDL_Delay(TimeLeft());
    nextTime += TICK_INTERVAL;
  }
}

void Game::InitCells(int rows, int cols)
{
  cells.clear();
  grid.rows = rows;
  grid.cols = cols;

  srand((unsigned)time(NULL));

  for (int x = 0; x < grid.rows; x++) {
    vector<Cell> column;
    for (int y = 0; y < grid.cols; y++) {
      Cell c = Cell();
      
      if ((x != 0 && x != grid.rows - 1) && (y != 0 && y != grid.cols - 1)) {
        if (((float)rand() / RAND_MAX) < 0.2f) {
          c.Born();
        }
      }

      column.push_back(c);

    }

    cells.push_back(column);
  }

  //for (int i = 0; i < rows; i++) {
  //  cells[i][i].Born();
  //}


}

void Game::Draw()
{
  for (int x = 0; x < grid.rows; x++) {
    for (int y = 0; y < grid.cols; y++) {
      SDL_FRect cellRect = {
        1.0f + (float)grid.posX + ((float)x * grid.cellWidth),
        1.0f + (float)grid.posY + ((float)y * grid.cellHeight),
        grid.cellWidth,
        grid.cellHeight
      };

      if (cells[x][y].isAlive)
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
        /*SDL_SetRenderDrawColor(renderer, 51, 215, 255, 255);*/
      else
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderFillRectF(renderer, &cellRect);
    }
  }
}

void Game::AdjustGrid()
{
  int ww = 0;
  int wh = 0;
  SDL_GetWindowSize(window, &ww, &wh);

  int maxLength = 0;
  if (ww > wh)
    maxLength = wh;
  else
    maxLength = ww;

  grid.posX = grid.borderSize;
  grid.posY = grid.borderSize;

  int botRightX = maxLength - grid.borderSize;
  int botRightY = maxLength - grid.borderSize;

  grid.width = botRightX - grid.posX;
  grid.height = botRightY - grid.posY;

  grid.cellWidth = (float)grid.width / (float)grid.rows;
  grid.cellHeight = (float)grid.height / (float)grid.cols;

  if (ww > wh)
    grid.posX = ((float)ww / 2.0f) - ((float)grid.width / 2.0f);
  else
    grid.posY = ((float)wh / 2.0f) - ((float)grid.height / 2.0f);
}

void Game::RunSimulation()
{
  for (int x = 0; x < grid.rows; x++) {
    for (int y = 0; y < grid.cols; y++) {
      cells[x][y].neighbors = 0;

      if (cells[iX(x - 1)][iY(y - 1)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x - 1)][iY(y)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x - 1)][iY(y + 1)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x)][iY(y - 1)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x)][iY(y + 1)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x + 1)][iY(y - 1)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x + 1)][iY(y)].isAlive) cells[x][y].neighbors++;

      if (cells[iX(x + 1)][iY(y + 1)].isAlive) cells[x][y].neighbors++;
    }
  }

  while (simTick < simDelay) {
    SDL_Delay(1);
    simTick++;
  }

  for (int x = 0; x < grid.rows; x++) {
    for (int y = 0; y < grid.cols; y++) {
      if (cells[x][y].neighbors == 3) {
        cells[x][y].Born();
      }

      if (cells[x][y].neighbors < 2) {
        cells[x][y].Die();
      }

      if (cells[x][y].neighbors > 3) {
        cells[x][y].Die();
      }
    }
  }

   simTick = 0;
}

void Game::CalculateNextGeneration()
{
  for (int x = 1; x < grid.rows - 1; x++) {
    for (int y = 1; y < grid.rows - 1; y++) {
      int neighborCount = 0;
      
      for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
          if (i != 0 || j != 0) {
            if (cells[x + i][y + j].isAlive) {
              neighborCount++;
            }
          }
        }
      }

      if (cells[x][y].isAlive && (neighborCount == 2 || neighborCount == 3)) {
        cells[x][y].Born();
      }
      else if (!cells[x][y].isAlive && neighborCount == 3) {
        cells[x][y].Born();
      }
      else {
        cells[x][y].Die();
      }
    }
  }
}

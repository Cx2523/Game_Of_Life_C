#pragma once

#include <iostream>
using namespace std;

#include <vector>

#include <SDL.h>

struct Cell
{
  Cell() { };
  Cell(bool alive) {
    isAlive = alive;
  }

  bool isAlive = false;
  int neighbors = 0;

  void Die() { isAlive = false; }
  void Born() { isAlive = true; }
  void Flip() { isAlive = !isAlive; }

};

struct Grid
{
  Grid() {}

  int rows = 0;
  int cols = 0;

  // Grid position in the window
  int posX = 0;
  int posY = 0;

  // Grid size in the window
  int width = 0;
  int height = 0;

  // Size of each cell
  float cellHeight = 0;
  float cellWidth = 0;

  int borderSize = 20;
};

class Game
{
private:
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;

  bool isRunning = false;

  Grid grid;

  vector<vector<Cell>> cells;

  int iX(int i) {
    if (i < 0) return 0;
    else if (i > grid.rows - 1) return grid.rows - 1;
    return i;
  }

  int iY(int i) {
    if (i < 0) return 0;
    else if (i > grid.cols - 1) return grid.cols - 1;
    else return i;
  }

  int mouseX = 0;
  int mouseY = 0;
  bool mouseDown = false;
  int mouseCellX = 0;
  int mouseCellY = 0;

  // Simulation variables
  bool simRunning = false;
  int simTick = 0;
  int simDelay = 40;

public:
  Game();
  ~Game();

  void Run();
  void InitCells(int rows, int cols);
  void Draw();
  void AdjustGrid();
  void RunSimulation();
  void CalculateNextGeneration();
};

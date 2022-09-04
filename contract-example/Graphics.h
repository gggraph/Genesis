#pragma once
#include "SDL2/include/SDL.h"
#include "SDL2/SDL2_image-2.0.5/include/SDL_image.h"
#include "SDL2/SDL2_ttf-2.0.15/include/SDL_ttf.h"
#include <iostream>

#define SQUARE_TEX_SIZE 80
#define WINDOW_WIDTH  800 
#define WINDOW_HEIGHT 800 
// 728
void InitSDL();
void CloseSDL();
void PrintImage(unsigned char* IMG, int size, int x, int y, int w, int h);
void PrintTXT(std::string s, int x, int y, int w, int h, SDL_Color c);
void FillRectangle(int x, int y, int w, int h, int r, int g, int b);
void DrawTriangle(float x0, float y0, float x1, float y1, float x2, float y2);
void DrawLine(float x0, float y0, float x1, float y1);
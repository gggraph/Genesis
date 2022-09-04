#include "Graphics.h"

SDL_Window* win = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font;



void InitSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	// Create the Window and the renderer 
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &win, &renderer);
	font = TTF_OpenFont("terminal.ttf", 24);
}
void CloseSDL()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

}
void PrintTXT(std::string s, int x, int y , int w, int h, SDL_Color c)
{
	if (font == NULL) {
		font = TTF_OpenFont("terminal.ttf", 24);
	}
	char* msg = new char[s.size() + 1];
	std::copy(s.begin(), s.end(), msg);
	msg[s.size()] = '\0'; // don't forget the terminating 0
	//SDL_Color White = { 255, 255, 255 };
	SDL_Surface* surfaceMessage =
		TTF_RenderText_Solid(font, msg, c);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	if (Message == NULL) {
		std::cout << "err";
	}
	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	Message_rect.w = w; // controls the width of the rect
	Message_rect.h = h; // controls the height of the rect

	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	SDL_RenderPresent(renderer);
	// Don't forget to free your surface and texture
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	delete[] msg;
}
void DrawTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
{
	// std::cout << x0 << std::endl;
	SDL_SetRenderDrawColor(renderer, 250, 0, 0, 250);
	DrawLine(x0, y0, x1, y1);
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x0, y0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 250);
	SDL_RenderPresent(renderer);
}
void DrawLine(float x0, float y0, float x1, float y1) {
	SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}
void FillRectangle(int x, int y, int w, int h, int r, int g, int b)
{
	
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_Rect rec;
	rec.x = x;
	rec.y = y; 
	rec.w = w; 
	rec.h = h;
	SDL_RenderFillRect(renderer, &rec);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderPresent(renderer);
}
void PrintImage(unsigned char * IMG, int size, int x, int y, int w, int h ) 
{
	// [1] load bmp as surface
	//surface = SDL_LoadBMP("C:\\sprite.bmp"); //<----------- here should be the spritesheet path
	SDL_RWops *  rw = SDL_RWFromMem(IMG, size);
	//surface = SDL_LoadBMP_RW(rw, 1);
	SDL_Surface*  surface = IMG_Load_RW(rw, 1);
	if (!surface) {
		std::cout << IMG_GetError();
		// handle error
	}
	// [2] copy surface 
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
	//  copy texture to renderer
	SDL_Rect rec;
	rec.x = x;
	rec.y = y; 
	if (surface->w > surface->h) {
		float divisor = w; 
		divisor /= surface->w;
		surface->w *= divisor;
		surface->h *= divisor;
	}
	else {
		float divisor = w;
		divisor /= surface->h;
		surface->w *= divisor;
		surface->h *= divisor;

	}
	rec.w = surface->w;
	rec.h = surface->h;
	
	SDL_RenderCopy(renderer, tex, NULL, &rec); 
	SDL_RenderPresent(renderer);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(tex);
}
/*
	COMP 3520: Computer Graphics
	Assignment 2
	Author: Mostapha Rammo 104599907
	Date: Friday, January 25 2019
*/

#include <iostream>
#include <tuple>
#include <cmath>

#include "SDL.h"
#include "SDL_ttf.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

using namespace std;

/* 	PROMPT FUNCTIONS  	*/

// get an integer input between minVal and maxVal
int getInt(int minVal, int maxVal)
{
	int num;
	while (true)
	{
		cin >> num;
		// check for invalid input
		if (!cin)
		{
			// clear error flag
			cin.clear();
			// remove previous input from buffer
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "! Please enter valid integer" << endl;
			continue;
		}
		if (num < minVal || num > maxVal)
		{
			cout << "! Please enter an integer between " << minVal << " and " << maxVal << endl;
			continue;
		}
		break;
	}

	return num;
}

int promptSelection()
{
	cout << "* Select an option below (type the number) " << endl;
	cout << "0) Exit program" << endl;
	cout << "1) Draw point(s) (up to 5)" << endl;
	cout << "2) Draw a line" << endl;
	cout << "3) Draw a circle" << endl;
	cout << "4) Clear canvas" << endl;

	return getInt(0, 4);
}

int promptPoints()
{
	cout << "How many points do you want to draw? (1 to 5)" << endl;
	//int num = getInt(1, 5);
	return getInt(1, 5);
}

auto promptPointData()
{
	int x, y;
	uint32_t color;

	cout << "Where do you want to draw your point?" << endl;
	cout << "x value: ";
	x = getInt(0, SCREEN_WIDTH);
	cout << "y value: ";
	y = getInt(0, SCREEN_HEIGHT);
	cout << "hex color with alpha (0x00FF00FF) ";
	cin >> hex >> color;

	return make_tuple(x, y, color);
}

/*
	Get data for new line from cin.
	Always returns point with smallest x value first
*/
auto promptLineData()
{
	int x1, y1, x2, y2;
	cout << "Specify line end points" << endl;
	cout << "x1: ";
	x1 = getInt(0, SCREEN_WIDTH);
	cout << "y1: ";
	y1 = getInt(0, SCREEN_HEIGHT);
	cout << "x2: ";
	x2 = getInt(0, SCREEN_WIDTH);
	cout << "y2: ";
	y2 = getInt(0, SCREEN_HEIGHT);

	// model data accordingly and return
	if (x1 <= x2)
	{
		return make_tuple(x1, y1, x2, y2);
	}
	else
	{
		return make_tuple(x2, y2, x1, y1);
	}
}

auto promptTranslationData()
{
	int xDiff, yDiff;
	cout << "translation in x value: ";
	xDiff = getInt(-SCREEN_WIDTH, SCREEN_WIDTH);
	cout << "translation in y value: ";
	yDiff = getInt(-SCREEN_HEIGHT, SCREEN_HEIGHT);
	return make_tuple(xDiff, yDiff);
}

auto promptLineRotationData()
{
	cout << "integer degrees to rotate about center: ";
	return getInt(-360, 360);
}

auto promptCircleData(){
	int x, y, r;
	cout << "x value: ";
	x = getInt(0, SCREEN_WIDTH);
	cout << "y value: ";
	y = getInt(0, SCREEN_HEIGHT);
	cout << "radius: ";
	r = getInt(0, SCREEN_HEIGHT);
	return make_tuple(x, y, r);
}

int promptScaleCircleData(){
	cout << "scale factor: ";
	return getInt(-1000, 1000);
}

/* 	DRAWING FUNCTIONS  */

void display(SDL_Renderer *renderer, SDL_Surface *canvas, SDL_Texture *texture)
{
	// copy canvas to GPU
	SDL_UpdateTexture(texture, NULL, canvas->pixels, canvas->pitch);
	// draw to window
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	// redraw window
	SDL_RenderPresent(renderer);
}

void drawPoint(uint32_t (*pixels)[SCREEN_WIDTH], int x, int y, uint32_t color)
{
	pixels[y][x] = color;
}

void drawLine(uint32_t (*pixels)[SCREEN_WIDTH], int x1, int y1, int x2, int y2, uint32_t color)
{
	float m;
	if (x1 == x2){
		int lowY = min(y1, y2);
		int highY = max(y1, y2);
		for (int y = lowY; y <= highY; y++)
			drawPoint(pixels, x1, y, color);
	}
	else {
		m = (float)(y2 - y1) / (float)(x2 - x1);

		float b = (y1 - (m * x1));
		int y;
		int x;
		for (int x = x1; x <= x2; ++x)
		{
			y = (int)(m * x + b);
			drawPoint(pixels, x, y, color);
		}
		if (m != 0){
			int yLow = min(y1, y2);
			int yHigh = max(y1, y2);
			for (int y = yLow; y <= yHigh; ++y){
				x = (y - b) / m;
				drawPoint(pixels, x, y, color);
			}
		}
	}
}

void rotatePixel(uint32_t (*pixels)[SCREEN_WIDTH], int x, int y, int rotationAngle)
{
	// set pivot to center
	int const cx = SCREEN_WIDTH/2;
	int const cy = SCREEN_HEIGHT/2;

	// convert to radians
	float theta = (rotationAngle * M_PI)/180;

	// calculate sine and cosine values
	float sine = sin(theta);
	float cosine = cos(theta);

	// translate points relative to the center
	x -= cx;
	y -= cy;

	// rotate the points about the origin (0,0)
	float xr = (x * cosine) - (y * sine);
	float yr = (x * sine) + (y * cosine);

	// translate points back to original location
	x = xr + cx;
	y = yr + cy;

	// don't draw offscreen pixels (they'll overlap to wrong pixels)
	if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT)
		return;

	drawPoint(pixels, x, y, 0x0000FFFF);
}

void rotateLine(uint32_t (*pixels)[SCREEN_WIDTH], int x1, int y1, int x2, int y2, int angle)
{
	if (x1 == x2){
		int lowY = min(y1, y2);
		int highY = max(y1, y2);
		for (int y = lowY; y <= highY; y++)
			rotatePixel(pixels, x1, y, angle);
	}
	else {
		float m = (float)(y2 - y1) / (float)(x2 - x1);
		float b = (y1 - (m * x1));
		int y;
		int x;
		for (int x = x1; x <= x2; ++x)
		{
			y = (int)(m * x + b);
			rotatePixel(pixels, x, y, angle);
		}
		if (m != 0)
		{
			int yLow = min(y1, y2);
			int yHigh = max(y1, y2);
			for (int y = yLow; y <= yHigh; ++y)
			{
				x = (y - b) / m;
				rotatePixel(pixels, x, y, angle);
			}
		}
	}
}

void drawCircle(uint32_t (*pixels)[SCREEN_WIDTH], int a, int b, int r, uint32_t color){
	// r^2 = (x-a)^2 + (y-b)^2
	// y = sqrt( r^2 - (x-a)^2 ) + b
	for (int x = a-r; x <= a+r; ++x){
		int y = sqrt( pow(r,2) - pow(x-a,2) ) + b;
		drawPoint(pixels, x, y, color);
		drawPoint(pixels, x, b-(y-b), color);
	}
	// x = sqrt( r^2 - (y-b)^2 ) + a
	for (int y = b-r; y < b+r; ++y){
		int x = sqrt( pow(r,2) - pow(y-b,2) ) + a;
		drawPoint(pixels, x, y, color);
		drawPoint(pixels, a-(x-a), y, color);
	}
}

int main()
{

	// Verify that SDL2 is initialized
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	// Verify SDL2_ttf library is initialized (for text)
	if (TTF_Init() < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize SDL2_ttf: %s\n", TTF_GetError());
		return 1;
	}


	// Create window to display graphics
	SDL_Window* window = SDL_CreateWindow(
		"assignment1",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	// check if window created successfully
	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create window: %s\n", SDL_GetError());
		return 1;
	}

	// create and check renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);		// using prefered graphics
	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	// create and check canvas
	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
	if (canvas == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create canvas: %s\n", SDL_GetError());
		return 1;
	}

	// create and check texture
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create texture: %s\n", SDL_GetError());
		return 1;
	}

	// get pointer to canvas pixels
	uint32_t (*pixels)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH]) canvas->pixels;

	SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0xCC, 0xCC, 0xCC));
	display(renderer, canvas, texture);
	
	
	int choice;
	while(true){
		choice = promptSelection();
		// Exit program
		if (choice == 0){
			cout << "Exiting.." << endl;
			// free resources
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(canvas);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			
			// exit program
			SDL_Quit();
			break;
		}
		// Draw points
		else if (choice == 1){
			int numPoints = promptPoints();
			for (int i = 0; i < numPoints; ++i){
				auto [x, y, color] = promptPointData();
				drawPoint(pixels, x, y, color);
				display(renderer, canvas, texture);
			}
		}
		// Draw line
		else if (choice == 2){
			// create initial line
			auto [x1, y1, x2, y2] = promptLineData();
			drawLine(pixels, x1, y1, x2, y2, 0xFF0000FF);
			display(renderer, canvas, texture);

			// (i) translate line
			auto [xDiff, yDiff] = promptTranslationData();
			x1 += xDiff;
			y1 += yDiff;
			x2 += xDiff;
			y2 += yDiff;
			drawLine(pixels, x1, y1, x2, y2, 0xFF0000FF);
			display(renderer, canvas, texture);

			// (ii) rotate line
			int degrees = promptLineRotationData();
			rotateLine(pixels, x1, y1, x2, y2, degrees);
			display(renderer, canvas, texture);
		}
		// Draw circle
		else if (choice == 3){
			auto [x, y, r] = promptCircleData();
			drawCircle(pixels, x, y, r, 0xFF0000FF);
			display(renderer, canvas, texture);

			// (i) translate circle
			auto [xDiff, yDiff] = promptTranslationData();
			x += xDiff;
			y += yDiff;
			drawCircle(pixels, x, y, r, 0xFF0000FF);
			display(renderer, canvas, texture);

			// (ii) scale circle
			int scaleFactor = promptScaleCircleData();
			r *= scaleFactor;
			drawCircle(pixels, x, y, r, 0xFF0000FF);
			display(renderer, canvas, texture);
		}
		// clear canvas
		else if (choice == 4){
			SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0xCC, 0xCC, 0xCC));
			display(renderer, canvas, texture);
		}
	}	

	return 0;
}


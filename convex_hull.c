#include <stdio.h>
#include <SDL.h> /*graphics library https://www.wikihow.com/Set-Up-SDL-with-Visual-Studio */
#include <stdlib.h> //qsort()
#include <stdbool.h> //bool
#include <windows.h> //sleep()


//screen dimension 
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

bool init();
void close();


SDL_Window* gWindow = NULL;
//window renderer
SDL_Renderer* gRenderer = NULL;


typedef struct Pixel {

	int x, y; //coordinates of a pixel

}pixel;

typedef struct Stack
{
	int top;
	unsigned capacity;
	pixel* array;
}stack;

pixel low; //bottommost pixel

pixel hull; //stack implementation using array

void push(struct Stack* stack, pixel item);
pixel pop(struct Stack* stack);
struct Stack* createStack(unsigned capacity);

//GRAPHICS FUNCTIONS

bool init()
{
	//flag
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Convex Hull drawing...", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}
bool loadMedia()
{
	bool success = true;

	return success;
}

void markPixelW(pixel p) {

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); //white 
	SDL_RenderDrawPoint(gRenderer, p.x, p.y);
}
void markPixelR(pixel p) {

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF); //red
	SDL_RenderDrawPoint(gRenderer, p.x, p.y);
}

void connectPixels(pixel p1, pixel p2) {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);  //red
	SDL_RenderDrawLine(gRenderer, p1.x, p1.y, p2.x, p2.y); //drawing line between p1 and p2
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void swap(pixel* p1, pixel* p2) {
	pixel temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

//find bottom-most pixel 
int bottommostPixel(pixel pixArray[], int r) {
	int ymin = pixArray[0].y;
	int min = 0, i;
	int y;
	for (i = 1; i < r; i++) {
		y = pixArray[i].y;
		if ((y < ymin) || (ymin == y && (pixArray[i].x < pixArray[min].x))) {
			ymin = pixArray[i].y;
			min = i;
		}
	}
	return min;

}
//distance between p1 and p2
int distance(pixel p1, pixel p2) {
	return ((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

/*find orientantion of ordered triplet (p, q, r)
Return:
0 - p, q and r are colinear
1 - clockwise
2 - counterclockwise
*/

int orientation(pixel p, pixel q, pixel r) {
	int value = ((q.y - p.y)*(r.x - q.x) - (q.x - p.x)*(r.y - q.y));
	if (value == 0) return 0; //colinear
	return (value > 0) ? 1 : 2; //clock or counterclock wise
}

//compare pixels/ function used by library function qsort() to sort pixels
int compare(const void *vp1, const void *vp2) {
	pixel *p1 = (pixel *)vp1;
	pixel *p2 = (pixel *)vp2;

	//find orientation
	int o = orientation(low, *p1, *p2);
	if (o == 0) {
		return(distance(low, *p2) >= distance(low, *p1)) ? -1 : 1;
	}
	return (o == 2) ? -1 : 1;
}
pixel nextToTop(stack* hull)
{
	pixel p = hull->array[hull->top];
	pop(hull);
	pixel res = hull->array[hull->top];
	push(hull, p);
	return res;
}


int main()
{
	int i, j, m = 1, k=1; //counters
	pixel *foo, *tmp;
	int min; //index
	int sizeStack;

	//nullptr
	pixel null;
	null.x = 0;
	null.y = 0;


	//640x480/{(0,0)}
	pixel pixArray[] = { {30, 20}, {600, 100}, {230, 430}, {130, 90}, {280, 390}, {460, 120}, {600, 360}, {530, 46}, {230, 240}, {180, 298}, {230, 450}, {390, 457}, {560, 430}, {150, 400}, {50, 380}, {20, 310}, {250, 250} };



	int n = sizeof(pixArray) / sizeof(pixArray[0]);


	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			
			//main loop flag
			bool quit = false;

			//event handler
			SDL_Event e;

			//clear screen
			SDL_SetRenderDrawColor(gRenderer, 0x40, 0x40, 0x40, 0xFF); //grey
			SDL_RenderClear(gRenderer);

			min = bottommostPixel(pixArray, n); //finding bottommost pixel

			printf("bottommost_pixel = (%d, %d)\n", pixArray[min].x, pixArray[min].y);
			SDL_RenderPresent(gRenderer);
			Sleep(1000);
			swap(&pixArray[0], &pixArray[min]);


			for (i = 0; i < n; i++) {
				if (i == 0) {
					markPixelR(pixArray[0]);
				}
				else markPixelW(pixArray[i]);
			}
			low = pixArray[0];

			qsort(&pixArray[1], n - 1, sizeof(pixel), compare);

			for (i = 1; i < n; i++) {
				while ((i < (n - 1) && (orientation(low, pixArray[i], pixArray[i + 1]) == 0))) {
					i++;
				}
				pixArray[m] = pixArray[i];
				m++;
			}

			if (m < 3) {
				printf("Less than three pixels were given. Convex hull isnot possible.\n");
				close();
				return 0;
			}

			struct Stack* hull = createStack(m);

			push(hull, pixArray[0]);
			push(hull, pixArray[1]);
			push(hull, pixArray[2]);

			for (i = 3; i < m; i++) {
				while (orientation(nextToTop(hull), hull->array[hull->top], pixArray[i]) != 2) {
					pop(hull);
				}
				push(hull, pixArray[i]);
			}

			sizeStack = hull->capacity;
			pixel *cloak = malloc(sizeof(pixel) * (sizeStack+1));


			i = 0;
			while (i<=(sizeStack+1)) {
				cloak[i] = null;
				i++;
			}
			
			i = 0;
			while (!isEmpty(hull)) {
				if (i == 0) {
					cloak[i] = low;
					i++;
				}
				else {
					pixel *p = &(hull->array[hull->top]);
					cloak[i] = *p;
					pop(hull);
					i++;
				}

			}
			SDL_RenderPresent(gRenderer);

			j = 0;
			while (cloak[j].x!=0 && cloak[j].y!=0) {
				j++;
			}


			printf("Vertices : \n");
			for (i = j; i > 0; i--) {
				if (cloak[i].x!=0 && cloak[i].y!=0) {
					connectPixels(cloak[i], cloak[i - 1]);
					printf("%d. ( %d , %d ) \n", k, cloak[i].x, cloak[i].y);
					SDL_RenderPresent(gRenderer);
					Sleep(500);
					k++;
				}
			}
			
			//while application is running
			while (!quit) {
				while (SDL_PollEvent(&e) != 0)
					if (e.type == SDL_QUIT) quit = true; //user requests quit
				//update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	//close SDL
	close();

	return 0;
}

//stack implementation

struct Stack* createStack(unsigned capacity)
{
	struct Stack* stack = (struct Stack*) calloc(1, sizeof(struct Stack));
	stack->capacity = capacity;
	stack->top = -1;
	stack->array = (pixel*) calloc(stack->capacity, sizeof(pixel));
	return stack;
}

// Stack is full when top is equal to the last index 
int isFull(struct Stack* stack)
{
	return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1 
int isEmpty(struct Stack* stack)
{
	return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1 
void push(struct Stack* stack, pixel item)
{
	if (isFull(stack))
		return;
	stack->array[++stack->top] = item;
}

// Function to remove an item from stack.  It decreases top by 1 
pixel pop(struct Stack* stack)
{
	if (!isEmpty(stack))
		return stack->array[stack->top--];
	else return;
}


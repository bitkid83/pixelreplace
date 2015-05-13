#include <stdio.h>
#include <cstdlib>
#include "SDL.h"
#include "SDL_image.h"

//SDL_PIXELFORMAT_UNKNOWN
//SDL_PIXELFORMAT_INDEX1LSB
//SDL_PIXELFORMAT_INDEX1MSB
//SDL_PIXELFORMAT_INDEX4LSB
//SDL_PIXELFORMAT_INDEX4MSB
//SDL_PIXELFORMAT_INDEX8
//SDL_PIXELFORMAT_RGB332
//SDL_PIXELFORMAT_RGB444
//SDL_PIXELFORMAT_RGB555
//SDL_PIXELFORMAT_BGR555
//SDL_PIXELFORMAT_ARGB4444
//SDL_PIXELFORMAT_RGBA4444
//SDL_PIXELFORMAT_ABGR4444
//SDL_PIXELFORMAT_BGRA4444
//SDL_PIXELFORMAT_ARGB1555
//SDL_PIXELFORMAT_RGBA5551
//SDL_PIXELFORMAT_ABGR1555
//SDL_PIXELFORMAT_BGRA5551
//SDL_PIXELFORMAT_RGB565
//SDL_PIXELFORMAT_BGR565
//SDL_PIXELFORMAT_RGB24
//SDL_PIXELFORMAT_BGR24
//SDL_PIXELFORMAT_RGB888
//SDL_PIXELFORMAT_RGBX8888
//SDL_PIXELFORMAT_BGR888
//SDL_PIXELFORMAT_BGRX8888
//SDL_PIXELFORMAT_ARGB8888
//SDL_PIXELFORMAT_RGBA8888
//SDL_PIXELFORMAT_ABGR8888
//SDL_PIXELFORMAT_BGRA8888
//SDL_PIXELFORMAT_ARGB2101010
//SDL_PIXELFORMAT_YV12
//planar mode : Y + V + U(3 planes)
//SDL_PIXELFORMAT_IYUV
//planar mode : Y + U + V(3 planes)
//SDL_PIXELFORMAT_YUY2
//packed mode : Y0 + U0 + Y1 + V0(1 plane)
//SDL_PIXELFORMAT_UYVY
//packed mode : U0 + Y0 + V0 + Y1(1 plane)
//SDL_PIXELFORMAT_YVYU
//packed mode : Y0 + V0 + Y1 + U0(1 plane)
const char *PixelFormat(Uint32 f)
{
	const char *tmp = 0;

	// todo: add more cases
	switch (f) {
	case SDL_PIXELFORMAT_RGB24:
		tmp = "RGB24";
		break;
	case SDL_PIXELFORMAT_RGBA8888:
		tmp = "RGBA8888";
		break;
	case SDL_PIXELFORMAT_ARGB8888:
		tmp = "ARGB8888";
		break;
	case SDL_PIXELFORMAT_ABGR8888:
		tmp = "ABGR8888";
		break;
	}

	return tmp;
}

int main(int argc, char *argv[])
{
	char *imageFile = "colors.png";

	if (argv[1] != NULL) {
		imageFile = argv[1];
	}

	bool done = false;
	bool autoSwatch = false;
	
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Surface *loadedImage;
	SDL_Surface *swatch;
	SDL_Texture *imgTexture;
	SDL_Texture *swatchTex;
	SDL_Rect swatchRec;
	SDL_Event evt;

	Uint32 imgw = 0;
	Uint32 imgh = 0;
	Uint32 imgpitch = 0;
	Uint32 imgpixfmt = 0;
	Uint8 imgbpp = 0;
	Uint8 imgbytespp = 0;

	Uint32 mousex = 0;
	Uint32 mousey = 0;

	Uint8 *p = NULL;
	Uint8 r, g, b, a;
	Uint32 pickedColor = 0;
	
	r = g = b = a = 0;

	// SDL2 & SDL2_image init
	//todo: error checking
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
		
	// load the image file
	loadedImage = IMG_Load(imageFile);

	// image loading failed
	if (!loadedImage) {
		printf("IMG_GetError: %s\n", IMG_GetError());
	} 
	// image loading success
	else {
		// get dimensions of the image
		imgw = loadedImage->w;
		imgh = loadedImage->h;
		imgpixfmt = loadedImage->format->format;
		imgbpp = loadedImage->format->BitsPerPixel;
		imgbytespp = loadedImage->format->BytesPerPixel;
		imgpitch = loadedImage->pitch;
		
		printf("IMG_Load success: %s (%dx%d)\n", imageFile, imgw, imgh);
		printf("Pixel format: %s (%d)\n", PixelFormat(imgpixfmt), imgpixfmt);
		printf("Bits Per Pixel: %d\n", imgbpp);
		printf("Bytes Per Pixel: %d\n", imgbytespp);
		printf("Stride: %d bytes\n", imgpitch);
	}
	
	// create SDL window and renderer
	window = SDL_CreateWindow("Image Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, imgw, imgh, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// create texture from loaded image's surface
	imgTexture = SDL_CreateTextureFromSurface(renderer, loadedImage);

	// draw the image on the window
	SDL_Rect imgsource = { 0, 0, imgw, imgh };
	SDL_RenderCopy(renderer, imgTexture, &imgsource, NULL);

	swatch = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);

	while (!done) {
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				done = true;
				break;
			case SDL_KEYDOWN:
				done = (evt.key.keysym.sym == SDLK_ESCAPE) ? true : false;
				//if (evt.key.keysym.sym == SDLK_ESCAPE) { done = true; }
				break;
			case SDL_MOUSEMOTION:
				//printf("Mouse: %5d, %5d\r", evt.motion.x, evt.motion.y);
				mousex = evt.motion.x;
				mousey = evt.motion.y;
				p = (Uint8 *)loadedImage->pixels + mousey * imgpitch + mousex * (imgbpp / 8);
				SDL_GetRGBA(*(Uint32 *)p, loadedImage->format, &r, &g, &b, &a);
				//SDL_FillRect(swatch, NULL, SDL_MapRGBA(loadedImage->format, b, g, r, 255)); // BGRA
				SDL_FillRect(swatch, NULL, SDL_MapRGBA(swatch->format, r, g, b, 255));
				swatchTex = SDL_CreateTextureFromSurface(renderer, swatch);				
				swatchRec = { 5, 5, 100, 100 };
				SDL_RenderCopy(renderer, swatchTex, NULL, &swatchRec);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (evt.button.button == SDL_BUTTON_LEFT) {	
					//r = g = b = a = 0;
					// get position of the mouse
					//mousex = evt.motion.x;
					//mousey = evt.motion.y;
					
					// get the color value for the pixel selected by mouse
					//p = (Uint8 *)loadedImage->pixels + mousey * imgpitch + mousex * (imgbpp / 8);
					pickedColor = *(Uint32 *)p;
					//*p = p[0] | p[1] << 8 | p[2] << 16;			
					
					// convert to RGBA values
					//SDL_GetRGBA(*(Uint32 *)p, loadedImage->format, &r, &g, &b, &a);

					printf("%d %d %d %d (RGBA)\n", r, g, b, a);
					printf("Mouse: %5d, %5d\n", mousex, mousey);
				}
				else if (evt.button.button == SDL_BUTTON_RIGHT) {
					Uint8 *pt;
					
					// replace selected pixels with the previously selected color with 0xFF00FF00 (SDL wants ARGB)
					for (Uint32 y = 0; y < imgh; y++) {
						for (Uint32 x = 0; x < imgw; x++) {							
							pt = (Uint8 *)loadedImage->pixels + y * imgpitch + x * (imgbpp / 8);							
							if (*(Uint32 *)pt == pickedColor/* *(Uint32 *)p */) { *(Uint32 *)pt = (Uint32)0xFF00FF00 * (imgbpp / 8); }
						}
					}
					SDL_Texture *tmptex = SDL_CreateTextureFromSurface(renderer, loadedImage);
					SDL_RenderCopy(renderer, tmptex, &imgsource, NULL);
				}
				break;
			}
		}

		SDL_RenderPresent(renderer);
	}	

	//SDL_Delay(1000);

	// SDL2 & SDL2_image cleanup
	loadedImage = NULL;

	SDL_DestroyTexture(imgTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);	
	
	IMG_Quit();
	SDL_Quit();

	return 0;
}
#include "SDLauxiliary.h"
#include <vector>
#include "Utilities.h"

SDL_Surface* InitializeSDL( int width, int height, bool fullscreen )
{
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
	{
		std::cout << "Could not init SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}
	atexit( SDL_Quit );

	Uint32 flags = SDL_SWSURFACE;
	if( fullscreen )
		flags |= SDL_FULLSCREEN;

	SDL_Surface* surface = 0;
	surface = SDL_SetVideoMode( width, height, 32, flags );
	if( surface == 0 )
	{
		std::cout << "Could not set video mode: "
				  << SDL_GetError() << std::endl;
		exit(1);
	}
	return surface;
}

bool NoQuitMessageSDL()
{
	SDL_Event e;
	while( SDL_PollEvent(&e) )
	{
		if( e.type == SDL_QUIT )
			return false;
		if( e.type == SDL_KEYDOWN )
			if( e.key.keysym.sym == SDLK_ESCAPE)
				return false;
	}
	return true;
}

// TODO: Does this work on all platforms?
void PutPixelSDL( SDL_Surface* surface, int x, int y, glm::vec3 color )
{
	if( x < 0 || surface->w <= x || y < 0 || surface->h <= y )
		return;

	Uint8 r = Uint8( glm::clamp( 255*color.r, 0.f, 255.f ) );
	Uint8 g = Uint8( glm::clamp( 255*color.g, 0.f, 255.f ) );
	Uint8 b = Uint8( glm::clamp( 255*color.b, 0.f, 255.f ) );

	Uint32* p = (Uint32*)surface->pixels + y*surface->pitch/4 + x;
	*p = SDL_MapRGB( surface->format, r, g, b );
}

void DrawLineSDL(SDL_Surface* surface, glm::ivec2 a, glm::ivec2 b, glm::vec3 color)
{
	std::vector<glm::ivec2> line;
	int delta_x = glm::abs(a.x - b.x);
	int delta_y = glm::abs(a.y - b.y);
	int pixels = glm::max(delta_x, delta_y) + 1;
	line.resize(pixels);
	Interpolate(a, b, line);
	for (auto pixel : line)
	{
		PutPixelSDL(surface, pixel.x, pixel.y, color);
	}
}
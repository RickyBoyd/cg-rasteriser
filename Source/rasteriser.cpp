#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "Triangle.h"
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/detail/type_mat.hpp>
#include <glm/detail/type_mat.hpp>
#include <algorithm>
#include "Model.h"
#include "Light.h"
#include "Scene.h"
#include "Material.h"

#define EDGE_AA

#if defined _WIN32 || defined _WIN64
extern "C" {
	FILE __iob_func[3] = { stdin, stdout,*stderr };
}
extern "C" {
	FILE __imp_fprintf[3] = { stdin, stdout,*stderr };
}
#endif

struct Pixel
{
	int x;
	int y;
	float z_inv;
};

using glm::vec3;
using glm::mat3;
using glm::ivec2;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
//const int AA_SAMPLES = 4;
//const glm::vec2 JITTER_MATRIX[AA_SAMPLES] = { glm::vec2(-0.25, 0.75), glm::vec2(0.75, 0.25), glm::vec2(-0.75, -0.25), glm::vec2(0.25, -0.75) };
SDL_Surface *screen;
int t;
const float FOCAL_LENGTH = SCREEN_WIDTH / 2;

void Update(Scene &scene, Uint8 &lightSelected);

void Draw(Scene &scene, const std::vector<Triangle> &triangles);

void Interpolate(float a, float b, std::vector<float> &result);
void Interpolate( ivec2 a, ivec2 b, std::vector<ivec2>& result );

void VertexShader(const vec3& v, Scene &scene, Pixel& p);

void DrawLineSDL( SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color );
void ComputeLine( SDL_Surface* surface, ivec2 a, ivec2 b, std::vector<ivec2> &line );

void DrawPolygonEdges( const std::vector<vec3>& vertices, Scene &scene );

void DrawPolygonRows( const std::vector<Pixel>& leftPixels, const std::vector<Pixel>& rightPixels, vec3 color );

void DrawPolygon( const std::vector<vec3>& vertices, vec3 color, Scene& scene );

void ComputePolygonRows(const std::vector<Pixel>& vertexPixels, std::vector<Pixel>& leftPixels, std::vector<Pixel>& rightPixels );

int main(int argc, char *argv[]) {
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();    // Set start value for timer.

	Uint8 lightSelected = 0;

	auto cubeScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cube.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 1.0f });

	auto cornellBoxScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 0.4f });

	auto cornellBoxTransparentScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box_transparency.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 1.0f });

#ifdef IMPORT_COMPLEX_MODELS
	auto bunnyBoxScene = Scene(
		std::vector<ModelInstance> {
		ModelInstance(Model("Resources/cornell_box_empty.obj")),
			ModelInstance(Model("Resources/bunny_transparent.obj"), glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(12.0f, 12.0f, 12.0f))
	},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
			Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto bunnyScene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/bunny.obj"), glm::vec3(0.0f, 0.0f, 0.0f)) },
		std::vector<Light> {
		Light{ vec3(0.0f, 0.5f, -1.0f), 15.0f * vec3(1,1,1) },
			Light{ vec3(0.5f, 0.1f, 0.0f), 15.0f * vec3(1,1,1) }},
		Camera{ glm::vec3(0.0f, 0.1f, -0.15f), 0.0f, 0.0f, 0.0f });

	auto teapotScene = Scene(
		std::vector<ModelInstance> {
		ModelInstance(Model("Resources/teapot.obj"), glm::vec3(-3.0f, 0.0f, 0.0f)),
			ModelInstance(Model("Resources/cube.obj"), glm::vec3(3.0f, 0.0f, 0.0f))
	},
		std::vector<Light> {
			Light{ vec3(3.0f, 2.0f, 0.0f), 100.0f * vec3(1,1,1) },
				Light{ vec3(-3.0f, 4.0f, 2.0f), 100.0f * vec3(1,1,1) },
				Light{ vec3(-3.0f, 4.0f, -2.0f), 30.0f * vec3(1,1,1) }},
			Camera{ glm::vec3(0.0f, 4.0f, -7.0f), 30.0f, 0.0f, 0.0f });
#endif

	Scene &scene = cornellBoxScene;

	std::vector<Triangle> sceneTris = scene.ToTriangles();
	std::cout << "Loaded " << sceneTris.size() << " tris" << std::endl;



	// std::vector<ivec2> vertexPixels(3);
	// vertexPixels[0] = ivec2(10, 5);
	// vertexPixels[1] = ivec2( 5,10);
	// vertexPixels[2] = ivec2(15,15);
	// std::vector<ivec2> leftPixels;
	// std::vector<ivec2> rightPixels;
	// ComputePolygonRows( vertexPixels, leftPixels, rightPixels );
	// for( int row=0; row<leftPixels.size(); ++row )
	// {
	// 	std::cout << "Start: ("
	// 	<< leftPixels[row].x << ","
	// 	<< leftPixels[row].y << "). "
	// 	<< "End: ("
	// 	<< rightPixels[row].x << ","
	// 	<< rightPixels[row].y << "). " << std::endl;
	// }

	while (NoQuitMessageSDL()) {
		Draw(scene, sceneTris);
		Update(scene, lightSelected);
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void Draw(Scene &scene, const std::vector<Triangle> &triangles)
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for (auto triangle : triangles)
	{
		std::vector<vec3> vertices(3);
		vertices[0] = triangle.v0;
		vertices[1] = triangle.v1;
		vertices[2] = triangle.v2;

		DrawPolygon( vertices, triangle.color, scene );
	}
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void DrawPolygon( const std::vector<vec3>& vertices, vec3 color, Scene& scene )
{
	int V = vertices.size();
	std::vector<Pixel> vertexPixels( V );
	for( int i=0; i<V; ++i )
	{

		VertexShader( vertices[i], scene, vertexPixels[i]);
	}

	std::vector<Pixel> leftPixels;
	std::vector<Pixel> rightPixels;
	ComputePolygonRows( vertexPixels, leftPixels, rightPixels );
	DrawPolygonRows( leftPixels, rightPixels, color );
}

void DrawPolygonRows( const std::vector<Pixel>& leftPixels, const std::vector<Pixel>& rightPixels, vec3 color )
{
	for(int j = 0; j < leftPixels.size(); j++)
	{
		for(int x = leftPixels[j].x; x <= rightPixels[j].x; x++)
		{
			PutPixelSDL(screen, x, leftPixels[j].y, color);
		}
	}
}

void ComputePolygonRows(const std::vector<Pixel>& vertexPixels, std::vector<Pixel>& leftPixels, std::vector<Pixel>& rightPixels )
{
// 1. Find max and min y-value of the polygon
// and compute the number of rows it occupies.
// 2. Resize leftPixels and rightPixels
// so that they have an element for each row.
// 3. Initialize the x-coordinates in leftPixels
// to some really large value and the x-coordinates
// in rightPixels to some really small value.
	
	int miny = std::numeric_limits<int>::max();
	int maxy = std::numeric_limits<int>::min();
	for(auto vertex : vertexPixels)
	{
		miny = std::min(vertex.y, miny);
		maxy = std::max(vertex.y, maxy);
	}
	int size = maxy - miny + 1;
	//initialise rows
	leftPixels.resize(size);
	rightPixels.resize(size);
	for( int i=0; i<size; ++i )
	{
		leftPixels[i].x =  std::numeric_limits<int>::max();
		rightPixels[i].x = std::numeric_limits<int>::min();
		leftPixels[i].y  = miny + i; 
		rightPixels[i].y = miny + i;
	}
// 4. Loop through all edges of the polygon and use
// linear interpolation to find the x-coordinate for
// each row it occupies. Update the corresponding
// values in rightPixels and leftPixels.
	for( int i=0; i<vertexPixels.size(); ++i )
	{
		int j = (i+1)%vertexPixels.size(); // The next vertex
		std::vector<ivec2> line;
		ivec2 v_i( vertexPixels[i].x, vertexPixels[i].y);
		ivec2 v_j(  vertexPixels[j].x,  vertexPixels[j].y);
		ComputeLine( screen, v_i, v_j, line );
		for(auto linePixel : line)
		{
			int y = linePixel.y - miny;
			leftPixels[y].x  = std::min(leftPixels[y].x,  linePixel.x);
			rightPixels[y].x = std::max(rightPixels[y].x, linePixel.x);
		}
	}
}

// void DrawPolygonEdges( const std::vector<vec3>& vertices, Scene &scene )
// {
// 	int V = vertices.size();
// 	// Transform each vertex from 3D world position to 2D image position:
// 	std::vector<ivec2> projectedVertices( V );
// 	for( int i=0; i<V; ++i )
// 	{
// 		VertexShader( vertices[i], scene, projectedVertices[i] );
// 	}
// 	// Loop over all vertices and draw the edge from it to the next vertex:
// 	for( int i=0; i<V; ++i )
// 	{
// 		int j = (i+1)%V; // The next vertex
// 		vec3 color( 1, 1, 1 );
// 		DrawLineSDL( screen, projectedVertices[i], projectedVertices[j], color );
// 	}
// }

void VertexShader(const vec3& world_vertex, Scene &scene, Pixel& p)
{
	// Convert the world-space vertex into a camera-space vertex
	vec3 camera_vertex = world_vertex - scene.camera_.position;
	camera_vertex = glm::rotate(camera_vertex, glm::radians(scene.camera_.pitch), vec3(1.0f, 0.0f, 0.0f));
	camera_vertex = glm::rotate(camera_vertex, glm::radians(scene.camera_.yaw), vec3(0.0f, 1.0f, 0.0f));
	camera_vertex = glm::rotate(camera_vertex, glm::radians(scene.camera_.roll), vec3(0.0f, 0.0f, 1.0f));

	p.x = SCREEN_WIDTH *  ( scene.camera_.focal_length * camera_vertex.x / camera_vertex.z + 1.0f / 2.0f);
	p.y = SCREEN_HEIGHT * (-scene.camera_.focal_length * camera_vertex.y / camera_vertex.z + 1.0f / 2.0f);
	p.z_inv = camera_vertex.z == 0.0f ? std::numeric_limits<float>::max() : 1.0f/camera_vertex.z;
}

void DrawLineSDL( SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color )
{	
	std::vector<ivec2> line;
	ComputeLine(screen, a, b, line);
	for(auto pixel : line)
	{
		PutPixelSDL(screen, pixel.x, pixel.y, color);
	}
}

void ComputeLine( SDL_Surface* surface, ivec2 a, ivec2 b, std::vector<ivec2> &line )
{
	ivec2 delta = glm::abs( a - b );
	int pixels = glm::max( delta.x, delta.y ) + 1;
	line.resize(pixels);
	Interpolate( a, b, line );
}


void Interpolate( ivec2 a, ivec2 b, std::vector<ivec2>& result )
{
	int N = result.size();
	glm::vec2 step = glm::vec2(b-a) / static_cast<float>( std::max(N-1, 1) );
	glm::vec2 current( a );
	for( int i=0; i<N; ++i )
	{
		result[i] = current;
		current += step;
	}
}

void Interpolate(float a, float b, std::vector<float> &result) {
	if (result.size() == 0) return;
	if (result.size() == 1) {
		result[0] = (a + b) / 2;
		return;
	}
	else {
		float diff = b - a;
		float step = diff / (result.size() - 1);
		result[0] = a;
		for (int i = 1; i < result.size(); i++) {
			result[i] = result[i - 1] + step;
		}
	}
}

void Update(Scene &scene, Uint8 &lightSelected) {
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	std::cout << "Render time: " << dt << " ms.\n";

	static float movementSpeed = 0.001;
	static float rotateSpeed = 0.01;

	auto keystate = SDL_GetKeyState(nullptr);
	if (keystate[SDLK_UP]) {
		scene.camera_.position.y += dt * movementSpeed;
	}
	if (keystate[SDLK_DOWN]) {
		scene.camera_.position.y -= dt * movementSpeed;
	}
	if (keystate[SDLK_LEFT]) {
		scene.camera_.position.x -= dt * movementSpeed;
	}
	if (keystate[SDLK_RIGHT]) {
		scene.camera_.position.x += dt * movementSpeed;
	}
	if (keystate[SDLK_w]) {
		scene.lights_[lightSelected].position += vec3(0.0f, 0.0f, movementSpeed * dt);
	}
	if (keystate[SDLK_s]) {
		scene.lights_[lightSelected].position += vec3(0.0f, 0.0f, -movementSpeed * dt);
	}
	if (keystate[SDLK_a]) {
		scene.lights_[lightSelected].position += vec3(-movementSpeed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_d]) {
		scene.lights_[lightSelected].position += vec3(movementSpeed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_q]) {
		scene.lights_[lightSelected].position += vec3(0.0f, movementSpeed * dt, 0.0f);
	}
	if (keystate[SDLK_e]) {
		scene.lights_[lightSelected].position += vec3(0.0f, -movementSpeed * dt, 0.0f);
	}
	if (keystate[SDLK_n]) {
		if (scene.lights_.size() < 6) {
			//AddLight(vec3(0.0f, 0.0f, 0.0f), 10.0f * vec3(1.0f, 1.0f, 1.0f), scene.lights_);
			//lightSelected = static_cast<Uint8>(scene.lights_.size()) - 1;
		}
	}

	if (keystate[SDLK_j])
	{
		scene.camera_.yaw -= dt * rotateSpeed;
	}
	if (keystate[SDLK_l])
	{
		scene.camera_.yaw += dt * rotateSpeed;
	}
	if (keystate[SDLK_i])
	{
		scene.camera_.pitch -= dt * rotateSpeed;
	}
	if (keystate[SDLK_k])
	{
		scene.camera_.pitch += dt * rotateSpeed;
	}
	if (keystate[SDLK_u])
	{
		scene.camera_.roll -= dt * rotateSpeed;
	}
	if (keystate[SDLK_o])
	{
		scene.camera_.roll += dt * rotateSpeed;
	}

	if (keystate[SDLK_1] && scene.lights_.size() > 0) {
		lightSelected = 0;
	}
	if (keystate[SDLK_2] && scene.lights_.size() > 1) {
		lightSelected = 1;
	}
	if (keystate[SDLK_3] && scene.lights_.size() > 2) {
		lightSelected = 2;
	}
	if (keystate[SDLK_4] && scene.lights_.size() > 3) {
		lightSelected = 3;
	}
	if (keystate[SDLK_5] && scene.lights_.size() > 4) {
		lightSelected = 4;
	}
	if (keystate[SDLK_6] && scene.lights_.size() > 5) {
		lightSelected = 5;
	}
}

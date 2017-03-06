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
#include "Utilities.h"

#define EDGE_AA

#if defined _WIN32 || defined _WIN64
extern "C" {
	FILE __iob_func[3] = { stdin, stdout,*stderr };
}
extern "C" {
	FILE __imp_fprintf[3] = { stdin, stdout,*stderr };
}
#endif

#if defined(_DEBUG) && (defined _WIN32 || defined _WIN64)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>	
#endif

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

void Draw(Scene &scene, const std::vector<Triangle> &triangles, float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT]);

void VertexShader(const vec3& v, Scene &scene, Pixel& p);

void ComputeLine( Pixel a, Pixel b, std::vector<Pixel> &line );

void DrawPolygonEdges(const std::vector<vec3>& vertices, Scene &scene);
void DrawPolygonRows( const std::vector<Pixel>& leftPixels, const std::vector<Pixel>& rightPixels, vec3 color, float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT] );
void DrawPolygon( const std::vector<vec3>& vertices, vec3 color, Scene& scene, float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT] );
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

	float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];

	while (NoQuitMessageSDL()) {
		Draw(scene, sceneTris, depth_buffer);
		Update(scene, lightSelected);
	}

	SDL_SaveBMP(screen, "screenshot.bmp");

#if defined(_DEBUG) && (defined _WIN32 || defined _WIN64)
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

void Draw(Scene &scene, const std::vector<Triangle> &triangles, float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT])
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for(int x = 0; x < SCREEN_WIDTH; x++)
	{
		for(int y = 0; y < SCREEN_HEIGHT; y++)
		{
			depth_buffer[x][y] = 0;
		}
	}
	for (auto triangle : triangles)
	{
		std::vector<vec3> vertices(3);
		vertices[0] = triangle.v0;
		vertices[1] = triangle.v1;
		vertices[2] = triangle.v2;

		DrawPolygon( vertices, triangle.color, scene, depth_buffer );
		//DrawPolygonEdges(vertices, scene);
	}
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void DrawPolygon( const std::vector<vec3>& vertices, vec3 color, Scene& scene, float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT] )
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
	DrawPolygonRows( leftPixels, rightPixels, color, depth_buffer );
}

void DrawPolygonRows( const std::vector<Pixel>& leftPixels, const std::vector<Pixel>& rightPixels, vec3 color, float depth_buffer[SCREEN_WIDTH][SCREEN_HEIGHT] )
{
	for(int j = 0; j < leftPixels.size(); j++)
	{
		std::vector<Pixel> row;
		ComputeLine(leftPixels[j], rightPixels[j], row);
		for(Pixel pixel : row)
		{
			if(pixel.pos.x >= 0 && pixel.pos.x <= SCREEN_WIDTH && pixel.pos.y >= 0 && pixel.pos.y < SCREEN_HEIGHT)
			{
				if(pixel.z_inv > depth_buffer[pixel.pos.x][pixel.pos.y])
				{
					depth_buffer[pixel.pos.x][pixel.pos.y] = pixel.z_inv;
					PutPixelSDL(screen, pixel.pos.x, pixel.pos.y, color);
				}
			}
		}
	}
}

void ComputePolygonRows(const std::vector<Pixel>& vertex_pixels, std::vector<Pixel>& left_pixels, std::vector<Pixel>& right_pixels)
{
	// Find max and min y-value of the polygon and compute the number of rows it occupies.

	int min_y = std::numeric_limits<int>::max();
	int max_y = std::numeric_limits<int>::min();

	for (auto vertex : vertex_pixels)
	{
		min_y = std::min(vertex.pos.y, min_y);
		max_y = std::max(vertex.pos.y, max_y);
	}
	int rows = max_y - min_y + 1;

	// Resize left_pixels and right_pixels so that they have an element for each row.
	left_pixels.resize(rows);
	right_pixels.resize(rows);

	// 3. Initialize the x-coordinates in left_pixels to some really large value and the x-coordinates in right_pixels to some really small value.
	for (int row = 0; row < rows; ++row)
	{
		left_pixels[row].pos.x = std::numeric_limits<int>::max();
		right_pixels[row].pos.x = std::numeric_limits<int>::min();
		left_pixels[row].pos.y = min_y + row;
		right_pixels[row].pos.y = min_y + row;
	}

	// 4. Loop through all edges of the polygon and use linear interpolation to find the x-coordinate for
	// each row it occupies. Update the corresponding values in right_pixels and left_pixels.
	for (int i = 0; i < vertex_pixels.size(); ++i)
	{
		int j = (i + 1) % vertex_pixels.size(); // the next vertex
		std::vector<Pixel> line;
		ComputeLine(vertex_pixels[i], vertex_pixels[j], line);
		for (auto line_pixel : line)
		{
			int row = line_pixel.pos.y - min_y;
			if (row >= rows)
			{
				std::cout << "row (" << row << ") is greater than number of rows (" << rows << ")" << std::endl;
			}
			else
			{
				if (line_pixel.pos.x < left_pixels[row].pos.x)
				{
					left_pixels[row].z_inv = line_pixel.z_inv;
					left_pixels[row].pos.x = line_pixel.pos.x;
				}
				if (line_pixel.pos.x > right_pixels[row].pos.x)
				{
					right_pixels[row].z_inv = line_pixel.z_inv;
					right_pixels[row].pos.x = line_pixel.pos.x;
				}
			}
		}
	}
}

void DrawPolygonEdges(const std::vector<vec3>& vertices, Scene &scene)
{
	// Transform each vertex from 3D world position to 2D image position:
	std::vector<Pixel> pixels(vertices.size());
	std::transform(vertices.begin(), vertices.end(), pixels.begin(), [&scene](vec3 vertex) -> Pixel
	{
		Pixel p;
		VertexShader(vertex, scene, p);
		return p;
	});
	// Loop over all vertices and draw the edge from it to the next vertex:
	int num_vertices = vertices.size();
	for (int i = 0; i < num_vertices; ++i)
	{
		int j = (i + 1) % num_vertices; // The next vertex
		vec3 color(1, 1, 1);
		DrawLineSDL(screen, pixels[i].pos, pixels[j].pos, color);
	}
}

void VertexShader(const vec3& world_vertex, Scene &scene, Pixel& p)
{
	// Convert the world-space vertex into a camera-space vertex
	vec3 camera_vertex = world_vertex - scene.camera_.position;
	camera_vertex = glm::rotate(camera_vertex, glm::radians(scene.camera_.pitch), vec3(1.0f, 0.0f, 0.0f));
	camera_vertex = glm::rotate(camera_vertex, glm::radians(scene.camera_.yaw), vec3(0.0f, 1.0f, 0.0f));
	camera_vertex = glm::rotate(camera_vertex, glm::radians(scene.camera_.roll), vec3(0.0f, 0.0f, 1.0f));

	p.pos.x = SCREEN_WIDTH *  ( scene.camera_.focal_length * camera_vertex.x / camera_vertex.z + 1.0f / 2.0f);
	p.pos.y = SCREEN_HEIGHT * (-scene.camera_.focal_length * camera_vertex.y / camera_vertex.z + 1.0f / 2.0f);
	p.z_inv = camera_vertex.z == 0.0f ? std::numeric_limits<float>::max() : 1.0f/camera_vertex.z;
}

void ComputeLine(Pixel a, Pixel b, std::vector<Pixel> &line)
{
	int delta_x = glm::abs(a.pos.x - b.pos.x);
	int delta_y = glm::abs(a.pos.y - b.pos.y);
	int pixels = glm::max(delta_x, delta_y) + 1; // TODO: why +1?
	line.resize(pixels);
	Interpolate(a, b, line);
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

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

void Update(Scene &scene, Uint8 &light_selected);
void Draw(const Scene &scene, const std::vector<Triangle> &triangles, std::vector<float>& depth_buffer);

void ComputeLine(const Pixel a, const Pixel b, std::vector<Pixel> &line);
void DrawPolygon(const std::vector<Vertex>& vertices, const vec3 color, const Scene& scene, std::vector<float>& depth_buffer);
void ComputePolygonRows(const std::vector<Pixel>& vertex_pixels, std::vector<Pixel>& left_pixels, std::vector<Pixel>& right_pixels);
void DrawPolygonEdges(const std::vector<Vertex>& vertices, const Scene &scene);
void DrawPolygonRows(const std::vector<Pixel>& left_pixels, const std::vector<Pixel>& right_pixels, std::vector<float>& depth_buffer);
void DrawPixel(const Pixel& pixel, std::vector<float>& depth_buffer);
void ShadeVertex(const Vertex& v, const Scene &scene, Pixel& p);

int main(int argc, char *argv[]) {
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();    // Set start value for timer.

	Uint8 light_selected = 0;

	auto cube_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cube.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 1.0f });

	auto cornell_box_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 0.4f });

	auto cornell_box_transparent_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box_transparency.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 1.0f });

#ifdef IMPORT_COMPLEX_MODELS
	auto bunny_box_scene = Scene(
		std::vector<ModelInstance> {
		ModelInstance(Model("Resources/cornell_box_empty.obj")),
			ModelInstance(Model("Resources/bunny_transparent.obj"), glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(12.0f, 12.0f, 12.0f))
	},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
			0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
			Camera{ glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f });

	auto bunny_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/bunny.obj"), glm::vec3(0.0f, 0.0f, 0.0f)) },
		std::vector<Light> {
		Light{ vec3(0.0f, 0.5f, -1.0f), 15.0f * vec3(1,1,1) },
			Light{ vec3(0.5f, 0.1f, 0.0f), 15.0f * vec3(1,1,1) }},
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera{ glm::vec3(0.0f, 0.1f, -0.15f), 0.0f, 0.0f, 0.0f });

	auto teapot_scene = Scene(
		std::vector<ModelInstance> {
		ModelInstance(Model("Resources/teapot.obj"), glm::vec3(-3.0f, 0.0f, 0.0f)),
			ModelInstance(Model("Resources/cube.obj"), glm::vec3(3.0f, 0.0f, 0.0f))
	},
		std::vector<Light> {
			Light{ vec3(3.0f, 2.0f, 0.0f), 100.0f * vec3(1,1,1) },
				Light{ vec3(-3.0f, 4.0f, 2.0f), 100.0f * vec3(1,1,1) },
				Light{ vec3(-3.0f, 4.0f, -2.0f), 30.0f * vec3(1,1,1) }},
			0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
			Camera{ glm::vec3(0.0f, 4.0f, -7.0f), 30.0f, 0.0f, 0.0f });
#endif

	Scene &scene = cornell_box_scene;

	std::vector<Triangle> scene_tris = scene.ToTriangles();
	std::cout << "Loaded " << scene_tris.size() << " tris" << std::endl;

	std::vector<float> depth_buffer;
	depth_buffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT);

	while (NoQuitMessageSDL()) {
		Draw(scene, scene_tris, depth_buffer);
		Update(scene, light_selected);
	}

	SDL_SaveBMP(screen, "screenshot.bmp");

#if defined(_DEBUG) && (defined _WIN32 || defined _WIN64)
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

void Draw(const Scene &scene, const std::vector<Triangle> &triangles, std::vector<float>& depth_buffer)
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for(int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for(int x = 0; x < SCREEN_WIDTH; x++)
		{
			depth_buffer[y * SCREEN_WIDTH + x] = 0;
		}
	}

	std::vector<Vertex> vertices(3);
	for (auto triangle : triangles)
	{
		vertices[0] = Vertex{ triangle.v0, triangle.normal, triangle.color, triangle.color };
		vertices[1] = Vertex{ triangle.v1, triangle.normal, triangle.color, triangle.color };
		vertices[2] = Vertex{ triangle.v2, triangle.normal, triangle.color, triangle.color };

		DrawPolygon(vertices, triangle.color, scene, depth_buffer);
		//DrawPolygonEdges(vertices, scene);
	}
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void ComputeLine(const Pixel a, const Pixel b, std::vector<Pixel> &line)
{
	int delta_x = glm::abs(a.pos.x - b.pos.x);
	int delta_y = glm::abs(a.pos.y - b.pos.y);
	int pixels = glm::max(delta_x, delta_y) + 1; // TODO: why +1?
	line.resize(pixels);
	Interpolate(a, b, line);
}

void DrawPolygon(const std::vector<Vertex>& vertices, const vec3 color, const Scene& scene, std::vector<float>& depth_buffer)
{
	int num_vertices = vertices.size();
	std::vector<Pixel> vertex_pixels(num_vertices);
	for (int i = 0; i < num_vertices; ++i)
	{
		ShadeVertex(vertices[i], scene, vertex_pixels[i]);
	}

	std::vector<Pixel> left_pixels;
	std::vector<Pixel> right_pixels;
	ComputePolygonRows(vertex_pixels, left_pixels, right_pixels);
	DrawPolygonRows(left_pixels, right_pixels, depth_buffer);
}

void ComputePolygonRows(const std::vector<Pixel>& vertex_pixels, std::vector<Pixel>& left_pixels, std::vector<Pixel>& right_pixels)
{
	// Find max and min y-value of the polygon and compute the number of rows it occupies.

	int min_y = std::numeric_limits<int>::max();
	int max_y = std::numeric_limits<int>::min();

	for (auto vertex : vertex_pixels)
	{
		min_y = std::min(vertex.ipos().y, min_y);
		max_y = std::max(vertex.ipos().y, max_y);
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
			int row = line_pixel.ipos().y - min_y;
			if (row >= rows)
			{
				std::cout << "row (" << row << ") is greater than number of rows (" << rows << ")" << std::endl;
			}
			else
			{
				if (line_pixel.pos.x < left_pixels[row].pos.x)
				{
					left_pixels[row] = line_pixel;
				}
				if (line_pixel.pos.x > right_pixels[row].pos.x)
				{
					right_pixels[row] = line_pixel;
				}
			}
		}
	}
}

void DrawPolygonEdges(const std::vector<Vertex>& vertices, const Scene &scene)
{
	// Transform each vertex from 3D world position to 2D image position:
	std::vector<Pixel> pixels(vertices.size());
	std::transform(vertices.begin(), vertices.end(), pixels.begin(), [&scene](Vertex vertex) -> Pixel
	{
		Pixel p;
		ShadeVertex(vertex, scene, p);
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

void DrawPolygonRows(const std::vector<Pixel>& left_pixels, const std::vector<Pixel>& right_pixels, std::vector<float>& depth_buffer)
{
	for (int i = 0; i < left_pixels.size(); i++)
	{
		std::vector<Pixel> row;
		ComputeLine(left_pixels[i], right_pixels[i], row);
		for (auto pixel : row)
		{
			if (pixel.pos.x >= 0 && pixel.pos.x < SCREEN_WIDTH && pixel.pos.y >= 0 && pixel.pos.y < SCREEN_HEIGHT)
			{
				DrawPixel(pixel, depth_buffer);
			}
		}
	}
}

void DrawPixel(const Pixel& pixel, std::vector<float>& depth_buffer)
{
	if (pixel.z_inv > depth_buffer[pixel.ipos().y * SCREEN_WIDTH + pixel.ipos().x])
	{
		depth_buffer[pixel.ipos().y * SCREEN_WIDTH + pixel.ipos().x] = pixel.z_inv;
		PutPixelSDL(screen, pixel.ipos().x, pixel.ipos().y, pixel.illumination);
	}
}

void ShadeVertex(const Vertex& world_vertex, const Scene &scene, Pixel& p)
{
	// Convert the world-space vertex into a camera-space vertex
	// TODO: cache all these computations
	vec3 camera_vertex_position = world_vertex.pos - scene.camera_.position;
	camera_vertex_position = glm::rotate(camera_vertex_position, glm::radians(scene.camera_.pitch), vec3(1.0f, 0.0f, 0.0f));
	camera_vertex_position = glm::rotate(camera_vertex_position, glm::radians(scene.camera_.yaw), vec3(0.0f, 1.0f, 0.0f));
	camera_vertex_position = glm::rotate(camera_vertex_position, glm::radians(scene.camera_.roll), vec3(0.0f, 0.0f, 1.0f));

	vec3 camera_vertex_normal = world_vertex.normal;
	camera_vertex_normal = glm::rotate(camera_vertex_normal, glm::radians(scene.camera_.pitch), vec3(1.0f, 0.0f, 0.0f));
	camera_vertex_normal = glm::rotate(camera_vertex_normal, glm::radians(scene.camera_.yaw), vec3(0.0f, 1.0f, 0.0f));
	camera_vertex_normal = glm::rotate(camera_vertex_normal, glm::radians(scene.camera_.roll), vec3(0.0f, 0.0f, 1.0f));

	p.pos.x = SCREEN_WIDTH * ( scene.camera_.focal_length * camera_vertex_position.x / camera_vertex_position.z + 1.0f / 2.0f);
	p.pos.y = SCREEN_HEIGHT * (-scene.camera_.focal_length * camera_vertex_position.y / camera_vertex_position.z + 1.0f / 2.0f);
	p.z_inv = camera_vertex_position.z == 0.0f ? std::numeric_limits<float>::max() : 1.0f/camera_vertex_position.z;
	
	p.illumination = glm::vec3(0.0f);
	for (auto light : scene.lights_)
	{
		vec3 camera_vertex_to_light = light.position - world_vertex.pos;
		camera_vertex_to_light = glm::rotate(camera_vertex_to_light, glm::radians(scene.camera_.pitch), vec3(1.0f, 0.0f, 0.0f));
		camera_vertex_to_light = glm::rotate(camera_vertex_to_light, glm::radians(scene.camera_.yaw), vec3(0.0f, 1.0f, 0.0f));
		camera_vertex_to_light = glm::rotate(camera_vertex_to_light, glm::radians(scene.camera_.roll), vec3(0.0f, 0.0f, 1.0f));

		 vec3 direct_light = light.color * std::max(glm::dot(glm::normalize(camera_vertex_to_light), glm::normalize(camera_vertex_normal)), 0.0f) / float(4.0f * M_PI * glm::dot(camera_vertex_to_light, camera_vertex_to_light));
		 p.illumination += direct_light * world_vertex.diffuse_reflectance + scene.indirect_light_ * world_vertex.indirect_reflectance;
	}
}

void Update(Scene &scene, Uint8 &light_selected) {
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	std::cout << "Render time: " << dt << " ms.\n";

	static float movement_speed = 0.001;
	static float rotate_speed = 0.01;

	auto keystate = SDL_GetKeyState(nullptr);
	if (keystate[SDLK_UP]) {
		scene.camera_.position.y += dt * movement_speed;
	}
	if (keystate[SDLK_DOWN]) {
		scene.camera_.position.y -= dt * movement_speed;
	}
	if (keystate[SDLK_LEFT]) {
		scene.camera_.position.x -= dt * movement_speed;
	}
	if (keystate[SDLK_RIGHT]) {
		scene.camera_.position.x += dt * movement_speed;
	}
	if (keystate[SDLK_w]) {
		scene.lights_[light_selected].position += vec3(0.0f, 0.0f, movement_speed * dt);
	}
	if (keystate[SDLK_s]) {
		scene.lights_[light_selected].position += vec3(0.0f, 0.0f, -movement_speed * dt);
	}
	if (keystate[SDLK_a]) {
		scene.lights_[light_selected].position += vec3(-movement_speed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_d]) {
		scene.lights_[light_selected].position += vec3(movement_speed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_q]) {
		scene.lights_[light_selected].position += vec3(0.0f, movement_speed * dt, 0.0f);
	}
	if (keystate[SDLK_e]) {
		scene.lights_[light_selected].position += vec3(0.0f, -movement_speed * dt, 0.0f);
	}
	if (keystate[SDLK_n]) {
		if (scene.lights_.size() < 6) {
			//AddLight(vec3(0.0f, 0.0f, 0.0f), 10.0f * vec3(1.0f, 1.0f, 1.0f), scene.lights_);
			//light_selected = static_cast<Uint8>(scene.lights_.size()) - 1;
		}
	}

	if (keystate[SDLK_j])
	{
		scene.camera_.yaw -= dt * rotate_speed;
	}
	if (keystate[SDLK_l])
	{
		scene.camera_.yaw += dt * rotate_speed;
	}
	if (keystate[SDLK_i])
	{
		scene.camera_.pitch -= dt * rotate_speed;
	}
	if (keystate[SDLK_k])
	{
		scene.camera_.pitch += dt * rotate_speed;
	}
	if (keystate[SDLK_u])
	{
		scene.camera_.roll -= dt * rotate_speed;
	}
	if (keystate[SDLK_o])
	{
		scene.camera_.roll += dt * rotate_speed;
	}

	if (keystate[SDLK_1] && scene.lights_.size() > 0) {
		light_selected = 0;
	}
	if (keystate[SDLK_2] && scene.lights_.size() > 1) {
		light_selected = 1;
	}
	if (keystate[SDLK_3] && scene.lights_.size() > 2) {
		light_selected = 2;
	}
	if (keystate[SDLK_4] && scene.lights_.size() > 3) {
		light_selected = 3;
	}
	if (keystate[SDLK_5] && scene.lights_.size() > 4) {
		light_selected = 4;
	}
	if (keystate[SDLK_6] && scene.lights_.size() > 5) {
		light_selected = 5;
	}
}

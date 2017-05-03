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
#include <algorithm>
#include "Model.h"
#include "Light.h"
#include "Scene.h"
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
#include <crtdbg.h>	
#endif

using glm::vec3;
using glm::mat3;
using glm::ivec2;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

const int AA_SAMPLES = 2;

const int PIXELS_X = SCREEN_WIDTH * AA_SAMPLES;
const int PIXELS_Y = SCREEN_HEIGHT * AA_SAMPLES;

//const int AA_SAMPLES = 4;
//const glm::vec2 JITTER_MATRIX[AA_SAMPLES] = { glm::vec2(-0.25, 0.75), glm::vec2(0.75, 0.25), glm::vec2(-0.75, -0.25), glm::vec2(0.25, -0.75) };
SDL_Surface *screen;
int t;
const float FOCAL_LENGTH = SCREEN_WIDTH / 2;

void Draw(Scene &scene, const std::vector<Triangle> &triangles, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3> &frame_buffer);
void Update(Scene &scene, Uint8 &light_selected);

void ComputeShadowMap(const std::vector<Triangle> &triangles, std::vector<float>& shadow_map);
void DrawTriangleShadow(const Triangle& triangle, const Scene& scene, std::vector<float>& shadow_map);
void DrawPixelShadow(const Pixel& pixel, const Scene& scene, std::vector<float>& shadow_map);

//Drawing functions
void ComputeLine(const Pixel a, const Pixel b, std::vector<Pixel> &line);
void DrawTriangle(const Triangle& triangle, const Scene& scene, glm::mat4& world, glm::mat4& projection, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer);
void ComputePolygonRows(const std::vector<Pixel>& vertex_pixels, std::vector<Pixel>& left_pixels, std::vector<Pixel>& right_pixels);
void DrawTriangleEdges(const Triangle& triangle, const Scene &scene);
void DrawPolygonRows(const Triangle &triangle, const std::vector<Pixel>& left_pixels, const std::vector<Pixel>& right_pixels, const Scene& scene, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer);
void DrawPixel(const Pixel& pixel, const Triangle &triangle, const Scene& scene, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer);
Pixel VertexToPixel(const glm::vec3& world_vertex, const Triangle& triangle, const glm::vec2 vt, const Scene &scene, glm::mat4& world, glm::mat4& projection);

void DrawToScreen(std::vector<glm::vec3>& frame_buffer);

int main(int argc, char *argv[]) {
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();    // Set start value for timer.

	Uint8 light_selected = 0;

	auto cube_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cube.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera( glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 1.0f ));

	auto cornell_box_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera( glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 0.4f ));

	auto cornell_box_transparent_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box_transparency.obj")) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera( glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 1.0f ));

	auto cornell_box_scene_textured = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/cornell_box_empty.obj")), 
			ModelInstance(Model("Resources/cube_textured.obj"), glm::vec3(-0.5f, -0.5f, -0.5f)) },
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera( glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f, 0.4f ));



#ifdef IMPORT_COMPLEX_MODELS
	auto bunny_box_scene = Scene(
		std::vector<ModelInstance> {
		ModelInstance(Model("Resources/cornell_box_empty.obj")),
			ModelInstance(Model("Resources/bunny_transparent.obj"), glm::vec3(0.0f, -1.5f, 0.0f), glm::vec3(12.0f, 12.0f, 12.0f))
	},
		std::vector<Light> { Light{ vec3(-0.3f, 0.5f, -0.7f), 15.0f * vec3(1,1,1) } },
			0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
			Camera( glm::vec3(0.0f, 0.0f, -2.0f), 0.0f, 0.0f, 0.0f ));

	auto bunny_scene = Scene(
		std::vector<ModelInstance> { ModelInstance(Model("Resources/bunny.obj"), glm::vec3(0.0f, 0.0f, 0.0f)) },
		std::vector<Light> {
		Light{ vec3(0.0f, 0.5f, -1.0f), 15.0f * vec3(1,1,1) },
			Light{ vec3(0.5f, 0.1f, 0.0f), 15.0f * vec3(1,1,1) }},
		0.2f * glm::vec3(1.0f, 1.0f, 1.0f),
		Camera( glm::vec3(0.0f, 0.1f, -0.15f), 0.0f, 0.0f, 0.0f ));

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
			Camera( glm::vec3(0.0f, 4.0f, -7.0f), 30.0f, 0.0f, 0.0f ));
#endif

	Scene &scene = cornell_box_scene_textured;

	std::vector<Triangle> world_tris = scene.ToTriangles();
	std::cout << "Loaded " << world_tris.size() << " tris" << std::endl;

	std::vector<float> depth_buffer;
	depth_buffer.resize(PIXELS_X * PIXELS_Y);

	std::vector<float> shadow_map;
	shadow_map.resize(PIXELS_X * PIXELS_Y);

	std::vector<glm::vec3> frame_buffer;
	frame_buffer.resize(PIXELS_X * PIXELS_Y);

	while (NoQuitMessageSDL()) {
		Draw(scene, world_tris, depth_buffer, shadow_map, frame_buffer);
		Update(scene, light_selected);
	}

	SDL_SaveBMP(screen, "screenshot.bmp");

#if defined(_DEBUG) && (defined _WIN32 || defined _WIN64)
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

void Draw(Scene &scene, const std::vector<Triangle> &triangles, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer)
{
	SDL_FillRect(screen, 0, 0);
	if (SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	for (auto &light : scene.lights_)
	{
		light.UpdateCameraSpacePosition(scene.camera_);
	}

	for(int y = 0; y < PIXELS_Y; y++)
	{
		for(int x = 0; x < PIXELS_X; x++)
		{
			depth_buffer[y * PIXELS_X + x] = 0;
			frame_buffer[y * PIXELS_X + x].x = 0;
			frame_buffer[y * PIXELS_X + x].y = 0;
			frame_buffer[y * PIXELS_X + x].z = 0;
		}
	}

	for(int y = 0; y < PIXELS_Y; y++)
	{
		for(int x = 0; x < PIXELS_X; x++)
		{
			shadow_map[y * PIXELS_X + x] = 0;
		}
	}

	glm::mat4 camera_projection = glm::perspective(90.0f, (float)PIXELS_X/(float)PIXELS_Y, 0.1f, 7.5f );

	//glm::worldToLight;
	//glm::mat4 light_projection = glm::ortho(2.0f, 2.0f, 0.1f, 10.0f );
	

	for (auto triangle : triangles)
	{
		DrawTriangle(triangle, scene, scene.camera_.worldToCamera, camera_projection, depth_buffer, shadow_map, frame_buffer);
		//DrawTriangleEdges(triangle, scene);
	}

	DrawToScreen(frame_buffer);

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

// void ComputeShadowMap(const std::vector<Triangle> &triangles, std::vector<float>& shadow_map)
// {
// 	for (auto triangle : triangles)
// 	{
// 		DrawTriangleShadow(triangle, scene, shadow_map);
// 	}
// }

// void DrawTriangleShadow(const Triangle& triangle, const Scene& scene, std::vector<float>& shadow_map)
// {
// 	std::vector<Pixel> vertex_pixels(3);
// 	vertex_pixels[0] = VertexToPixel(glm::vec3(triangle.v0_), triangle, scene);
// 	vertex_pixels[1] = VertexToPixel(glm::vec3(triangle.v1_), triangle, scene);
// 	vertex_pixels[2] = VertexToPixel(glm::vec3(triangle.v2_), triangle, scene);

// 	std::vector<Pixel> left_pixels;
// 	std::vector<Pixel> right_pixels;
// 	ComputePolygonRows(vertex_pixels, left_pixels, right_pixels);
// 	DrawPolygonRowsShadow(left_pixels, right_pixels, scene, depth_buffer);
// }

void DrawToScreen(std::vector<glm::vec3>& frame_buffer)
{
	for(int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for(int x = 0; x < SCREEN_WIDTH; x++)
		{
			vec3 colour = glm::vec3(0.0f, 0.0f, 0.0f);
			for(int y_buff = y*AA_SAMPLES; y_buff < y*AA_SAMPLES + AA_SAMPLES; y_buff++ )
			{
				for(int x_buff = x*AA_SAMPLES; x_buff < x*AA_SAMPLES + AA_SAMPLES; x_buff++ )
				{
					colour += frame_buffer[y_buff * PIXELS_X + x_buff];
				}
			}
			colour /= (float)(AA_SAMPLES*AA_SAMPLES);
			PutPixelSDL(screen, x, y, colour);
		}
	}
}

void ComputeLine(const Pixel a, const Pixel b, std::vector<Pixel> &line)
{
	int delta_x = glm::abs(a.pos.x - b.pos.x);
	int delta_y = glm::abs(a.pos.y - b.pos.y);
	int pixels = glm::max(delta_x, delta_y) + 1; // TODO: why +1?
	line.resize(pixels);
	Interpolate(a, b, line);
}

void DrawTriangle(const Triangle& triangle, const Scene& scene, glm::mat4& world, glm::mat4& projection, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer)
{
	glm::vec3 triCentre = glm::vec3(triangle.v0_ + triangle.v1_ + triangle.v2_ / 3.0f);

	glm::vec3 camToTri = scene.camera_.Position() - triCentre;

	float dot = glm::dot(camToTri, triangle.normal_);
	if(dot < -0.1)
	{
		return;
	}

	std::vector<Pixel> vertex_pixels(3);
	vertex_pixels[0] = VertexToPixel(glm::vec3(triangle.v0_), triangle, triangle.vt0_, scene, world, projection);
	vertex_pixels[1] = VertexToPixel(glm::vec3(triangle.v1_), triangle, triangle.vt1_, scene, world, projection);
	vertex_pixels[2] = VertexToPixel(glm::vec3(triangle.v2_), triangle, triangle.vt2_, scene, world, projection);

	std::vector<Pixel> left_pixels;
	std::vector<Pixel> right_pixels;
	ComputePolygonRows(vertex_pixels, left_pixels, right_pixels);
	DrawPolygonRows(triangle, left_pixels, right_pixels, scene, depth_buffer, shadow_map, frame_buffer);
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

// void DrawTriangleEdges(const Triangle& triangle, const Scene &scene)
// {
// 	// Transform each vertex from 3D world position to 2D image position
// 	auto p0 = VertexToPixel(glm::vec3(triangle.v0_), triangle, scene);
// 	auto p1 = VertexToPixel(glm::vec3(triangle.v1_), triangle, scene);
// 	auto p2 = VertexToPixel(glm::vec3(triangle.v2_), triangle, scene);

// 	vec3 color(1, 1, 1);
// 	DrawLineSDL(screen, p0.pos, p1.pos, color);
// 	DrawLineSDL(screen, p1.pos, p2.pos, color);
// 	DrawLineSDL(screen, p2.pos, p0.pos, color);
// }

void DrawPolygonRows(const Triangle &triangle, const std::vector<Pixel>& left_pixels, const std::vector<Pixel>& right_pixels, const Scene& scene, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer)
{
	for (int i = 0; i < left_pixels.size(); i++)
	{
		std::vector<Pixel> row;
		ComputeLine(left_pixels[i], right_pixels[i], row);
		for (auto pixel : row)
		{
			if (pixel.pos.x >= 0 && pixel.pos.x < PIXELS_X && pixel.pos.y >= 0 && pixel.pos.y < PIXELS_Y)
			{
				DrawPixel(pixel, triangle, scene, depth_buffer, shadow_map, frame_buffer);
			}
		}
	}
}

void DrawPixel(const Pixel& pixel, const Triangle &triangle, const Scene& scene, std::vector<float>& depth_buffer, std::vector<float>& shadow_map, std::vector<glm::vec3>& frame_buffer)
{
	if (pixel.z_inv > depth_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x])
	{
		depth_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x] = pixel.z_inv;

		//auto illumination = glm::vec3(0.0f);
		frame_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x].x = 0;
		frame_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x].y = 0;
		frame_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x].z = 0;
		for (auto light : scene.lights_)
		{
			// Compute the camera-space vector between the 3d position of the pixel and the light source
			// TODO: cache this while camera and light static
			vec3 camera_pixel_to_light = light.camera_position - pixel.camera_pos;

			vec3 direct_light = light.color * std::max(abs(glm::dot(glm::normalize(camera_pixel_to_light), glm::normalize(pixel.camera_normal))), 0.0f) / float(4.0f * M_PI * glm::dot(camera_pixel_to_light, camera_pixel_to_light));
			//std::cout << "x: " << pixel.vt.x << ", y: " << pixel.vt.y << "\n";
			frame_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x] += direct_light * triangle.GetDiffuseColour(pixel.vt) + scene.indirect_light_ * triangle.GetAmbientColour(pixel.vt);
		}
		//frame_buffer[pixel.pos.y * PIXELS_X + pixel.pos.x] = illumination;
	}
}

Pixel VertexToPixel(const glm::vec3& world_pos, const Triangle& triangle, const glm::vec2 vt, const Scene &scene, glm::mat4& world, glm::mat4& projection)
{
	glm::vec4 camera_vertex_position = world * glm::vec4(world_pos, 1.0f);

	glm::vec4 projected = projection * camera_vertex_position;
	//Clip here

	glm::vec3 screen = glm::vec3(projected/projected.w);

	return Pixel{
		glm::ivec2(
			PIXELS_X * ( screen.x + 1.0f ) / 2.0f,
			PIXELS_Y * ( screen.y + 1.0f) / 2.0f),
		camera_vertex_position.z == 0.0f ? std::numeric_limits<float>::max() : 1.0f / camera_vertex_position.z,
		glm::vec3(camera_vertex_position),
		triangle.normal_,
		vt
	};
}

// void Clip(std::vector<glm::vec4> vertices, std::vector<glm::vec4> &out_vertices)
// {
// 	for(int i = 0; i < 3; i++)
// 	{
// 		out_vertices[i] = vertices[i];
// 	}
// 	ClipPolygonOnWAxis(vertices, out_vertices);
// 	ClipPolygonForAxis(vertices, 0, out_vertices);
// 	ClipPolygonForAxis(vertices, 1, out_vertices);
// 	ClipPolygonForAxis(vertices, 2, out_vertices);
// 	//PolygonToTriangles(clipped_vertices, triangle, out_triangles);
// }


// void ClipPolygonOnWAxis(std::vector<glm::vec4>& vertices, std::vector<glm::vec4> &out)
// {
//     char previousInside;
//     char currentInside;
	
//     float a;
//     glm::vec4 intersectionPoint; 

//     glm::vec4 currentVertex;  
//     glm::vec4 previousVertex = vertices[2];

//     previousInside = previousVertex.w < W_CLIPPING_PLANE ? -1 : 1;

//     for(int i = 0; i < vertices.size(); i++ )
//     {
//     	currentVertex = vertices[i];
    	
//         currentInside  =  currentVertex.w <= W_CLIPPING_PLANE ? -1 : 1;

//         if (previousInside * currentInside < 0) // One end in and one out since -1 * 1 = -1
//     	{
//         	//Need to clip against plane w=0
			
//         	a = (W_CLIPPING_PLANE - previousVertex.w ) / (previousVertex.w - currentVertex.w);
			
//         	// I = Qp + f(Qc-Qp))
//         	glm::vec4 intersectionPoint(currentVertex);         
//         	intersectionPoint = previousVertex - intersectionPoint;    
//         	intersectionPoint = a * intersectionPoint;                  
//         	intersectionPoint = previousVertex + intersectionPoint;     
			
//         	// Insert
//         	out[i] = glm::vec4(intersectionPoint); 
//     	}
		
//     	if (currentInside > 0) //if current vertex inside 
//     	{
//         	//Insert
//         	out[i] = glm::vec4(currentVertex); //NEED TO COPY
//     	}
		
//     	previousInside = currentInside;
// 		previousVertex = currentVertex;
//     }
// }

// void ClipPolygonForAxis(std::vector<glm::vec4>& vertices, int AXIS, std::vector<glm::vec4> &clipped)
// {
//     glm::vec4 currentVertex;
//     glm::vec4 previousVertex;
		
//     char previousInside;
//     char currentInside;
	
//     float a;
//     glm::vec4 intersectionPoint;

//     //Clip against first plane
//     previousVertex = vertices[2];
//     previousInside = previousVertex[AXIS] <= previousVertex.w ? 1 : -1;
//     for ( int i = 0; i < vertices.size(); i++ ) 
//     {
//     	currentVertex = vertices[i];
//         currentInside = currentVertex[AXIS] <= currentVertex.w ? 1 : -1;
    			
//         if (previousInside * currentInside < 0)
//         {
//             // Intersection point P can be written as a combination of both the points
//             // P = (1-a)*P1 + a *P2
//             //and a = (w1 + x1) / ( (w1 + x1) - (w2 + x2) )
// 			// http://fabiensanglard.net/polygon_codec/clippingdocument/p245-blinn.pdf

//             a = (previousVertex.w - previousVertex[AXIS]) / 
//                 ((previousVertex.w - previousVertex[AXIS]) - (currentVertex.w - currentVertex[AXIS] ));
			
//             glm::vec4 intersectionPoint(currentVertex);
//             intersectionPoint = previousVertex - intersectionPoint;
//             intersectionPoint = a * intersectionPoint;
//             intersectionPoint = previousVertex + intersectionPoint;
			
//             // Insert
//             clipped[i] = glm::vec4(intersectionPoint);
//         }
		
//         if (currentInside > 0)
//         {
//             //Insert
//             clipped[i] = glm::vec4(currentVertex);
//         }
		
//         previousInside = currentInside;
//         //Move forward
//         previousVertex = currentVertex;
//     }

//     //Clip against opposite plane
//     previousVertex = vertices[2];
//     previousInside = previousVertex[AXIS] <= previousVertex.w ? 1 : -1;
//     currentVertex = vertices[0];
//     for ( int i = 0; i < vertices.size(); i++ ) 
//     {
//     	currentVertex = vertices[i];
//         currentInside = currentVertex[AXIS] <= currentVertex.w ? 1 : -1;
		
//         if (previousInside * currentInside < 0)
//         {
//             //Need to clip against plan w=0
			
//             a = (previousVertex.w + previousVertex[AXIS]) / 
//                 ( (previousVertex.w + previousVertex[AXIS]) - (currentVertex.w + currentVertex[AXIS]) );
			
// 			glm::vec4 intersectionPoint(currentVertex);
// 			intersectionPoint = previousVertex - intersectionPoint;
// 			intersectionPoint = a * intersectionPoint;
// 			intersectionPoint = previousVertex + intersectionPoint;
			
//             clipped[i] = glm::vec4(intersectionPoint);
//         }
		
//         if (currentInside > 0)
//         {
// 			clipped[i] = glm::vec4(currentVertex);
//         }
		
//         previousInside = currentInside;
		
//         //Move forward
//         previousVertex = currentVertex;
//     }        
// }

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
		scene.camera_.Translate(glm::vec3(0.0f, -dt * movement_speed, 0.0f ));
	}
	if (keystate[SDLK_DOWN]) {
		scene.camera_.Translate(glm::vec3(0.0f,  dt * movement_speed, 0.0f ));
	}
	if (keystate[SDLK_LEFT]) {
		scene.camera_.Translate(glm::vec3(-1.0f * dt * movement_speed, 0.0f, 0.0f ));
	}
	if (keystate[SDLK_RIGHT]) {
		scene.camera_.Translate(glm::vec3(dt * movement_speed, 0.0f, 0.0f ));
	}
	if (keystate[SDLK_LSHIFT]) {
		scene.camera_.Translate(glm::vec3(0.0f, 0.0f, -dt * movement_speed));
	}
	if (keystate[SDLK_SPACE]) {
		scene.camera_.Translate(glm::vec3(0.0f, 0.0f, dt * movement_speed));
	}


	if (keystate[SDLK_w]) {
		scene.lights_[light_selected].position += vec3(0.0f, 0.0f, movement_speed * dt);
	}
	if (keystate[SDLK_s]) {
		scene.lights_[light_selected].position += vec3(0.0f, 0.0f, -movement_speed * dt);
	}
	if (keystate[SDLK_a]) {
		scene.lights_[light_selected].position += vec3(movement_speed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_d]) {
		scene.lights_[light_selected].position += vec3(-movement_speed * dt, 0.0f, 0.0f);
	}
	if (keystate[SDLK_q]) {
		scene.lights_[light_selected].position += vec3(0.0f, movement_speed * dt, 0.0f);
	}
	if (keystate[SDLK_e]) {
		scene.lights_[light_selected].position += vec3(0.0f, -movement_speed * dt, 0.0f);
	}
	// if (keystate[SDLK_n]) {
	// 	if (scene.lights_.size() < 6) {
	// 		//AddLight(vec3(0.0f, 0.0f, 0.0f), 10.0f * vec3(1.0f, 1.0f, 1.0f), scene.lights_);
	// 		//light_selected = static_cast<Uint8>(scene.lights_.size()) - 1;
	// 	}
	// }

	if (keystate[SDLK_j])
	{
		scene.camera_.Rotate(360 - dt * rotate_speed, glm::vec3(1.0f, 0.0f, 0.0f) );
	}
	if (keystate[SDLK_l])
	{
		scene.camera_.Rotate(dt * rotate_speed, glm::vec3(1.0f, 0.0f, 0.0f) );
	}
	if (keystate[SDLK_i])
	{
		scene.camera_.Rotate(360 - dt * rotate_speed, glm::vec3(0.0f, 1.0f, 0.0f) );
	}
	if (keystate[SDLK_k])
	{
		scene.camera_.Rotate(dt * rotate_speed, glm::vec3(0.0f, 1.0f, 0.0f) );
	}
	if (keystate[SDLK_u])
	{
		scene.camera_.Rotate(360 - dt * rotate_speed, glm::vec3(0.0f, 0.0f, 1.0f) );
	}
	if (keystate[SDLK_o])
	{
		scene.camera_.Rotate(dt * rotate_speed, glm::vec3(0.0f, 0.0f, 1.0f) );
	}

	// if (keystate[SDLK_1] && scene.lights_.size() > 0) {
	// 	light_selected = 0;
	// }
	// if (keystate[SDLK_2] && scene.lights_.size() > 1) {
	// 	light_selected = 1;
	// }
	// if (keystate[SDLK_3] && scene.lights_.size() > 2) {
	// 	light_selected = 2;
	// }
	// if (keystate[SDLK_4] && scene.lights_.size() > 3) {
	// 	light_selected = 3;
	// }
	// if (keystate[SDLK_5] && scene.lights_.size() > 4) {
	// 	light_selected = 4;
	// }
	// if (keystate[SDLK_6] && scene.lights_.size() > 5) {
	// 	light_selected = 5;
	// }
}

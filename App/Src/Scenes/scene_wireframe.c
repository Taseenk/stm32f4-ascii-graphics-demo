/**
 ******************************************************************************
 * @file           : scene_wireframe.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_wireframe.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

/* Private Defines -----------------------------------------------------------*/

/* Private Variables ---------------------------------------------------------*/
typedef struct {
	float16_t x, y, z; // 3D coordinates of a point in space
} Vector3d_t;

typedef struct {
	uint8_t a; // Index of the first vertex
	uint8_t b; // Index of the Second vertex
} VertexIndexPair_t;

typedef struct {
	const Vector3d_t *vertices; // Pointer to the array of vertices
	const Edge_t *edges;        // Pointer to the array of edges
	uint8_t vertex_count;
	uint8_t edge_count;
} Wireframe_t;

static const Vector3d_t cube_vertices = {
    // Front Face
    {1.0f, 1.0f, 1.0f},   // Index 0: Top-Right-Front
    {-1.0f, 1.0f, 1.0f},  // Index 1: Top-Left-Front
    {-1.0f, -1.0f, 1.0f}, // Index 2: Bottom-Left-Front
    {1.0f, -1.0f, 1.0f},  // Index 3: Bottom-Right-Front

    // Back Face
    {1.0f, 1.0f, -1.0f},   // Index 4: Top-Right-Back
    {-1.0f, 1.0f, -1.0f},  // Index 5: Top-Left-Back
    {-1.0f, -1.0f, -1.0f}, // Index 6: Bottom-Left-Back
    {1.0f, -1.0f, -1.0f}   // Index 7: Bottom-Right-Back
};

static const Edge_t cube_edges = {
    // Front Face Outline
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 0},

    // Back Face Outline
    {4, 5},
    {5, 6},
    {6, 7},
    {7, 4},

    // Side Edges connecting Front to Back
    {0, 4}, // Top-Right
    {1, 5}, // Top-Left
    {2, 6}, // Bottom-Left
    {3, 7}  // Bottom-Right
};

const Wireframe_t cube_wireframe = {cube_vertices, cube_edges, 8, 12};

/* Private Function Prototypes -----------------------------------------------*/

/* Private Functions ---------------------------------------------------------*/

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void WireframeInit(void)
 * @brief
 */
void WireframeInit(void)
{
}

/**
 * @fn void WireframeRender(uint32_t scene_frame)
 * @brief
 * @param scene_frame The current frame index provided by the scene manager.
 */
void WireframeRender(uint32_t scene_frame)
{
}
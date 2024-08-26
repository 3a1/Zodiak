#pragma once
#include "pch.h"

/* 
	Attempt to create something like an sdk for cs 1.6
	but it turned out more like a wrapper
*/

uint32_t hw_dll;
uint32_t client_dll;
view_matrix_t view_matrix;
Vector2 screen_size;
int player_team;

float state_list[MAX_ENTITIES];
Vector2 entity_list[MAX_ENTITIES];

/* Third party functions */
bool w2s(Vector2 screen, Vector3 position, Vector2* out, view_matrix_t matrix);
void aim(Vector2 position, Vector2 screen, float SmoothAmount);
double distance(int x1, int y1, int x2, int y2);

void HandleKeyInputs();

bool get_hw_base();
bool get_client_base();

bool read_view_matrix();

void clear_entity_list();

bool entity_dead(int i);
bool entity_teammate(int i);
bool entity_screen(int i, BOX* box);

Vector2 get_closest_entity();

int get_player_team();

float get_entity_state(int i);
char* get_entity_model(int i);
Vector3 get_entity_origin(int i);
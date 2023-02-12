#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

typedef struct s_physics_obj
{
	CircleShape		circle;
	Vector2f		v; //Velocity
	Vector2f		a; //acceleration
	int				id;
	bool			valid = false;
	float			mass;
}	t_physics_obj;

typedef struct s_game
{
	t_physics_obj	physics_obj[1000];
	uint32_t		obj_count = 0;
	uint32_t		obj_max = 1000;
	Time			elapsed;
	bool			ballselected = false;
	uint32_t		ballindex;
}	t_game;
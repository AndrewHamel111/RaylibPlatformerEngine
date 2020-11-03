#include "environment.h"

/// STRUCT ENV_OBJECT

env_object::env_object(): rect{Rectangle{0,0,100,50}}, sides{true, false, false, false}, id{255}, color{DARKGRAY}
{}

env_object::env_object(Rectangle rect): rect{rect}, sides{true, false, false, false}, id{255}, color{DARKGRAY}
{}

void DrawEnvObject(const env_object o)
{
	if (o.id == 255)
	{
		DrawRectangleRec(o.rect, o.color);
	}
}

void DrawEnvList(const env_list l)
{
	auto i = l.begin(), iE = l.end();
	while (i != iE)
		DrawEnvObject(*(i++));
}

/// STRUCT ENV_LEVEL

env_level::env_level(): id{0}, label{"SAMPLE TEXT"}, env_objects{0}
{}

env_level::env_level(unsigned short id, std::string label, env_list env_objects): id{id}, label{label}, env_objects{env_objects}
{}
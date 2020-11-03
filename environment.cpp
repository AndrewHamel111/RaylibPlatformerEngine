#include "environment.h"

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

#include "files.h"

///////////////
/// FILE IO ///
///////////////

void SaveLevelToFile(env_level level, std::string fileName)
{
	json j = GetJSONOfLevel(level);
	std::ofstream os(fileName);
	// force pretty printing
	os << std::setw(4) << j;
	os.close();
}

void LoadLevelFromFile(env_level* level, std::string fileName)
{
	*level = LoadLevelFromFile(fileName);
}

env_level LoadLevelFromFile(std::string fileName)
{
	json j;

	std::ifstream is(fileName);
	is >> j;
	is.close();

	env_level level = GetLevelFromJSON(j);

	return level;
}

/////////////////
/// JSON Work ///
/////////////////

json GetJSONOfLevel(env_level level)
{
	json j;
	j["id"] = level.id;
	j["label"] = level.label;
	j["player_start"] = json::array({ level.player_start.x, level.player_start.y });
	j["objects"] = GetJSONOfEnvList(level.env_objects);

	return j;
	/// TODO add camera controller
}

/// Pointer overload for GetJSONOfLevel
void GetJSONOfLevel(env_level level, json* j)
{
	*j = GetJSONOfLevel(level);
}

env_level GetLevelFromJSON(json j)
{
	env_level level;
	level.id = j["id"];
	level.label = j["label"];
	level.player_start = Vector2{ j["player_start"][0], j["player_start"][1] };
	level.env_objects = GetEnvListFromJSON(j["objects"]);

	/// TODO add camera controller
	return level;
}

/// Pointer overload for GetLevelFromJSON
void GetLevelFromJSON(json j, env_level* level)
{
	*level = GetLevelFromJSON(j);
}

json GetJSONOfEnvList(env_list env)
{
	json j = json::array();
	std::cout << "env list size " << env.size();

	auto it = env.begin(), iEnd = env.end();
	while(it != iEnd)
	{
		json obj;

		obj["uuId"] = it->uuId;
		obj["spriteId"] = it->spriteId;
		obj["label"] = it->label;
		obj["rect"] = json::array({ it->rect.x, it->rect.y, it->rect.width, it->rect.height });
		obj["sides"] = json::array({ it->sides[0], it->sides[1], it->sides[2], it->sides[3] });
		obj["color"] = json::array({ it->color.r, it->color.g, it->color.b, it->color.a });
		obj["type"] = it->type;
		obj["func"] = it->func;

		j.push_back(obj);
		it++;
	}
	std::cout << " j size " << j.size() << std::endl;

	return j;
}

/// Pointer overload for GetJSONOfEnvList
void GetJSONOfEnvList(env_list env, json* j)
{
	*j = GetJSONOfEnvList(env);
}

env_list GetEnvListFromJSON(json j)
{
	env_list env;

	auto it = j.begin(), iEnd = j.end();
	while(it != iEnd)
	{
		env_object obj;

		obj.uuId = it->at("uuId");
		obj.spriteId = it->at("spriteId");
		obj.label = it->at("label");

		/// TODO Somehow this works.. huh..
		obj.rect = Rectangle{it->at("rect")[0], it->at("rect")[1], it->at("rect")[2], it->at("rect")[3]};

		obj.sides[0] = it->at("sides")[0];
		obj.sides[1] = it->at("sides")[1];
		obj.sides[2] = it->at("sides")[2];
		obj.sides[3] = it->at("sides")[3];

		obj.color = Color{it->at("color")[0], it->at("color")[1], it->at("color")[2], it->at("color")[3]};

		obj.type = it->at("type");
		obj.func = it->at("func");

		env.push_back(obj);
		it++;
	}

	return env;
}

void GetEnvListFromJSON(json j, env_list* env)
{
	*env = GetEnvListFromJSON(j);
}

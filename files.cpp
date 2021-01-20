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

	try
	{
		env_level level = GetLevelFromJSON(j);
		return level;
	}
	catch (JSONLevelException except)
	{
		// pass the exception on to the caller, since I handle it differently when called from LoadLevelsFromFolder or just from the main method.
		throw except;
	}
}

/**
 * @brief Retrieve a list of levels from a particular folder
 * @param folder A std::string path to the folder ending with a '/'.
 * @returns A std::vector<env_level> with the levels ordered by id.
 * @throw JSONLevelException in the event that one of the files loaded was not a valid json level
 **/
std::vector<env_level> LoadLevelsFromFolder(std::string folder)
{
	int count;
	char ** files = GetDirectoryFiles(folder.c_str(), &count);
	// files[0] and files[1] are the current and parent directories, so we start our search at files[2]

	if (count < 3)
		throw std::string("Error Code 01: No files found in folder \"levels\"!");

	std::vector<env_level> levels;
	for(int i = 2; i < count; i++)
	{
		try
		{
			levels.push_back(LoadLevelFromFile(folder + "/" + std::string(files[i])));
		}
		catch (JSONLevelException except)
		{
			throw except;
		}
	}

	// sort the vector using env_level.id
	std::sort(levels.begin(), levels.end());
	ClearDirectoryFiles();

	return levels;
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
	j["coins_required"] = level.coinsRequired;
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
	level.coinsRequired = j["coins_required"];
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
		obj["type"] = (int)it->type;
		obj["func"] = (int)it->func;

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

		obj.type = (ENV_OBJECT_TYPE)it->at("type");
		obj.func = (ENV_OBJECT_FUNCTION)it->at("func");

		env.push_back(obj);
		it++;
	}

	return env;
}

void GetEnvListFromJSON(json j, env_list* env)
{
	*env = GetEnvListFromJSON(j);
}

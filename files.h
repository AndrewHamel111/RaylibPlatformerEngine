/// All File IO logic is included here, as used by the editor and the game.

#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

// from https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

#include "environment.h"

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

// for convenience
using json = nlohmann::json;

void SaveLevelToFile(env_level, std::string);
void LoadLevelFromFile(env_level*, std::string);
env_level LoadLevelFromFile(std::string);

json GetJSONOfLevel(env_level);
void GetJSONOfLevel(env_level, json*);
env_level GetLevelFromJSON(json);
void GetLevelFromJSON(json, env_level*);

json GetJSONOfEnvList(env_list);
void GetJSONOfEnvList(env_list, json*);
env_list GetEnvListFromJSON(json);
void GetEnvListFromJSON(json, env_list*);

#endif // FILES_H_INCLUDED

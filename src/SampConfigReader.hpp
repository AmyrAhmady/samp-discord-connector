#pragma once

#include <string>
#include <vector>

#include "Singleton.hpp"

using std::string;
using std::vector;


class ServerConfigReader : public Singleton<ServerConfigReader>
{
	friend class Singleton<ServerConfigReader>;
private:
	ServerConfigReader();
	~ServerConfigReader() = default;

public:
	bool GetVar(string varname, string &dest);
	bool GetVarList(string varname, vector<string> &dest);
	bool GetGamemodeList(vector<string> &dest);

private:
	vector<string> m_FileContent;
};

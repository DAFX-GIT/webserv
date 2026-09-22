#include "../webserv.h"

bool compareLocationLength(const Location *a, const Location *b)
{
	return a->prefix.length() > b->prefix.length();
}

bool ft_start_with_tab(std::string line, int nb_tab)
{
	if (!line[0])
		return (0);
	for (int i = 0; i < 1 * nb_tab; i++)
	{
		if (line[i] == ' ')
		{
			for (int j = 0; j < 4; j++)
			{
				if (!line[j] || line[j] != ' ')
					return (0);
			}
		}
		else if (!line[i] || line[i] != '\t')
			return (0);
	}
	return (1);
}

bool ft_verif_line(std::string line, int nb_tab)
{
	if (!ft_start_with_tab(line, nb_tab))
		return (0);
	return (1);
}

void ft_parse_line(std::string line, std::map<std::string, std::string> &types, int nb_tab)
{
	if (nb_tab && !ft_verif_line(line, nb_tab))
		return;
	std::istringstream cut_line(line);
	std::string value;
	std::string word;
	int i = 0;

	while (cut_line >> word)
	{
		if (i == 0)
			value = word;
		else
			types[word] += value;
		i++;
	}
}

void ft_parse_error(std::string line, std::map<std::string, std::string> &error_map)
{
	std::istringstream cut_line(line);
	int i = 0;
	std::string value;
	std::string word;

	while (cut_line >> word)
	{
		if (i == 0)
			value = word;
		else
			error_map[word] = value;
		i++;
	}
}

bool ft_check_port(std::string port)
{
	if (port.empty())
		return (1);
	if (atoi(port.c_str()) < 0 || atoi(port.c_str()) > 65535)
		return (1);
	return (0);
}

bool ft_check_server_name(const std::string &name)
{
	if (name.empty())
		return (false);
	for (size_t i = 0; i < name.size(); i++)
	{
		char c = name[i];

		if (c == '_')
			return (false);
	}
	return (true);
}

bool ft_check_method(std::string method)
{
	if (!method.empty() && method != "GET" && method != "POST" && method != "DELETE")
		return (1);
	return (0);
}

int Config::parseConf(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	std::ifstream file2(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Webserv: Error: could not open config file " << filename << std::endl;
		return (1);
	}

	std::string line;
	ServerConfig *currentServer = NULL;
	string bin_path = getBinaryDirectory();
	conf._bin_path = bin_path;

	int braceCount = 0;
	int nb_line = 0;
	string first_brace = "";
	int first_brace_line = 0;
	while (braceCount >= 0 && getline(file2, line))
	{
		nb_line++;
		if (line[0] == '#')
			continue;
		stringstream skip(line);
		string token;
		while (skip >> token)
		{
			if (token == "{")
			{
				if (braceCount == 0)
				{
					first_brace_line = nb_line;
					first_brace = line;
				}
				braceCount++;
			}
			else if (token == "}")
				braceCount--;
		}
	}
	if (braceCount != 0)
	{
		if (braceCount < 0)
			std::cerr << "Webserv: line " << nb_line << ": " << line << ": invalid format" << endl;
		else
			std::cerr << "Webserv: line " << first_brace_line << ": " << first_brace << ": invalid format" << endl;
		return (1);
	}

	nb_line = 0;
	while (std::getline(file, line))
	{
		nb_line++;
		std::stringstream ss(line);
		std::string key, value, value2, value3;
		int serverNb = 0;
		ss >> key >> value >> value2 >> value3;
		if (key.empty() || key[0] == '#')
			continue;
		if (key == "max_clients")
		{
			_max_clients = std::atoi(value.c_str());
			continue;
		}
		else
			_max_clients = 64;
		if (_max_clients == 0)
			_max_clients = 64;
		
		if (key == "types_path")
		{
			if (value[0] == '.')
			{
				value.erase(0, 2);
				value = bin_path + value;
			}
			_types_path = value;
			if (!value3.empty())
			{
				cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
				return (1);
			}
		}
		else if (key == "server")
		{
			if (value == "{" && value2.empty())
			{
				ServerConfig blankServer;
				conf._servers.push_back(blankServer);
				currentServer = &conf._servers.back();
				continue;
			}
			else
			{
				cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
				return (1);
			}
		}
		else if (currentServer != NULL)
		{
			if (key == "port")
			{
				currentServer->_port = value.c_str();
				if (ft_check_port(currentServer->_port))
				{
					cerr << "Webserv: " << currentServer->_port << ": invalid or missing port" << endl;
					return (1);
				}
				if (!value2.empty())
				{
					cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
					return (1);
				}
			}
			else if (key == "server_name")
			{
				currentServer->_server_name = value;
				if (!ft_check_server_name(currentServer->_server_name))
				{
					cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
					return (1);
				}
				if (!value2.empty())
				{
					cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
					return (1);
				}
			}
			else if (key == "error_page")
			{
				std::string line;
				std::string line_value;
				line += value;
				line += " " + value2;
				line += " " + value3;
				while (ss >> line_value)
					line += " " + line_value;
				ft_parse_error(line, conf._error);
			}
			else if (key == "location")
			{
				Location *newLoc = new Location();

				if (!value3.empty())
				{
					cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
					return (1);
				}
				bool isDuplicate = false;
				for (size_t i = 0; i < currentServer->_locations.size(); i++)
				{
					if (conf._servers[serverNb]._locations[i]->prefix == value)
					{
						cerr << "Location: " << value << ": already configured. Skipping..." << endl;
						isDuplicate = true;
						break;
					}
				}
				if (isDuplicate)
				{
					delete newLoc;

					int braceCount = 1;
					while (braceCount > 0 && getline(file, line))
					{
						nb_line++;
						stringstream skip(line);
						string token;
						while (skip >> token)
						{
							if (token == "{")
								braceCount++;
							else if (token == "}")
								braceCount--;
						}
					}
					continue;
				}
				newLoc->prefix = value;
				while (getline(file, line))
				{
					nb_line++;
					stringstream ss2(line);
					string locKey, locValue, locValue2, locValue3;

					ss2 >> locKey >> locValue >> locValue2 >> locValue3;

					if (locKey == "}")
						break;
					if (locKey == "root")
					{
						if (locValue[0] == '.')
						{
							locValue.erase(0, 2);
							locValue = bin_path + locValue;
						}
						newLoc->root = locValue;
					}
					else if (locKey == "index")
					{
						newLoc->index = locValue;
						if (!locValue2.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
					}
					else if (locKey == "autoindex")
					{
						newLoc->autoindex = (locValue == "YES");
						if (!locValue2.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
					}
					else if (locKey == "upload_dir")
					{
						if (locValue[0] == '.')
						{
							locValue.erase(0, 2);
							locValue = bin_path + locValue;
						}
						newLoc->upload_dir = locValue;
						if (!locValue2.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
					}
					else if (locKey == "isCgi")
					{
						newLoc->isCgi = (locValue == "YES");
						if (!locValue2.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
					}
					else if (locKey == "allowed_methods")
					{
						if (ft_check_method(locValue) || ft_check_method(locValue2) || ft_check_method(locValue3))
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": unknown method" << endl;
							return (1);
						}
						newLoc->methods.insert(locValue);
						newLoc->methods.insert(locValue2);
						newLoc->methods.insert(locValue3);
						while (ss2 >> locValue)
						{
							if (ft_check_method(locValue))
							{
								cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": unknown method" << endl;
								return (1);
							}
							newLoc->methods.insert(locValue);
						}
					}
					else if (locKey == "return" && !newLoc->redirection)
					{
						newLoc->redirection = 1;
						newLoc->redirection_vec.push_back(locValue);
						newLoc->redirection_vec.push_back(locValue2);
						if (!locValue3.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
					}
					else if (locKey == "cgi_assign")
					{
						if (!locValue2.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
						while (getline(file, line))
						{
							nb_line++;
							stringstream ss3(line);
							std::string token;

							ss3 >> token;
							if (token.empty() || token[0] == '#')
								continue;
							if (token == "}")
								break;
							ft_parse_line(line, newLoc->cgi_interpreters, 2);
						}
					}
					else if (locKey == "isUploadable")
					{
						if (!locValue2.empty())
						{
							cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
							return (1);
						}
						if (locValue == "YES")
							newLoc->isUploadable = 1;
					}
				}
				currentServer->_locations.push_back(newLoc);
				std::sort(currentServer->_locations.begin(), currentServer->_locations.end(), compareLocationLength);
				currentServer->_nbofLocations++;
			}
			else if (key != "{" && key != "}")
			{
				cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
				return (1);
			}
			serverNb++;
		}
		else if (key != "{" && key != "}")
		{
			cerr << "Webserv: " << "line " << nb_line << ": " << line.substr(line.find_first_not_of("\t ")) << ": invalid format" << endl;
			return (1);
		}
	}
	return (0);
}

void Config::ft_parse_types(void)
{
	std::string line;
	std::string path = _types_path;
	std::ifstream read_input(path.c_str());
	while (getline(read_input, line))
		ft_parse_line(line, _types, 1);
}

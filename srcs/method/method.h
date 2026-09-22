#ifndef METHOD_H
# define METHOD_H
# include "../webserv.h"
#include "../error/error.hpp"
# include <string>
# include <dirent.h>
# include <sys/stat.h>

struct User
{
	int			id;
	std::string	username;
};

// ----- delete.cpp -----
void handleDelete(Client* curClient, Location* loc);

// ----- get.cpp -----
void handleGet(Client* curClient, Location* loc);

// ----- post.cpp -----
void	handleNativeUpload(Connection* connect, Client* curClient, Location *loc);
void	handleCookie(Client* curClient);
void	handleKillCookie(Client* curClient);

void	getEveryFile(std::string base_location, std::string location, std::vector<std::string> &files, std::string prefix);

#endif

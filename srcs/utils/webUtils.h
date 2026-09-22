#ifndef WEBUTILS_H
# define WEBUTILS_H
# include "../webserv.h"

std::string getAbsolutePath(const std::string& configPath);
std::string getBinaryDirectory();
std::string getMineType(const std::string& path);
bool isPathSafe(string target, Client* curClient, Location *loc);
int make_socket_non_blocking(int sockfd);

#endif

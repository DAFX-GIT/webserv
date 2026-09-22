#include "../webserv.h"
#include "../error/error.hpp"

std::string getAbsolutePath(const std::string& configPath) {
	char actualPath[PATH_MAX];

	char* ptr = realpath(configPath.c_str(), actualPath);
	if (ptr != NULL) {
		return std::string(actualPath);
	}
	return (configPath); 
}

std::string getBinaryDirectory() {
	char buffer[PATH_MAX];
		
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len == -1)
		return ("");
	buffer[len] = '\0';
	std::string binaryPath(buffer);
	size_t lastSlash = binaryPath.find_last_of("/");
	if (lastSlash != std::string::npos) {
		return (binaryPath.substr(0, lastSlash + 1)); 
	}
	return ("");
}

bool isPathSafe(string target, Client* curClient, Location *loc) {

	char absoluteBin[PATH_MAX];
	if (!realpath(conf._bin_path.c_str(), absoluteBin)) {
		getError(500, curClient);
		return false;
	}
	std::string sandboxStr(absoluteBin);
	char absoluteTarget[PATH_MAX];
	if (!realpath(target.c_str(), absoluteTarget)) {
		getError(404, curClient);
		return false;
	}
	std::string targetStr(absoluteTarget);
	if (targetStr.find(sandboxStr) != 0 || targetStr.find(loc->prefix) == string::npos) {
		std::cout << "[SECURITY WARNING] Blocked directory traversal attempt to: " << targetStr << std::endl;
		getError(403, curClient);
		return false;
	}
	return true;
}

int make_socket_non_blocking (int sockfd)
{
	int flags;
	int	s;

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl");
		return (-1);
	}
	flags |= O_NONBLOCK;
	s = fcntl(sockfd, F_SETFL, flags);
	if (s == -1)
	{
		perror("fcntl");
		return (-1);
	}
	return (0);
}

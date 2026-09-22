#include "method.h"
#include "../class/Response.hpp"
#include "../cgi/cgi.h"
#include "../utils/webUtils.h"

std::string getMineType(const std::string& path) {
	size_t idx = path.find_last_of('.');
	if (idx == std::string::npos)
		return ("application/octet-stream");

	std::string ext = path.substr(idx + 1);
	std::map<std::string, std::string>::iterator value = conf._types.find(ext);
	if (value == conf._types.end())
		return ("application/octet-stream");
	return (value->second);
}

void	handleRedirection(Client *curClient, Location *loc)
{
	Response	response;

	response.setProtocol(curClient->http.getProtocol());
	response.setStatus(atoi(loc->redirection_vec[0].c_str()));
	response.setStatus("Moved Permanently");
	response.addHeader("Location", loc->redirection_vec[1]);
	curClient->oBuffer.append(response.getResponse());
}

void process(Connection* connect, Client* curClient){
	std::string method = curClient->http.getMethod();
	std::string path   = curClient->http.getPath();
	string host = curClient->http.getHeader("Host");
	string port;
	Location* loc = NULL;

	if (host.empty()) {
		host = conf._servers[0]._server_name;
    }

	size_t colonPos = host.find(':');
    if (colonPos != std::string::npos) {
		port = host.substr(colonPos + 1);
        host = host.substr(0, colonPos);
    }

    ServerConfig servConf = conf._servers[0];
	for (size_t i = 0; i < conf._servers.size(); ++i) {
		if ((conf._servers[i]._server_name == host || "localhost" == host) && conf._servers[i]._port == port) {
			
			servConf = conf._servers[i];
			curClient->http.curServ = conf._servers[i];
			break;
		}
	}

	if (curClient->http.getProtocol() != "HTTP/1.1")
	{
		getError(505, curClient);
		return ;
	}
	for (size_t i = 0; i < servConf._locations.size(); i++) {
		if (path.find(servConf._locations[i]->prefix) == 0) {
			loc = servConf._locations[i];
			break;
		}
	}
	if (!loc) {
		std::cerr << "Error: No location matching path: " << path << std::endl;
		getError(404, curClient);
		return;
	}
	if (!loc) {
		std::cerr << "Error: No location matching path: " << path << std::endl;
		getError(404, curClient);
		return;
	}
	if (loc->redirection)
		handleRedirection(curClient, loc);

	
	if (loc->isCgi) {
		if(std::find(loc->methods.begin(), loc->methods.end(), method) == loc->methods.end()) {
			getError(405, curClient);
			return;
		}
		execCgi(connect, curClient, loc);
		return;
	}
	if (method == "GET") {
		handleGet(curClient, loc);
		return ;
	}

	else if (method == "POST") {
		std::string contentType;
		
		contentType = curClient->http.getHeader("Content-Type");
	
		if (path == "/cookie")
			return (handleCookie(curClient));
		if (path == "/killcookie")
			return (handleKillCookie(curClient));
		if (contentType.find("multipart/form-data") != std::string::npos)
		{
			if (loc->isUploadable)
				return (handleNativeUpload(connect, curClient, loc));
			else
			{
				getError(403, curClient);
				return ;
			}
		}
		return ;
	}
	else if (method == "DELETE") {
		if(loc->methods.find("DELETE") != loc->methods.end()) {
			handleDelete(curClient, loc);
		} else {
			getError(403, curClient);
		}
		return ;
	}
	else if (method == "HEAD" || method == "PUT" || method == "CONNECT" || method == "OPTIONS" || method == "TRACE" || method == "PATCH")
		getError(501, curClient);
	else
		getError(405, curClient);
}
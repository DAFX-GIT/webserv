#include "error.hpp"
#include "../class/Response.hpp"
#include "../method/method.h"

std::string generateDefaultErrorPage(int code, const std::string &message)
{
	std::stringstream ss;

	ss << "<html>\r\n";
	ss << "<head><title>" << code << " " << message << "</title></head>\r\n";
	ss << "<body>\r\n";
	ss << "<center><h1>" << code << " " << message << "</h1></center>\r\n";
	ss << "<hr><center>WebServ</center>\r\n";
	ss << "</body>\r\n";
	ss << "</html>\r\n";

	return ss.str();
}

std::string statusMessage(int err)
{
	if (err == 400)
		return ("Bad Resquest");
	else if (err == 403)
		return ("Forbidden");
	else if (err == 404)
		return ("Not Found");
	else if (err == 405)
		return ("Method Not Allowed");
	else if (err == 413)
		return ("Payload Too Large");
	else if (err == 500)
		return ("Internal Server Error");
	else if (err == 501)
		return ("Not Implemented");
	else if (err == 502)
		return ("Bad Gateway");
	else if (err == 503)
		return ("Service Unavailable");
	else if (err == 504)
		return ("Gateway Timeout");
	else if (err == 505)
		return ("HTTP Version Not Supported");
	else
		return ("Error");
}

void getError(int err, Client *curClient)
{
	std::string path;
	std::vector<std::string> files;
	std::string content;

	for (std::map<std::string, std::string>::iterator it = conf._error.begin(); it != conf._error.end(); it++)
	{
		if (atoi(it->first.c_str()) == err)
		{
			std::string loc;
			path = it->second;
			for (size_t i = 0; i < curClient->http.curServ._locations.size(); i++)
			{
				if (path.find(curClient->http.curServ._locations[i]->prefix) == 0)
				{
					loc = curClient->http.curServ._locations[i]->root;
					break;
				}
			}
			path = loc + it->second;
			break;
		}
	}
	if (path.empty())
	{
		getEveryFile("/tmp/WebServ/DefaultError/", "", files, "");
		for (size_t i = 0; i < files.size(); i++)
		{
			if (atoi(files[i].substr(0, files[i].size() - 5).c_str()) == err)
			{
				path = "/tmp/WebServ/DefaultError/" + files[i];
				break;
			}
		}
	}
	if (path.empty())
	{
		content = generateDefaultErrorPage(err, "Error");
		path = "default.html";
	}
	else
	{
		std::ifstream file(path.c_str(), std::ios::binary);
		if (!file.is_open())
		{
			curClient->oBuffer.append("HTTP/1.1 404 Not Found\r\n\r\n404 Not Found");
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		content = buffer.str();
	}
	Response response;
	response.setProtocol(curClient->http.getProtocol());
	response.setStatus(err);
	response.setStatus(statusMessage(err));
	response.addHeader("Content-Length", content.size());
	response.addHeader("Content-Type", getMineType(path));
	response.setBody(content);
	curClient->oBuffer.append(response.getResponse());
}

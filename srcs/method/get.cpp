#include "method.h"

void	getEveryFile(std::string base_location, std::string location, std::vector<std::string> &files, std::string prefix)
{
	DIR				*dir;
	struct dirent	*entry;
	struct stat		st;

	dir = opendir((base_location + "/" + location).c_str());
	if (!dir)
		return ;
	entry = readdir(dir);
	while (entry != NULL)
	{
		stat((base_location + "/" + location + "/" + entry->d_name).c_str(), &st);
		if (S_ISDIR(st.st_mode) && strncmp(entry->d_name, ".", 1))
			getEveryFile(base_location, location + "/" + entry->d_name, files, prefix);
		else if (strncmp(entry->d_name, ".", 1))
			files.push_back(location + prefix + entry->d_name);
		entry = readdir(dir);
	}
	closedir(dir);
}

void	handleAutoIndexation(Client* curClient, Location* loc)
{
	std::vector<std::string>	files;
	std::string					content;

	getEveryFile(loc->root + loc->prefix, "", files, loc->prefix);
	content += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>File Explorer</title>\n    <style>\n        * {\n            margin: 0;\n            padding: 0;\n            box-sizing: border-box;\n            font-family: \"Segoe UI\", sans-serif;\n        }\n        body {\n            min-height: 100vh;\n            background: linear-gradient(135deg, #0f172a, #1e293b);\n            display: flex;\n            justify-content: center;\n            align-items: center;\n            padding: 40px;\n        }\n        .container {\n            width: 100%;\n            max-width: 800px;\n            background: rgba(255, 255, 255, 0.05);\n            backdrop-filter: blur(10px);\n            border-radius: 20px;\n            padding: 30px;\n            box-shadow: 0 10px 40px rgba(0, 0, 0, 0.3);\n        }\n        h1 {\n            color: white;\n            text-align: center;\n            margin-bottom: 30px;\n            font-size: 2rem;\n        }\n        .file-list {\n            display: flex;\n            flex-direction: column;\n            gap: 12px;\n        }\n        .file {\n            display: flex;\n            align-items: center;\n            padding: 15px 20px;\n            background: rgba(255, 255, 255, 0.08);\n            border-radius: 12px;\n            text-decoration: none;\n            color: white;\n            transition: all 0.25s ease;\n        }\n        .file:hover {\n            transform: translateX(8px);\n            background: rgba(59, 130, 246, 0.25);\n        }\n        .icon {\n            margin-right: 15px;\n            font-size: 1.3rem;\n        }\n        .path {\n            word-break: break-all;\n        }\n        footer {\n            margin-top: 25px;\n            text-align: center;\n            color: #94a3b8;\n            font-size: 0.9rem;\n        }\n    </style>\n</head>\n<body>\n    <div class=\"container\">\n        <h1>📁 Available Files</h1>\n        <div class=\"file-list\">";
	for (size_t i = 0; i < files.size(); i++)
		content += "            <a class=\"file\" href=" + files[i] + ">\n                <span class=\"path\">" + files[i] + "</span>\n            </a>";
	content += "</div>\n    </div>\n</body>\n</html>";
	std::stringstream response;
	response << curClient->http.getProtocol() << " 200 OK\r\n";
	response << "Content-Type: " << "text/html" << "\r\n";
	response << "Content-Length: " << content.size() << "\r\n";
	response << "\r\n";
	response << content;

	curClient->oBuffer.append(response.str());
}

void handleGet(Client* curClient, Location* loc)
{
	std::string path = curClient->http.getPath();

	if (path == loc->prefix && !loc->index.empty() && !loc->autoindex)
		path = "/" + loc->index;

	else if (path == loc->prefix && loc->autoindex)
	{
		handleAutoIndexation(curClient, loc);
		return ;
	}

	if (path.find("..") != std::string::npos) {
		getError(403, curClient);
		return ;
	}

	path = loc->root + path;
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		getError(404, curClient);
		return ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	std::stringstream response;
	response << curClient->http.getProtocol() << " 200 OK\r\n";
	response << "Content-Type: " << getMineType(path) << "\r\n";
	response << "Content-Length: " << content.size() << "\r\n";
	response << "\r\n";
	response << content;

	curClient->oBuffer.append(response.str());
}
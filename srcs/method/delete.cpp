#include "method.h"
#include "../utils/webUtils.h"

void handleDelete(Client* curClient, Location* loc)
{
	string relativePath = curClient->http.getPath();
	if (relativePath.find("..") != string::npos) {
		getError(403, curClient);
		return;
	}
	string fullPath = loc->root + relativePath;
	string fullSystemPath = getAbsolutePath(fullPath);
	cout << "[Delete Request] Attempting to erase file: " << fullSystemPath << endl;

	if (!isPathSafe(fullSystemPath, curClient, loc))
		return;

	if (std::remove(fullSystemPath.c_str()) != 0) {
		if (errno == ENOENT) {
			cout << "[Delete Error] File not found: " << fullSystemPath << endl;
		getError(404, curClient);
		} else {
			cout << "[Delete Error] Permission denied or system fault." << endl;
			getError(500, curClient);
		}
		return;
	}
	cout << "[Delete Success] File erased perfectly." << endl;
		
	stringstream response;
	response << curClient->http.getProtocol() << " 200 OK\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: 68\r\n";
	response << "\r\n";
	response << "<html><body><h1>200 OK</h1><p>File deleted cleanly!</p></body></html>";
		
	curClient->oBuffer.append(response.str());
}

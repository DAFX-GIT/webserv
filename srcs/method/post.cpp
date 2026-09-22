#include "method.h"
#include "../class/Response.hpp"
#include "../parsing/parsing.h"

void handleNativeUpload(Connection* connect, Client* curClient, Location *loc)
{
	std::string contentType = curClient->http.getHeader("Content-Type");
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos) {
		getError(400, curClient);
		return;
	}
	std::string boundary = "--" + contentType.substr(boundaryPos + 9);

		
	std::string body = curClient->http.getBody();
	size_t fileStart = body.find(boundary);
	if (fileStart == std::string::npos) {
		getError(400, curClient);
		return;
	}

	size_t filenamePos = body.find("filename=\"", fileStart);
	if (filenamePos == std::string::npos) {
		getError(400, curClient);
		return;
	}
	filenamePos += 10; 
	size_t filenameEnd = body.find("\"", filenamePos);
	std::string filename = body.substr(filenamePos, filenameEnd - filenamePos);

		
	size_t headerEnd = body.find("\r\n\r\n", filenameEnd);
	if (headerEnd == std::string::npos) {
		getError(400, curClient);
		return;
	}
	size_t dataStart = headerEnd + 4; 

		
	size_t dataEnd = body.find(boundary, dataStart);
	if (dataEnd == std::string::npos) {
		getError(400, curClient);
		return;
	}
	dataEnd -= 2; 

		
	std::string fileData = body.substr(dataStart, dataEnd - dataStart);
	std::string savePath = loc->upload_dir + filename;
	int out_fd = open(savePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out_fd == -1) {
		perror("open upload file");
		getError(500, curClient);
		return;
	}

	curClient->upload_buffer = fileData;
	curClient->bytes_sent = 0;
	curClient->upload_fd = out_fd;
	curClient->state = WAITING_UPLOAD;

	struct epoll_event ev_modify;
	ev_modify.events = EPOLLOUT ;
	ev_modify.data.ptr = connect;
	if (epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify) == -1) {
		perror("epoll_ctl file upload");
		close(out_fd);
		curClient->upload_fd = -1;
	}
}

/// @brief Create cookie
/// @param curClient information of current client
void	handleCookie(Client* curClient)
{
	std::stringstream ss;

	srand(time(NULL));
	ss << time(NULL) << rand() << rand();

	Response	response;
	response.setProtocol(curClient->http.getProtocol());
	response.setStatus(200);
	response.setStatus("OK");
	response.addHeader("Set-Cookie", "session_token=" + ss.str() +"; Path=/; HttpOnly");
	response.addHeader("Content-Length", "68");
	response.addHeader("Content-Type", "text/html");
	response.setBody("<html><body><h1>Cookie</h1><p>Cookie has been set!</p></body></html>");
	curClient->oBuffer.append(response.getResponse());
}

/// @brief Delete cookie
/// @param curClient information of current client
void	handleKillCookie(Client* curClient)
{
	Response	response;
	response.setProtocol(curClient->http.getProtocol());
	response.setStatus(200);
	response.setStatus("OK");
	response.addHeader("Set-Cookie", "session_token=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
	response.addHeader("Content-Length", "71");
	response.addHeader("Content-Type", "text/html");
	response.setBody("<html><body><h1>Cookie</h1><p>Cookie has been killed!</p></body></html>");
	curClient->oBuffer.append(response.getResponse());
}

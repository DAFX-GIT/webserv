#include "../webserv.h"
#include "../error/error.hpp"

void handleBadRequest(Connection* connect, Client* curClient) {
	curClient->oBuffer.clear();

	
	getError(400, curClient);

	curClient->state = WRITING_RESPONSE; 


	struct epoll_event ev_modify;
	ev_modify.events = EPOLLOUT ;
	ev_modify.data.ptr = connect; 


	if (epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify) == -1) {
		perror("epoll_ctl MOD inside handleBadRequest");
		
		epoll_ctl(conf.epollfd, EPOLL_CTL_DEL, connect->fd, NULL);
		conf._all_connections.erase(connect);
		delete connect;
	}
}

void HTTPRequest::parseChunk(Connection* connect, Client* curClient, std::string &iBuffer) {
	
	_rawBuffer.append(iBuffer);
	iBuffer.clear(); 

	while (_state != PARSE_COMPLETE) {
		if (_state == PARSE_REQUEST_LINE) {
			size_t pos = _rawBuffer.find("\r\n");
			int len = 2;
			if (pos == std::string::npos) {
				pos = _rawBuffer.find("\n");
				len = 1;
			}
			if (pos == std::string::npos)
				return; 

			std::string line = _rawBuffer.substr(0, pos);
			_rawBuffer.erase(0, pos + len);

			std::stringstream ss(line);
			ss >> _method >> _path >> _protocol;
			if (ss.fail() || !ss.eof()) {
				handleBadRequest(connect, curClient);
				_state = PARSE_ERROR;
				return; 
			}
			_state = PARSE_HEADERS;
		}

		else if (_state == PARSE_HEADERS) {
			while (1) {
				size_t pos = _rawBuffer.find("\r\n");
				int len = 2;
				if (pos == std::string::npos) {
					pos = _rawBuffer.find("\n");
					len = 1;
				}
				if (pos == std::string::npos)
					return; 

				std::string line = _rawBuffer.substr(0, pos);
				_rawBuffer.erase(0, pos + len);

				if (line.empty()) { 
					if (_headers.count("Content-Length")) {
						_bodyLen = std::atoi(_headers["Content-Length"].c_str());
						_state = PARSE_BODY;
					}
					else if ((_headers.count("Transfer-Encoding") && _headers["Transfer-Encoding"] == "chunked") ||
							(_headers.count("Transfer-Encoding:") && _headers["Transfer-Encoding:"] == "chunked")) {
						_state = PARSE_BODY;
					}
					else {
						_state = PARSE_COMPLETE;
					}
					break; 
				}
				else {
					size_t pos_colon = line.find(":");
					if (pos_colon == std::string::npos) continue;

					std::string key = line.substr(0, pos_colon);
					std::string value = line.substr(pos_colon + 1);
					
					size_t first = value.find_first_not_of(" \t");
					if (first != std::string::npos) {
						size_t last = value.find_last_not_of(" \t\r\n");
						value = value.substr(first, (last - first + 1));
					} else {
						value = "";
					}
					_headers[key] = value;
				}
			}
		}

		else if (_state == PARSE_BODY) {
			if (_headers["Transfer-Encoding"] == "chunked" || _headers["Transfer-Encoding:"] == "chunked") {
				while (1) { 
					size_t pos = _rawBuffer.find("\r\n");
					int len = 2;
					if (pos == std::string::npos) {
						pos = _rawBuffer.find("\n");
						len = 1;
					}
					if (pos == std::string::npos) {
						return; 
					}
					
					std::string hexStr = _rawBuffer.substr(0, pos);
					std::stringstream ss;
					size_t chunkSize;
					
					ss << std::hex << hexStr;
					ss >> chunkSize;
					
					size_t totalNeeded = pos + (len * 2) + chunkSize;
					
					if (_rawBuffer.size() < totalNeeded) {
						return; 
					}
					
					if (chunkSize == 0) {
						_state = PARSE_COMPLETE;
						_rawBuffer.clear(); 
						break;
					}
					
					std::string cleanChunk = _rawBuffer.substr(pos + len, chunkSize);
					_body.append(cleanChunk); 
					
					_rawBuffer.erase(0, totalNeeded);
				}
			}
			else {
				
				if (_rawBuffer.size() >= static_cast<size_t>(_bodyLen - _body.size())) {
					size_t needed = _bodyLen - _body.size();
					_body.append(_rawBuffer.substr(0, needed));
					_rawBuffer.erase(0, needed);
					_state = PARSE_COMPLETE;
				} else {
					_body.append(_rawBuffer);
					_rawBuffer.clear();
					return; 
				}
			}
		}
	}
}




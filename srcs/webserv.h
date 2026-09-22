#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <limits.h>
#include <vector>
#include <set>
#include <sys/wait.h>
#include <ctime>
#include <algorithm>
#include <csignal>
#include <ctime>

extern volatile sig_atomic_t server_running;

#define BACKLOG 10
#define CLIENT_TIMEOUT_SEC 60
#define CGI_TIMEOUT_SEC 5

using namespace std;

struct Client;
struct Connection;
struct Location;


enum ParseState {
	PARSE_REQUEST_LINE,
	PARSE_HEADERS,
	PARSE_BODY,
	PARSE_COMPLETE,
	PARSE_ERROR
};

struct ServerConfig {
	std::string							_server_name;
	std::string							_port;
	
	vector<Location*>					_locations;
	int									_nbofLocations;
};

class HTTPRequest {
private:
	ParseState _state;
	std::string _method;
	std::string _path;
	std::string _protocol;
	std::map<std::string, std::string> _headers;
	std::string _body;
	string _rawBuffer;
	int _bodyLen;
	
public:
	ServerConfig curServ;
	HTTPRequest() : _state(PARSE_REQUEST_LINE) {}
	void parseChunk(Connection* connect, Client* curClient, std::string &iBuffer);
	bool isComplete() const { return _state == PARSE_COMPLETE; }
		

	std::string getPath() const { return _path; }
	std::string getMethod() const { return _method; }
	std::string getProtocol() const { return _protocol; }
	string getBody() const { return _body; }
	string getHeader(string toFind) const {
		std::map<std::string, std::string>::const_iterator it = _headers.find(toFind);
		if (it == _headers.end()) {
			return "";
		}
		return it->second;
	}
};

enum ClientState {
	READING_HEADERS,
	READING_BODY,
	PROCESSING_RESPONSE,
	WAITING_CGI,
	WAITING_UPLOAD,
	WRITING_RESPONSE,
	DONE,
	ERROR
};

struct Client {
	int				client_fd;
	std::time_t 	last_activity;
	ClientState		state;
		
	HTTPRequest		http;
	std::string		iBuffer;
	std::string		oBuffer;
	size_t			bytes_sent;

	Connection*		cgi_connection;
	string			cgi_buffer;
	int				cgi_pid;
	int				cgi_fd;
	time_t			cgi_start_time;

	int				upload_fd;
	string			upload_buffer;
		
	Client(int fd) : client_fd(fd), state(READING_HEADERS), bytes_sent(0), cgi_connection(NULL), cgi_fd(-1), cgi_start_time(0), upload_fd(-1) {update_activity();}
	~Client() {if (upload_fd > 0) {close(upload_fd);}}

	void update_activity() {
		last_activity = std::time(NULL);
	}
};

enum cType {
	SERVER,
	CLIENT,
	CGI,
	FILE_WRITE
};

struct Connection {
	cType type;
	int fd;
	void* data;
	~Connection();
};

struct Location {
	std::string prefix;
	std::string root;
	std::set<std::string> methods;
	std::string index;
	bool isUploadable;
	std::string upload_dir;
	std::vector<std::string>	redirection_vec;
	bool	redirection;
	bool autoindex;
	bool isCgi;
	std::map<std::string, std::string> cgi_interpreters;

	Location() : isUploadable(false), redirection(false), autoindex(false), isCgi(false)  {};
};


struct Config {

	char** envp;
	int epollfd;

	string								_bin_path;
	string								_types_path;
	int									_max_clients;

	std::map<std::string, std::string>	_types;

    std::vector<ServerConfig>			_servers;
    std::set<Connection*>				_all_connections;

	std::map<std::string, std::string>	_error;

	int		parseConf (const std::string& filename);
	void	ft_parse_types(void);
};







extern Config conf;

void process(Connection* connect, Client* curClient);
int initServ(string port);
void serv(vector<int>& sockfds);

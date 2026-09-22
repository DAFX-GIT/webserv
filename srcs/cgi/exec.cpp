#include "cgi.h"
#include "../error/error.hpp"
#include "../class/Response.hpp"
#include "../utils/webUtils.h"

void ft_parse_line_header(std::string line, std::map<std::string, std::string> &maps)
{
	size_t pos = line.find(":");

	if (pos == std::string::npos)
		return;
	maps[line.substr(0, pos)] = line.substr(pos + 1);
}

void cgi_response(Client *curClient)
{
	size_t len = 4;
	size_t pos = curClient->cgi_buffer.find("\r\n\r\n");

	if (pos == std::string::npos)
	{
		pos = curClient->cgi_buffer.find("\n\n");
		len = 2;
	}

	int status = 200;
	std::string status_str = "OK";
	Response response;

	if (!curClient->cgi_buffer.empty())
	{
		std::string cgi_headers = curClient->cgi_buffer.substr(0, pos);
		std::string cgi_body = curClient->cgi_buffer.substr(pos + len);
		std::stringstream ss(cgi_headers);
		std::string line;
		std::map<std::string, std::string> cgi_headers_map;
		while (std::getline(ss, line))
			ft_parse_line_header(line, cgi_headers_map);
		for (std::map<std::string, std::string>::iterator it = cgi_headers_map.begin(); it != cgi_headers_map.end(); it++)
		{
			if (it->first == "Status" || it->first == "status")
			{
				status = atoi(it->second.c_str());
				if (it->second.size() > 4)
					status_str = it->second.substr(4);
			}
			else
				response.addHeader(it->first, it->second);
		}
		response.setBody(cgi_body);
	}
	response.setProtocol(curClient->http.getProtocol());
	response.setStatus(status);
	response.setStatus(status_str);
	curClient->oBuffer.append(response.getResponse());
}

void execCgi(Connection *connect, Client *curClient, Location *loc)
{
	string ext = "";			 //.py
	string cgiRelativePath = ""; // /cgi/user.py
	string path_info_val = "";	 // /bin/test
	string path_translated = "";
	string query = "";

	string httpPath = curClient->http.getPath();
	map<string, string>::iterator it;
	for (it = loc->cgi_interpreters.begin(); it != loc->cgi_interpreters.end(); ++it)
	{
		string defined_ext = it->first;
		size_t pos = httpPath.find(defined_ext);
		if (pos != std::string::npos)
		{
			ext = defined_ext;
			size_t cgiRelativePath_end = pos + ext.length();

			cgiRelativePath = httpPath.substr(0, cgiRelativePath_end);
			if (cgiRelativePath_end < httpPath.size())
			{
				path_info_val = httpPath.substr(cgiRelativePath_end);
			}
			break;
		}
	}
	if (ext.empty())
	{
		getError(404, curClient);
		return;
	}

	string bin = loc->cgi_interpreters[ext];
	string file_path = loc->root + cgiRelativePath;

	size_t question_mark_pos = path_info_val.find('?');

	if (question_mark_pos != std::string::npos)
	{
		query = path_info_val.substr(question_mark_pos + 1);
		path_info_val = path_info_val.substr(0, question_mark_pos);
	}

	path_translated = getAbsolutePath(path_info_val);

	std::ifstream file(file_path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		getError(404, curClient);
		return;
	}

	std::string cookie;

	cookie = curClient->http.getHeader("Cookie");

	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("GATEWAY_INTERFACE", "HTTP/1.1", 1);
	setenv("SERVER_SOFTWARE", "webserv", 1);
	setenv("REQUEST_METHOD", curClient->http.getMethod().c_str(), 1);
	setenv("REDIRECT_STATUS", "200", 1);
	setenv("QUERY_STRING", query.c_str(), 1);
	if (ext != ".foo")
	{
		setenv("PATH_INFO", path_info_val.c_str(), 1);
		setenv("SCRIPT_NAME", cgiRelativePath.c_str(), 1);
		setenv("SCRIPT_FILENAME", file_path.c_str(), 1);
		setenv("PATH_TRANSLATED", path_translated.c_str(), 1);
		setenv("BIN_PATH", conf._bin_path.c_str(), 1);
		if (!cookie.empty())
			setenv("HTTP_COOKIE", cookie.c_str(), 1);
	}
	else
	{
		setenv("PATH_INFO", file_path.c_str(), 1);
	}
	cout << file_path << endl;
	// for cgi tester only set PATH INFO with absolute path of exec

	int in_pipe[2];
	int out_pipe[2];

	if (pipe(in_pipe) < 0 || pipe(out_pipe) < 0)
	{
		perror("CGI Pipe allocation failed");
		return;
	}

	int pid = fork();
	if (pid < 0)
	{
		close(in_pipe[1]);
		close(in_pipe[0]);
		close(out_pipe[0]);
		close(out_pipe[1]);
		curClient->oBuffer.clear();
		curClient->oBuffer.append("HTTP/1.1 500 Internal Server Error\r\n"
								  "Content-Length: 25\r\n"
								  "Connection: close\r\n\r\n"
								  "500 Internal Server Error");

		curClient->state = WRITING_RESPONSE;

		struct epoll_event ev_modify;
		ev_modify.events = EPOLLOUT;
		ev_modify.data.ptr = connect;
		epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);

		return;
	}
	if (pid == 0)
	{
		dup2(in_pipe[0], STDIN_FILENO);
		dup2(out_pipe[1], STDOUT_FILENO);

		dup2(STDERR_FILENO, STDERR_FILENO);

		close(in_pipe[1]);
		close(in_pipe[0]);
		close(out_pipe[0]);
		close(out_pipe[1]);

		if (curClient->http.getMethod() == "POST")
		{
			std::string body = curClient->http.getBody();
			std::stringstream ss;
			ss << body.size();
			setenv("CONTENT_LENGTH", ss.str().c_str(), 1);
			setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
		}

		char *args[3];
		args[0] = const_cast<char *>(bin.c_str());
		args[1] = const_cast<char *>(file_path.c_str());
		args[2] = NULL;

		extern char **environ;
		execve(args[0], args, environ);
		perror("execve");
		exit(1);
	}
	curClient->cgi_start_time = time(NULL);

	close(in_pipe[0]);
	close(out_pipe[1]);

	curClient->cgi_pid = pid;
	curClient->cgi_fd = out_pipe[0];

	if (curClient->http.getMethod() == "POST")
	{
		string body = curClient->http.getBody();
		write(in_pipe[1], body.c_str(), body.size());
	}
	close(in_pipe[1]);

	fcntl(curClient->cgi_fd, F_SETFL, O_NONBLOCK);
	Connection *cgi_connect = new Connection();
	conf._all_connections.insert(cgi_connect);
	cgi_connect->type = CGI;
	cgi_connect->fd = curClient->cgi_fd;
	cgi_connect->data = connect;

	curClient->cgi_connection = cgi_connect;

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = cgi_connect;
	epoll_ctl(conf.epollfd, EPOLL_CTL_ADD, curClient->cgi_fd, &ev);
	curClient->state = WAITING_CGI;
}

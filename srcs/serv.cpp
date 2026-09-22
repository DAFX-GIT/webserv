#include "webserv.h"
#include "parsing/parsing.h"
#include "error/error.hpp"
#include "cgi/cgi.h"
#include "utils/webUtils.h"

void shutdownServ()
{
	std::set<Connection *>::iterator it = conf._all_connections.begin();
	while (it != conf._all_connections.end())
	{
		Connection *conn = *it;

		delete conn;

		conf._all_connections.erase(it++);
	}
	for (size_t j = 0; j < conf._servers.size(); j++)
	{
		for (size_t i = 0; i < conf._servers[j]._locations.size(); ++i)
		{
			delete conf._servers[j]._locations[i];
		}
		conf._servers[j]._locations.clear();
	}
}

void registerListeningSockets(vector<int> &sockfds)
{
	struct epoll_event ev_register;

	while (sockfds.size())
	{
		int sockfd = sockfds.back();
		sockfds.pop_back();
		Connection *server_connec = new Connection();
		server_connec->type = SERVER;
		server_connec->fd = sockfd;
		server_connec->data = NULL;

		conf._all_connections.insert(server_connec);

		ev_register.data.ptr = server_connec;
		ev_register.events = EPOLLIN;

		if (epoll_ctl(conf.epollfd, EPOLL_CTL_ADD, sockfd, &ev_register) == -1)
		{
			perror("epoll_ctl");
			exit(1);
		}
	}
}

void reapZombies()
{
	int status;
	pid_t reaped_pid;
	while ((reaped_pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
	}
}

void handleConnectionError(Connection *connect)
{
	if (connect->type == SERVER)
	{
		std::cerr << "Webserv: Error: Server listening socket error" << std::endl;
		exit(1);
	}
	else
	{
		std::cerr << "Webserv: Error: Client connection dropped or errored out" << std::endl;
		conf._all_connections.erase(connect);
		delete connect;
	}
}

void handleListenerReadable(Connection *connect)
{
	struct epoll_event ev_register;
	struct sockaddr in_addr;
	socklen_t addrLen = sizeof(in_addr);
	char ipAddr[NI_MAXHOST], port[NI_MAXSERV];

	int infd = accept(connect->fd, &in_addr, &addrLen);
	if (infd == -1)
	{
		return;
	}

	if (getnameinfo(&in_addr, addrLen, ipAddr, sizeof ipAddr, port, sizeof port, NI_NUMERICHOST | NI_NUMERICSERV) == -1)
		exit(1);
	printf("Accepted connection on descriptor %d "
		   "(host=%s, port=%s)\n",
		   infd, ipAddr, port);
	if (make_socket_non_blocking(infd) == -1)
		exit(1);

	Client *new_client = new Client(infd);

	Connection *client_connect = new Connection();
	client_connect->type = CLIENT;
	client_connect->fd = infd;
	client_connect->data = new_client;

	conf._all_connections.insert(client_connect);
	ev_register.events = EPOLLIN;
	ev_register.data.ptr = client_connect;
	if (epoll_ctl(conf.epollfd, EPOLL_CTL_ADD, infd, &ev_register) == -1)
	{
		perror("epoll_ctl");
		exit(1);
	}
}

void handleCgiEvent(Connection *connect, uint32_t events)
{
	Connection *client_connect = static_cast<Connection *>(connect->data);
	Client *curClient = static_cast<Client *>(client_connect->data);

	if (events & EPOLLERR)
	{

		getError(502, curClient);
		struct epoll_event ev_modify;
		ev_modify.events = EPOLLOUT;
		ev_modify.data.ptr = client_connect;
		epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);
		curClient->state = WRITING_RESPONSE;
		conf._all_connections.erase(connect);
		delete connect;
		return;
	}

	bool pipe_closed = false;

	if (events & EPOLLIN)
	{
		char buf[512];

		ssize_t bytes = read(connect->fd, buf, sizeof(buf));
		if (curClient->cgi_buffer.size() > 1000000)
		{
			getError(500, curClient);
			std::cerr << "Webserv: Error: CGI script send over 1mb." << std::endl;
			curClient->cgi_buffer.clear();
			pipe_closed = true;

		}
		if (bytes > 0)
		{
			curClient->update_activity();
			curClient->cgi_buffer.append(buf, bytes);
		}
		else if (bytes == -1 || bytes == 0)
			pipe_closed = true;
	}

	if (pipe_closed || (events & EPOLLHUP))
	{

		conf._all_connections.erase(connect);
		curClient->cgi_connection = NULL;
		curClient->cgi_fd = -1;

		delete connect;

		int status;
		pid_t res = waitpid(curClient->cgi_pid, &status, WNOHANG);
		if (res == 0)
		{
			kill(curClient->cgi_pid, SIGKILL);
			waitpid(curClient->cgi_pid, &status, 0);
		}
		if (curClient->oBuffer.empty()) {
			if (curClient->cgi_buffer.empty()) {
				std::cerr << "Webserv: Error: CGI script returned 0 bytes." << std::endl;
				getError(502, curClient);
			}
			else if (curClient->cgi_buffer.find("\r\n\r\n") == std::string::npos &&
					curClient->cgi_buffer.find("\n\n") == std::string::npos) {
				std::cerr << "Webserv: Error: CGI returned malformed headers." << std::endl;
				curClient->cgi_buffer.clear();
				getError(502, curClient);
			}
			else {
				cgi_response(curClient);
			}
		}

		struct epoll_event ev_modify;
		ev_modify.events = EPOLLOUT;
		ev_modify.data.ptr = client_connect;
		epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);
		curClient->state = WRITING_RESPONSE;
	}
}

void handleClientReadable(Connection *connect)
{
	int done = 0;

	Client *curClient = static_cast<Client *>(connect->data);

	ssize_t count;
	char buf[512];

	if (curClient->state == WAITING_CGI || curClient->state == WAITING_UPLOAD)
	{
		return;
	}
	count = read(curClient->client_fd, buf, sizeof buf);
	if (count > 0)
	{
		curClient->update_activity();
		curClient->iBuffer.append(buf, count);

		if (curClient->iBuffer.size() > 10000000) {
			getError(413, curClient);
			curClient->state = WRITING_RESPONSE;
			struct epoll_event ev_modify;
			ev_modify.events = EPOLLOUT;
			ev_modify.data.ptr = connect;
			epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);
			return;
		}

		curClient->http.parseChunk(connect, curClient, curClient->iBuffer);
	}
	else if (count == 0)
	{
		if (!curClient->http.isComplete())
		{
			done = 1;
		}
	}
	else if (count == -1)
	{
		done = 1;
	}
	if (curClient->http.isComplete() && !done && (curClient->state == READING_HEADERS || curClient->state == READING_BODY))
	{
		curClient->state = PROCESSING_RESPONSE;
		process(connect, curClient);
		if (curClient->state != WAITING_CGI && curClient->state != WAITING_UPLOAD)
		{
			struct epoll_event ev_modify;
			ev_modify.events = EPOLLOUT;
			ev_modify.data.ptr = connect;
			if (epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify) == -1)
			{
				perror("epoll_ctl MOD to EPOLLOUT");
			}
			else
			{
				curClient->state = WRITING_RESPONSE;
			}
		}
	}

	if (done)
	{
		conf._all_connections.erase(connect);
		std::cout << "Closed connection on descriptor " << connect->fd << std::endl;
		delete connect;
	}
}

void handleUploadWritable(Client *curClient, Connection *connect)
{
	size_t chunk_size = 65536;
	size_t remaining = curClient->upload_buffer.size() - curClient->bytes_sent;
	if (chunk_size > remaining)
		chunk_size = remaining;

	const char *data_ptr = curClient->upload_buffer.c_str() + curClient->bytes_sent;
	ssize_t w_bytes = write(curClient->upload_fd, data_ptr, chunk_size);

	if (w_bytes > 0)
	{
		curClient->update_activity();
		curClient->bytes_sent += w_bytes;

		if (curClient->bytes_sent >= curClient->upload_buffer.size())
		{
			close(curClient->upload_fd);
			curClient->upload_fd = -1;
			std::cout << "Upload Success." << std::endl;

			std::stringstream response;
			response << curClient->http.getProtocol() << " 201 Created\r\n";
			response << "Content-Type: text/html\r\n";
			response << "Content-Length: 94\r\n\r\n";
			response << "<html><body><h1>201 Created</h1><p>File uploaded successfully!</p></body></html>";

			curClient->oBuffer = response.str();
			curClient->state = WRITING_RESPONSE;
			curClient->bytes_sent = 0;

			struct epoll_event ev_modify;
			ev_modify.events = EPOLLOUT;
			ev_modify.data.ptr = connect;
			epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);
		}
		else
		{
			struct epoll_event ev_modify;
			ev_modify.events = EPOLLOUT;
			ev_modify.data.ptr = connect;
			epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);
		}
	}
	else
	{
		perror("File write error");
		close(curClient->upload_fd);
		curClient->upload_fd = -1;

		getError(500, curClient);
		curClient->state = WRITING_RESPONSE;

		struct epoll_event ev_modify;
		ev_modify.events = EPOLLOUT;
		ev_modify.data.ptr = connect;
		epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, curClient->client_fd, &ev_modify);
	}
}

void handleResponseWritable(Client *curClient, Connection *connect)
{
	ssize_t bytes_sent = write(curClient->client_fd, curClient->oBuffer.c_str(), curClient->oBuffer.size());

	if (bytes_sent == -1)
	{
		perror("write");
		conf._all_connections.erase(connect);
		delete connect;
	}
	else
	{
		curClient->update_activity();
		curClient->oBuffer.erase(0, bytes_sent);
		if (curClient->oBuffer.empty())
		{
			curClient->state = DONE;
			cout << "Finished sending response to descriptor " << curClient->client_fd << endl;
			conf._all_connections.erase(connect);
			delete connect;
		}
	}
}

void handleClientWritable(Connection *connect)
{
	Client *curClient = static_cast<Client *>(connect->data);

	if (curClient->state == WAITING_UPLOAD)
	{
		handleUploadWritable(curClient, connect);
		return;
	}

	handleResponseWritable(curClient, connect);
}

void dispatchEvent(struct epoll_event &ev)
{
	Connection *connect = static_cast<Connection *>(ev.data.ptr);
	if (!connect)
		return;

	if (connect->type == CGI)
	{
		handleCgiEvent(connect, ev.events);
		return;
	}

	if ((ev.events & EPOLLERR) || (ev.events & EPOLLHUP))
		handleConnectionError(connect);
	else if (connect->type == SERVER)
		handleListenerReadable(connect);
	else if (ev.events & EPOLLIN)
		handleClientReadable(connect);
	else if (ev.events & EPOLLOUT)
		handleClientWritable(connect);
}

void sweepTimeouts()
{
	std::time_t now = std::time(NULL);

	std::set<Connection *>::iterator it = conf._all_connections.begin();
	while (it != conf._all_connections.end())
	{
		Connection *conn = *it;

		if (!conn || conn->type == SERVER)
		{
			++it;
			continue;
		}

		if (conn->type == CLIENT)
		{
			Client *client = static_cast<Client *>(conn->data);

			if (client->state != WAITING_CGI && (now - client->last_activity > CLIENT_TIMEOUT_SEC))
			{
				std::cout << "[Timeout] Client on fd " << conn->fd << " expired due to inactivity.\n";
				conf._all_connections.erase(it++);
				delete conn;
				continue;
			}
		}

		if (conn->type == CGI)
		{
			Connection *client_conn = static_cast<Connection *>(conn->data);
			Client *client = static_cast<Client *>(client_conn->data);

			if (now - client->cgi_start_time > CGI_TIMEOUT_SEC)
			{
				std::cout << "[Timeout] CGI script on fd " << conn->fd << " took too long. Killing pid " << client->cgi_pid << "\n";
				if (client->cgi_pid > 0)
					kill(client->cgi_pid, SIGKILL);
				waitpid(client->cgi_pid, NULL, WNOHANG);
				client->oBuffer.clear();
				getError(504, client);
				client->state = WRITING_RESPONSE;
				struct epoll_event ev_modify;
				ev_modify.events = EPOLLOUT;
				ev_modify.data.ptr = client_conn;
				epoll_ctl(conf.epollfd, EPOLL_CTL_MOD, client->client_fd, &ev_modify);
				it++;
				continue;
			}
		}
		++it;
	}
}

void serv(vector<int> &sockfds)
{
	int epollfd = epoll_create1(0);
	if (epollfd == -1)
	{
		perror("epoll_create");
		exit(1);
	}
	conf.epollfd = epollfd;

	registerListeningSockets(sockfds);

	struct epoll_event *triggered_events = (epoll_event *)calloc(conf._max_clients, sizeof(struct epoll_event));
	cout << "Server running !" << std::endl;
	while (server_running)
	{
		reapZombies();
		int num_ready = epoll_wait(epollfd, triggered_events, conf._max_clients, 1000);
		for (int i = 0; i < num_ready; i++)
			dispatchEvent(triggered_events[i]);
		sweepTimeouts();
	}
	cout << "\nExiting..." << endl;
	shutdownServ();
	free(triggered_events);

	if (epollfd > 0)
	{
		close(epollfd);
		epollfd = -1;
	}
	cout << "Bye!" << endl;
}

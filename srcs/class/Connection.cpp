#include "../webserv.h"

Connection::~Connection() {
	if (fd > 0) {
		epoll_ctl(conf.epollfd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		fd = -1;
	}
	if ( this->type == CLIENT && this->data != NULL) {
		Client* client = static_cast<Client*>(this->data);

		if (client->cgi_connection != NULL) {
			Connection* cgi = client->cgi_connection;
			client->cgi_connection = NULL;
			conf._all_connections.erase(cgi);
			delete cgi;
		}
		delete client;
	}
}
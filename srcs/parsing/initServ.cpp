#include "../webserv.h"
#include "../utils/webUtils.h"

int initServ(string port) {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int sockfd;
	int yes = 1;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
		std::cerr << "Webserv: gai error: " << gai_strerror(status) << std::endl;
		exit(1);
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		sockfd = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1)
			continue ;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		status = bind (sockfd, p->ai_addr, p->ai_addrlen);
		if (status == 0)
			break ;

		close (sockfd);
	}

	freeaddrinfo (servinfo);

	if (p == NULL) {
		std::cerr << "Webserv: Error: could not bind" << std::endl;
		return (-1);
	}
	if (make_socket_non_blocking(sockfd) == -1) {
		close(sockfd);
		return (-1);
	}
	if (listen(sockfd, SOMAXCONN) == -1) {
		perror("listen");
		close(sockfd);
		return (-1);
	}

	return (sockfd);
}

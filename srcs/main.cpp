#include "webserv.h"

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

volatile sig_atomic_t server_running = 1;

Config conf;

void handle_sigint(int signum)
{
	(void)signum;
	server_running = 0;
}

int main(int ac, char **av, char **envp)
{
	std::signal(SIGINT, handle_sigint);
	conf.envp = envp;
	if (ac > 1)
	{
		if (conf.parseConf(av[1]))
			return (1);
	}
	else
	{
		std::cerr << "Webserv: Error: need a configuration file" << endl;
		return (1);
	}
	conf.ft_parse_types();

	vector<int> sockfds;
	vector<string> opened_ports;
	for (size_t i = 0; i < conf._servers.size(); i++)
	{
		std::string current_port = conf._servers[i]._port;

		bool port_already_open = false;
		for (size_t j = 0; j < opened_ports.size(); j++)
		{
			if (opened_ports[j] == current_port)
			{
				port_already_open = true;
				std::cerr << "Webserv: Error: double port" << endl;
				for (size_t j = 0; j < conf._servers.size(); j++)
				{
					for (size_t i = 0; i < conf._servers[j]._locations.size(); ++i)
						delete conf._servers[j]._locations[i];
					conf._servers[j]._locations.clear();
				}
				return (1);
			}
		}
		if (!port_already_open)
		{
			int new_fd = initServ(current_port);
			if (new_fd != -1)
			{
				sockfds.push_back(new_fd);
				opened_ports.push_back(current_port);
			}
		}
	}
	if (sockfds.empty())
	{
		std::cerr << "Webserv: Error: no listening sockets could be established." << std::endl;
		return (1);
	}
	serv(sockfds);
}
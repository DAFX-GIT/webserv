#ifndef CGI_H
# define CGI_H
# include "../webserv.h"

void execCgi(Connection* connect, Client* curClient, Location* loc);
void cgi_response(Client *curClient);
void ft_parse_line_header(std::string line, std::map<std::string, std::string> &maps);

#endif

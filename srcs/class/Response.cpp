#include "Response.hpp"

int			Response::getStatus(void)
{
	return (_status);
}

std::string	Response::getStatus(void) const
{
	return (_status_msg);
}

std::string	Response::getHeader(std::string key)
{
	return (_header[key]);
}

std::string	Response::getBody(void)
{
	return (_body);
}
std::string Response::getProtocol(void)
{
	return (_protocol);
}

std::string Response::getResponse(void)
{
	std::stringstream	response;

	response << _protocol << " " << _status << " " << _status_msg << "\r\n";
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		response << it->first << ": " << it->second << "\n";
	response << "\r\n" << _body;
	return (response.str());
}

void		Response::setStatus(int status)
{
	_status = status;
}

void		Response::setStatus(std::string message)
{
	_status_msg = message;
}

void		Response::setHeader(std::map<std::string, std::string> header)
{
	_header = header;
}

void		Response::setBody(std::string content)
{
	_body = content;
}

void		Response::setProtocol(std::string protocol)
{
	_protocol = protocol;
}
void		Response::addHeader(std::string key, std::string value)
{
	_header[key] = value;
}

void		Response::addHeader(std::string key, size_t value)
{
	std::string			s;
	std::stringstream	out;
	out << value;
	s = out.str();
	_header[key] = s;
}

void		Response::addBody(std::string content)
{
	_body.append(content);
}

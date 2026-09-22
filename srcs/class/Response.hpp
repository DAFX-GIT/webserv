/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Response.hpp									   :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: etessoer <etessoer@student.42.fr>		  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2026/06/05 15:13:08 by etessoer		  #+#	#+#			 */
/*   Updated: 2026/06/05 21:13:20 by etessoer		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "../webserv.h"
# include <stdlib.h>

class Response
{
	private:
		int									_status;
		std::string							_status_msg;
		std::map<std::string, std::string>	_header;
		std::string							_body;
		std::string							_protocol;
	public:
		int			getStatus(void);
		std::string	getStatus(void) const;
		std::string	getHeader(std::string key);
		std::string	getBody(void);
		std::string getProtocol(void);
		std::string getResponse(void);
		void		setStatus(int status);
		void		setStatus(std::string message);
		void		setHeader(std::map<std::string, std::string> header);
		void		setBody(std::string content);
		void		setProtocol(std::string protocol);
		void		addHeader(std::string key, std::string value);
		void		addHeader(std::string key, size_t value);
		void		addBody(std::string content);
};

#endif

#ifndef CHATGPT_HPP
# define CHATGPT_HPP

# include "IRC.hpp"

class ChatGPT : public Client
{
	private:
		std::string _apikey;
		Server*		_server;
	public:
	ChatGPT();
	ChatGPT(const std::string& apikey, Server* server);
	std::string	getAPIkey() const { return _apikey; };
	void		setAPIkey(const std::string& apikey) { _apikey = apikey; };
   	int			parseBotCommand(std::string message, Client& sender);
    std::string getChatGPTResponse(std::string request);
    std::string trimResponse(std::string &response);
};

#endif

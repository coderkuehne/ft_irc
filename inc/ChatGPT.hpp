#ifndef CHATGPT_HPP
# define CHATGPT_HPP

# include "IRC.hpp"

class ChatGPT : public Client
{
	private:
		std::string& _apikey;
	public:
	ChatGPT();
	ChatGPT(std::string& apikey);
	std::string	getAPIkey() const { return _apikey; };
	void		setAPIkey(const std::string& apikey) { _apikey = apikey; };
   	void parseBotCommand(std::string message, Client *sender);
    std::string getChatGPTResponse(std::string request);
    std::string trimResponse(std::string &response);
};

#endif
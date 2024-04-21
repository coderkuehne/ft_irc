#include "IRC.hpp"
#include "Commands.hpp"
#include "Channel.hpp"

// int Channel::kickClient(const std::string channel, const std::string target, Client &client)
// {
//     (void)client;
//     std::cout << "Kicking " << target << "from " << channel << std::endl;
//     return (0);
// }

int	Channel::addOperator(Client client) {
	int total_clients = _clients.size() + _operators.size();
	int limit = getClientLimit();

	if (limit > 0 && total_clients > limit)
		return (1);
	_operators.push_back(client);
	(void)_server; //temp
	return (0);
}

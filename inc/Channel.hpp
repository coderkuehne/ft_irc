# include "Client.hpp"

class Channel
{
    private:
        std::string	_name;
        std::string	_topic;
        std::vector<Client>	_clients;

    public:
        Channel(std::string name): _name(name) {};
        ~Channel() {};

        std::string	getName() { return _name; };
        std::vector<Client>	getClients() { return _clients; };
        void	addClient(Client client) { _clients.push_back(client); };
        void	removeClient(Client client) { _clients.erase(std::find(_clients.begin(), _clients.end(), client));};
};

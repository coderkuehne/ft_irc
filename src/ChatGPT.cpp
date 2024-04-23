#include "ChatGPT.hpp"
ChatGPT::ChatGPT() : Client(0)
{

}

ChatGPT::ChatGPT(std::string& apikey) : Client(0)
{
	setAPIkey(apikey);
}

void ChatGPT::parseBotCommand(std::string message_string, Client* sender) {
    std::string response;
    if (getApiKey().empty())
        return sender->sendMessage(MSG_PRIVMSG_TEMPLATE(getPrefix(), sender->getNickname(), "I wasn't provided with api key, so im not going to pretend that I'm ChatGPT\r\n"));
    response = getChatGPTResponse(message_string);
    response = trimResponse(response);
    sender->sendMessage(MSG_PRIVMSG_TEMPLATE(getPrefix(), sender->getNickname(), response));
}

std::string ChatGPT::trimResponse(std::string& response) {
    size_t start, finish;
    start = response.find("\"content\": \"");
    finish = response.substr(start, response.size()).find("\"");
    return response.substr(start + 12, finish - start - 3);
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::string* data = static_cast<std::string*>(stream);
    size_t bytes = size * nmemb;
    data->append(static_cast<char*>(ptr), bytes);
    return bytes;
}

std::string ChatGPT::getChatGPTResponse(std::string request)
{
    std::string requestInFormat = "{\"model\": \"gpt-3.5-turbo\",\"messages\": [{\"role\": \"user\", \"content\": \"" + request + "\"}]}";
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Set up the request
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + getApiKey()).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestInFormat.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Send the request and receive the response
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    // Clean up libcurl
    curl_global_cleanup();
    return response;
}

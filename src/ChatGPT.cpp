#include "ChatGPT.hpp"

ChatGPT::ChatGPT() : Client(0)
{
    std::cout << "This shoulndt be called something went wrong" << std::endl;
}

ChatGPT::ChatGPT(const std::string& apikey, Server* server) : Client(0)
{
    _apikey = apikey;
    _server = server;
    beAuthenticated();
    beRegistered();
    setNickname("ChatGPT");
    setUsername("BOT");
}

// int ChatGPT::parseBotCommand(std::string message_string, Client& sender) {
//     std::string response;
//     if (getAPIkey().empty())
//         return (_server->sendToClient("I wasn't provided with api key, so im not going to pretend that I'm ChatGPT", sender));
//     response = getChatGPTResponse(message_string);
// 	std::cout << "What do we have here: " << message_string << std::endl;
//     response = trimResponse(response);
// //    _server->sendToClient(response, sender);
//     return (0);
// }

std::string ChatGPT::trimResponse(std::string& response) {
    size_t startPos = response.find("{");
    if (startPos == std::string::npos) {
        return "";
    }
    // Start position after "response": "
    startPos += 13; // Length of "\"response\": \""
    std::cout << "startPos: " << startPos << std::endl; 

    // Find the position of the last occurrence of "
    size_t endPos = response.find("\"}", startPos);
    std::cout << "endPos: " << endPos << std::endl; 

    // Extract the substring between startPos and endPos
    std::string trimmedResponse = response.substr(startPos, endPos - startPos);
    std::cout << "trimmedResponse: " << trimmedResponse << std::endl;

    return trimmedResponse;
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::string* data = static_cast<std::string*>(stream);
    size_t bytes = size * nmemb;
    data->append(static_cast<char*>(ptr), bytes);
    return bytes;
}

std::string ChatGPT::getChatGPTResponse(std::string message)
{
	if (!message.empty() && message[0] == ':')
    	message = message.substr(1, message.size() - 3);
	
	std::cout << "do we have something? " << message << std::endl;
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	std::string response;
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/message");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		std::string json_data = "{\"message\": \"" + message + "\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
		curl_slist_free_all(headers);
//		std::cout << "Respo: " << res << std::endl;
		curl_easy_cleanup(curl);
	}
//	std::cout << "Before trim: " << response << std::endl;
	response = trimResponse(response);
//	std::cout << "And that: " << response << std::endl;
	return response;
}

// std::string ChatGPT::getChatGPTResponse(std::string request)
// {
//     // std::string requestInFormat = "{\"model\": \"gpt-3.5-turbo\",\"messages\": [{\"role\": \"user\", \"content\": \"" + request + "\"}]}";
// 	// std::string requestInFormat = "{\"model\": \"tinyllama\",\"prompt\": \"" + request + "\"}";
// //	std::string requestInFormat = "{\"message\":\"" + request + "\"}";
//     // Initialize libcurl
//     curl_global_init(CURL_GLOBAL_ALL);
//     std::cout << "API: we in boiz: " << request << std::endl;
//     // Set up the request
//     CURL* curl = curl_easy_init();
//     std::string response;
// 	CURLcode res;
//     if (curl) {
//     	struct curl_slist* headers = NULL;
//         headers = curl_slist_append(headers, "Content-Type: application/json");
//         // headers = curl_slist_append(headers, ("Authorization: Bearer " + getAPIkey()).c_str());
//         // curl_easy_setopt(curl, CURLOPT_URL, "https://api.ollamallama.com/v2/chat/completions");
//         // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//         // curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestInFormat.c_str());
//         // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
//         // curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

// 		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
// 		curl_easy_setopt(curl, CURLOPT_URL, "http://10.12.8.5:3000/message");
// 		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
// 		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
//       	// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestInFormat.c_str());
// //        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

// 		const char *data = "{\n    \"message\":\"Hello, who are you?\"\n}";
// 		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
// 		res = curl_easy_perform(curl);
// 		// curl_slist_free_all(headers);

//         // Send the request and receive the response
// 		std::cout << "what is the" << res << std::endl;
// 		std::cout << "do we have something?" << res << std::endl;
// //        CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
//         }
// //        std::cout << "API: response: " << response << std::endl;
//         // Clean up
//         curl_slist_free_all(headers);
//         curl_easy_cleanup(curl);
//     }

//     // Clean up libcurl
//     curl_global_cleanup();
//     return response;
// }

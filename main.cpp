#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include "/Users/mzeroual/goinfre/downloads/json-develop/single_include/nlohmann/json.hpp"
#include <fstream>


using json = nlohmann::json;

// functon to handle response from the cURL request
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


std::string sendRequest(const std::string& url, const json& payload, const std::string& __unused accessToken = "") {
	CURL *curl;
	CURLcode __unused res;
	std::string readBuffer;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1L); // Set method to POST
		
		// Set the request payload
		std::string json__ = payload.dump();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json__.c_str());
		
		// Set headers, Add Athorizition if accessToken is provide
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		
		// Set up the write Callback to capture the response 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "Fatal error -> in 'curl_esay_perform' result not CURLE_OK" << std::endl;
		}

		// Free resources
    	curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	
	}
	return readBuffer;
}

const std::string getAccessToken(void) {
	const std::string& url = "https://test.deribit.com/api/v2/public/auth";

	std::ifstream ifs("payload.json");
	json payload = json::parse(ifs);
	const auto& response = json::parse(sendRequest(url, payload));
	if (response.contains("result") && response["result"].contains("access_token") ) {
		return response["result"]["access_token"];
	} else {
		std::cerr << "Fatal error -> failed to retrive access token" << std::endl;
		return "";
	}
}

int main(void) {

	std::string access_token = getAccessToken();	

	
	std::cout << access_token << std::endl;
	return 0;
}

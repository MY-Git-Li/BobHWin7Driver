
#include "pch.h"
#include "MyHttpQuest.h"

#include <sstream>




size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	string data((const char*)ptr, (size_t)size * nmemb);

	*((stringstream*)stream) << data << endl;

	return size * nmemb;
}



rapidjson::Document json_get(const std::string &url)
{
	/*HTTP GET json data*/
	std::stringstream out;
	void* curl = curl_easy_init();
	// 设置URL
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	// 设置接收数据的处理函数和存放变量
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

	// 执行HTTP GET操作
	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}

	// 接受数据存放在out中，输出之
	//cout << out.str() << endl;
	string str_json = out.str();
	curl_easy_cleanup(curl);
	rapidjson::Document json;
	json.Parse(str_json.c_str());
	return json;
}

rapidjson::Document  json_post(const std::string &url, const string &params)
{
	std::stringstream out;
	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	CURL* curl = curl_easy_init();
	if (curl) {
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

		/* Perform the request, res will get the return code */
		const CURLcode res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	string str_json = out.str();
	//AfxMessageBox(to_wide_string(str_json).c_str());
	rapidjson::Document json;
	json.Parse(str_json.c_str());
	return json;
}

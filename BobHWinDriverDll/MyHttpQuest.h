#pragma once
#include <iostream>  
#include <string>  
#include "curl/curl.h"
#include "rapidjson/document.h"
using namespace std;


  rapidjson::Document  json_get(const std::string &url);
  rapidjson::Document  json_post(const std::string &url, const string &params);
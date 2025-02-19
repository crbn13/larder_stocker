// #pragma once
#ifndef CRBN_IMAGE_DOWNLOADER
#define CRBN_IMAGE_DOWNLOADER


// THIS FILE IS AI GENERATED
// but a lot of my own code has also been implemented


#include "apikeys.hpp" // store api keys as global strings

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../curl/curl.h"

#include "crbn_logging.hpp"
#include "crbn_json.hpp"
// #include "crbn_json.hpp"

// Write callback for libcurl
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userData);

// Function to perform a GET request
std::string httpGet(const std::string &url);

// Function to download a file from a URL
bool downloadFile(const std::string &url, const std::string &outputPath);

namespace crbn
{
    bool imageDownload(std::string query, const std::string & tagId);
}

#endif
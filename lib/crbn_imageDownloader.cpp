#include "headers/crbn_imageDownloader.hpp"

// Write callback for libcurl
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userData)
{
    size_t totalSize = size * nmemb;
    userData->append((char *)contents, totalSize);
    return totalSize;
}

// Function to perform a GET request
std::string httpGet(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    return response;
}

// Function to download a file from a URL
bool downloadFile(const std::string &url, const std::string &outputPath)
{
    CURL *curl;
    FILE *file;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        file = fopen(outputPath.c_str(), "wb");
        if (!file)
        {
            std::cerr << "Failed to open file for writing: " << outputPath << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        res = curl_easy_perform(curl);
        fclose(file);
        if (res != CURLE_OK)
        {
            std::cerr << "File download failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_cleanup(curl);
        return true;
    }

    return false;
}

namespace crbn
{

    bool imageDownload(std::string query, const std::string & tagId)
    {
        // std::string query = querey;

        // std::cout << "Enter search term: ";
        // std::getline(std::cin, query);

        // Replace spaces with "+" for the query string

        std::replace(query.begin(), query.end(), ' ', '+');

        // Construct the search API URL
        std::string apiUrl = "https://www.googleapis.com/customsearch/v1?q=" + query +
                             "&key=" + apiKey + "&cx=" + searchEngineId + "&searchType=image";

        // Perform the HTTP GET request
        std::string response = httpGet(apiUrl);
        if (response.empty())
        {
            std::cerr << "Failed to get a response from the search API." << std::endl;
            return 0;
        }

        // Parse the JSON response
        json jsonResponse;
        try
        {
            jsonResponse = json::parse(response);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            return 0;
        }

        // Extract the first image link
        std::string imageUrl;
        try
        {
            if (!jsonResponse["items"].empty())
            {
                imageUrl = jsonResponse["items"][0]["link"].get<std::string>();
            }
            else
            {
                std::cerr << "crbn::imageDownloader() No image results found for the search term :" << query << std::endl;
                return 0;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "crbn::imageDownloader() Error extracting image URL: " << e.what() << std::endl;
            return 0;
        }
        
        crbn::jsn::Json_Helper j;
        j.init();
        auto unsafelock = j.setSafeModeUnsafe();
        std::string outputFilePath = j.strGet(crbn::jsn::keys::server_path_to_data) + tagId;

        if (downloadFile(imageUrl, outputFilePath))
        {
            crbn::log( "Image successfully downloaded to " + outputFilePath );
        }
        else
        {
            std::cerr << "Failed to download the image." << std::endl;
            return 0;
        }

        return 1;
    }

}

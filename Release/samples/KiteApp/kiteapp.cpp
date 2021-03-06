#include <cpprest/http_client.h> 
#include <cpprest/filestream.h> 
//----- Some standard C++ headers emitted for brevity
#include "cpprest/json.h" 
#include "cpprest/http_listener.h" 
#include "cpprest/uri.h" 
#include "cpprest/asyncrt_utils.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>
#include <openssl/sha.h>
//////////////////////////////////////////////// 
// A Simple HTTP Client to Demonstrate  
// REST SDK Client programming model 
// The Toy sample shows how one can read  
// contents of a web page 
// 
using namespace utility;  // Common utilities like string conversions 
using namespace web;      // Common features like URIs. 
using namespace web::http;// Common HTTP functionality 
using namespace web::http::client;// HTTP client features 
using namespace concurrency::streams;// Asynchronous streams 

/* 
    /// <summary>
    /// Asynchronously sends an HTTP request.
    /// </summary>
    /// <param name="mtd">HTTP request method.</param>
    /// <param name="path_query_fragment">String containing the path, query, and fragment, relative to the http_client's
    /// base URI.</param> <param name="body_data">The data to be used as the message body, represented using the json
    /// object library.</param> <param name="token">Cancellation token for cancellation of this request
    /// operation.</param> <returns>An asynchronous operation that is completed once a response from the request is
    /// received.</returns>
    pplx::task<http_response> request(const method& mtd,
                                      const utility::string_t& path_query_fragment,
                                      const json::value& body_data,
                                      const pplx::cancellation_token& token = pplx::cancellation_token::none())
    {
*/
std::string sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

int main(int argc, char* argv[]) 
{ 
   auto fileStream = std::make_shared<ostream>(); 
   // Open stream to output file. 
   pplx::task<void> requestTask =  
              fstream::open_ostream(U("jsondata.txt")). 
         then([=](ostream outFile) 
   { 
         *fileStream = outFile;
         std::string api_key = utility::conversions::to_utf8string("utmcr52rmqeze8sc");
         std::string api_secret = utility::conversions::to_utf8string("443isdhujy5d45jw85b4ldr0wmbg07me");
         std::string request_token= utility::conversions::to_utf8string(argv[1]);

         std::string checksum=sha256(api_key + request_token + api_secret);
         std::string req_string("api_key="+api_key+"&request_token="+request_token+"&checksum="+checksum);

         // Send request to the Kite and get the response
         http_client client(U("https://api.kite.trade")); 
         // Build request URI and start the request. 
          uri_builder builder(U("/session/token"));
          //std::cout<<builder.to_string()<<"\n"<<body_data<<std::endl;
         return client.request(methods::POST, utility::conversions::to_utf8string(builder.to_string()), req_string,
                               utility::conversions::to_utf8string("application/x-www-form-urlencoded"));
 
   }).then([=](http_response response) 
   { 
         printf("Received response status code:%un",  
                                    response.status_code()); 
             return response.body(). 
                           read_to_end(fileStream->streambuf()); 
   }).then([=](size_t total){
         printf("Exiting...\n");
         return fileStream->close(); 
   }); 
 
   // We have not started execution, just composed 
   // set of tasks in a Continuation Style 
   // Wait for all the outstanding I/O to complete  
   // and handle any exceptions, If any  
   try { 
         //-- All Taskss will get triggered here 
         requestTask.wait(); 
   } 
   catch (const std::exception &e) { 
         printf("Error exception:%sn", e.what()); 
   } 

   getchar(); 
   return 0; 
} 

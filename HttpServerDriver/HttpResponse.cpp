#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "boost/filesystem.hpp"  

#include "HttpResponse.hpp"

namespace Naive
{
    namespace Http
    {
        std::map<uint8_t, std::string> Response::http_codes{
            { 200, "OK" },
            { 301, "Moved Permanently" },
            { 400, "Bad Request" },
            { 401, "Not Authorized" },
            { 403, "Forbidden" },
            { 404, "Not Found" },
            { 500, "Internal Server Error" }
        };
        std::map<std::string, std::string> Response::mime_types{
            { "jpg", "image/jpeg" },
            { "gif", "image/gif" },
            { "css", "text/css" },
            { "js", "application/x-javascript" },
            { "html", "text/html" },
            { "png", "image/png" },
            { "ico", "image/x-icon" }
        };
        Response::Response() : m_code(400)
        {
            set_response_body("invalid");
            default_headers();
        }
        Response::Response(std::string response_text, uint8_t http_code) :
            m_code(http_code)
        {
            set_response_body(response_text);
            default_headers();
        }
        void Response::default_headers()
        {
            set_header("Connection", "keep-alive");
            set_header("Content-Type", "text/plain");
        }
        Response::~Response()
        {
        }
        void Response::set_response_body(std::string b)
        {
            set_body(b);
            set_header("Content-Length", std::to_string(get_body().size()));
        }
        void Response::set_file_response(std::string fpath)
        {
            //TODO: If fpath ends in a slash, search that dir for a default file.
            std::ifstream infile(fpath, std::ios::binary);
            if (!infile)
            {
                m_code = 404;
                set_response_body("404 file not found in " + boost::filesystem::current_path().string());
                return;
            }
            std::ostringstream oss;
            oss << infile.rdbuf();
            set_response_body(oss.str());
            m_code = 200;

            //content-type
            std::string ct("text/plain");
            std::for_each(mime_types.begin(), mime_types.end(), [&ct, fpath](auto &mt) {
                std::string extension = "." + mt.first;
                std::string mime_type = mt.second;
                if (fpath.size() >= extension.size() + 1)
                {
                    auto mismatch = std::mismatch(extension.begin(), extension.end(), fpath.end() - (extension.size()));
                    if (mismatch.first == extension.end() && mismatch.second == fpath.end())
                    {
                        ct = mime_type;
                    }
                }
            });
            set_header("Content-Type", ct);
        }
        std::string Response::get_text()
        {
            return m_body;
        }
        std::string Response::first_line()
        {
            return "HTTP/1.1 " + std::to_string(m_code) + " " + http_codes[m_code];
        }
        uint8_t Response::get_code()
        {
            return m_code;
        }

        void Response::set_header(std::string key, std::string value)
        {
            m_headers[key] = value;
        }

    } //namespace Http
} //namespace Naive

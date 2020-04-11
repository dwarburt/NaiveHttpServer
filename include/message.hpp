#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include "url.hpp"
#include "auth.hpp"

namespace Naive
{
    namespace Http
    {
        class Request;
        class Response;
        typedef std::shared_ptr<Request> RequestPtr;
        typedef std::shared_ptr<Response> ResponsePtr;

        enum HandleResult {
            Continue,
            Stop
        };

        typedef std::function<HandleResult(RequestPtr, ResponsePtr)> RequestHandler;
        typedef std::function<void(RequestPtr, ResponsePtr)> FinalRequestHandler;
        typedef uint32_t MiddlewareHandle;

        class Message
        {
        public:
            Message();
            virtual ~Message();
            std::string to_string();
            bool keep_alive();
            std::string get_header(std::string);
            std::string get_method();
            Url get_url();
            std::string get_http_version();
            std::string get_body();
            std::map<std::string, std::string> get_headers();

        protected:
            virtual void set_body(std::string);
            virtual std::string first_line() = 0;
            std::map<std::string, std::string> m_headers;
            std::string m_method;  // one of GET, POST etc.
            Url m_url;
            std::string m_http_version;
            std::string m_body;
        };
    }
}

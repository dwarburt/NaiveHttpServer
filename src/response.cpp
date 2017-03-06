#include <iostream>
#include <sstream>
#include <fstream>

#include "response.hpp"
#include "util.hpp"

using namespace std;

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
        Response::Response() : m_code(200)
        {
            set_response_body("");
            default_headers();
        }
        Response::Response(std::string response_text, uint16_t http_code) :
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
        RequestHandler Response::blank(uint16_t code)
        {
            return [=](RequestPtr, ResponsePtr resp) {
                resp->set_code(code);
                return Stop;
            };
        }
        RequestHandler Response::simple(const std::string response, uint16_t code)
        {
            return [=](RequestPtr, ResponsePtr resp) {
                resp->set_response_body(response);
                resp->set_code(code);
                return Stop;
            };
        }
        HandleResult validate(AuthScheme scheme, Credentials creds, FinalRequestHandler next, RequestPtr req, ResponsePtr resp)
        {
            if (req->is_valid(scheme, creds)) {
              next(req, resp);
            }
            else {
              resp->set_code(401);
              resp->set_authenticate_header(scheme);
            }
            return Stop;
        }
        RequestHandler Response::protect(AuthScheme scheme,
            std::function<Credentials()> credential_resolver, FinalRequestHandler next)
        {
            return [=](RequestPtr req, ResponsePtr resp) {
                auto creds = credential_resolver();
                return validate(scheme, creds, next, req, resp);
            };
        }
        RequestHandler Response::protect(AuthScheme scheme,
            std::function<CredentialList()> credentials_resolver, FinalRequestHandler next)
        {
            return [=](RequestPtr req, ResponsePtr resp) {
              auto credslist = credentials_resolver();
              bool valid = false;
              //TODO: look up the correct username to attempt to authenticate
              //      with instead of brute forcing it.
              for (auto &creds : credslist) {
                if (req->is_valid(scheme, creds)) {
                  next(req, resp);
                  valid = true;
                }
              }
              if (!valid) {
                resp->set_code(401);
                resp->set_authenticate_header(scheme);
              }
              return Stop;
            };
        }
        RequestHandler Response::protect(AuthScheme scheme, Credentials creds, FinalRequestHandler next)
        {
          return [=](RequestPtr req, ResponsePtr resp) {
              return validate(scheme, creds, next, req, resp);
          };
        }
        std::string new_nonce() {
            return b64_encode(random_bytes(12));
        }
        void Response::set_authenticate_header(AuthScheme scheme) {
            std::string challenge;
            switch (scheme) {
                case Basic:
                    challenge = "Basic realm=\"WiDAN\"";
                    break;
                case Digest:
                    challenge = create_digest_challenge();
                    break;
            }
            set_header("WWW-Authenticate", challenge);
        }
        std::string Response::create_digest_challenge() {
            return "Digest realm=\"WiDAN\", nonce=\"" + new_nonce() + "\"";
        }

        void Response::set_response_body(std::string b)
        {
            set_body(b);
            set_header("Content-Length", Naive::Http::to_string(get_body().size()));
        }
        void Response::set_file_response(std::string fpath)
        {
            if (Naive::Http::path_is_dir(fpath))
            {
                fpath += "/index.html";
            }
            std::ifstream infile(fpath, std::ios::binary);
            if (!infile)
            {
                m_code = 404;
                set_response_body("404 file not found");
                return;
            }
            std::ostringstream oss;
            oss << infile.rdbuf();
            set_response_body(oss.str());
            m_code = 200;

            //content-type
            std::string ct("text/plain");
            for (const auto &mt : mime_types) {
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
            }
            set_header("Content-Type", ct);
        }
        std::string Response::get_text()
        {
            return m_body;
        }
        std::string Response::first_line()
        {
            return "HTTP/1.1 " + Naive::Http::to_string(m_code) + " " + http_codes[m_code];
        }
        uint16_t Response::get_code()
        {
            return m_code;
        }
        void Response::set_code(uint16_t code)
        {
            m_code = code;
        }
        void Response::set_header(std::string key, std::string value)
        {
            m_headers[key] = value;
        }

    } //namespace Http
} //namespace Naive

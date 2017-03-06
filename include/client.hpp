
#ifndef WEBAGENT_HTTP_CLIENT_H
#define WEBAGENT_HTTP_CLIENT_H

#include <curl/curl.h>
#include <memory>
#include <sstream>
#include <map>

namespace Naive {
  namespace Http {

      enum RequestType {
          REQUEST_PUT,
          REQUEST_GET,
          REQUEST_POST,
          REQUEST_DELETE,
      };
      enum Code {
        OK = 200,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        SERVER_ERROR = 500,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503
      };

      class CResponse {
      public:
          typedef std::shared_ptr<CResponse> Ptr;

          CResponse(uint16_t rspCode = UINT16_MAX, const std::string& rspMsg = "Not Initialized", const std::string& payload = "");
          ~CResponse() {};
          uint16_t getResponseCode() const { return code_; };
          std::string getErrorMessage() const { return message_; };
          std::string getPayload() const { return payload_; };
          bool success() const { return code_ >= 200 && code_ < 300; }
      private:
          uint16_t code_;
          std::string message_;
          std::string payload_;
      };

      typedef std::map<std::string, std::string> HeaderMap;

      class Client {
      public:
          ~Client();

          static CResponse::Ptr sendRequest(RequestType type, const std::string &address,
                                       HeaderMap headers = HeaderMap(), const std::string& body = "",
                                     const std::string& username = "",
                                     const std::string& password = "");

      private:
          Client();
          std::string username;
          std::string password;

          CResponse::Ptr doSend(RequestType type, const std::string &address,
                           HeaderMap headers = HeaderMap(), const std::string& body = "");
          static size_t write_cb(char *ptr, size_t size, size_t nmemb, void *data);

          CURL* curl_;
      };
  }
}
#endif //WEBAGENT_HTTP_CLIENT_H


#include "client.hpp"

namespace Naive {
  namespace Http {
      CResponse::CResponse(uint16_t rspCode, const std::string &message, const std::string &payload) {
          code_ = rspCode;
          message_ = message;
          payload_ = payload;
      }

      Client::Client() {
          curl_ = curl_easy_init();
      }

      Client::~Client() {
          if (nullptr != curl_) {
              curl_easy_cleanup(curl_);
              curl_ = nullptr;
          }
      }

      CResponse::Ptr
      Client::sendRequest(RequestType type, const std::string &address, HeaderMap headers, const std::string &body,
          const std::string& username, const std::string &password) {
          Client client;
          client.password = password;
          client.username = username;
          return client.doSend(type, address, headers, body);
      }

      CResponse::Ptr
      Client::doSend(RequestType type, const std::string &address, HeaderMap headers, const std::string &body) {
          curl_easy_setopt(curl_, CURLOPT_URL, address.c_str());

          if (!body.empty()) {
              curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, body.length());
              curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body.c_str());
          }

          switch (type) {
              case REQUEST_PUT:
                  curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PUT");
                  break;
              case REQUEST_GET:
                  curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "GET");
                  break;
              case REQUEST_POST:
                  curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "POST");
                  break;
              case REQUEST_DELETE:
                  curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
                  break;
          }

          struct curl_slist *headerParams = NULL;
          for (auto &h : headers) {
              headerParams = curl_slist_append(headerParams, (h.first + ": " + h.second).c_str());
          }
          curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headerParams);
          if (username != "") {
            curl_easy_setopt(curl_, CURLOPT_USERNAME, username.c_str());
            curl_easy_setopt(curl_, CURLOPT_PASSWORD, password.c_str());
            curl_easy_setopt(curl_, CURLOPT_HTTPAUTH, CURLAUTH_BASIC|CURLAUTH_DIGEST);
          }


          std::ostringstream payload;
          curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_cb);
          curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &payload);

          long http_code = 0;

          curl_easy_setopt(curl_, CURLOPT_FAILONERROR, true); //<= this is important, but not obvious
          char error_buf[CURL_ERROR_SIZE];
          curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, error_buf);

          curl_easy_perform(curl_);

          curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
          std::string err_msg(error_buf);

          return std::make_shared<CResponse>((uint16_t) http_code, err_msg, payload.str());
      }

      size_t Client::write_cb(char *ptr, size_t size, size_t nmemb, void *data) {
          size_t rlen = size * nmemb;
          std::ostringstream* str = (std::ostringstream*)data;
          *str << std::string(ptr, rlen);
          return rlen;
      }
  }

}

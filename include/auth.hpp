#pragma once
#include "util.hpp"

namespace Naive
{
    namespace Http
    {
        enum AuthScheme {
          Basic,
          Digest
        };

        struct Credentials {
          const std::string username;
          const std::string password;
        };

        class Auth
        {
        public:
          AuthScheme m_scheme;
          std::string m_username;
          std::string m_nonce;
          std::string m_password;
          std::string m_realm;
          std::string m_uri;
          std::string m_response;
          std::string m_method;
          std::string m_qop;
          std::string m_algorithm;
          std::string m_opaque;
          std::string m_authorization;
          std::string m_cnonce;
          std::string m_challenge_header;
          int m_nc;
          Auth(std::string m) : m_method(m), m_cnonce(b64_encode(random_bytes(12))), m_nc(0) {}
          static Auth from_credentials(const std::string& m, const std::string &authorization_header);
          static Auth from_challenge(const std::string& m, const std::string& challenge_header, const std::string& uri, const Credentials& creds);
          void set_cnonce(const std::string& cnonce);
          bool check_digest(Credentials creds);
          bool is_valid(AuthScheme input_scheme, Credentials creds);

        private:
          std::string generate_authorize_header(const std::string& authenticate_header);
          void parse_auth(const std::string &authorization);
          void parse_digest(const std::string &digest_auth);
          void parse_basic(const std::string &basic_auth);
          std::string build_digest(Credentials creds);
      };
    }
}

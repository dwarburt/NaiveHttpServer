#include "auth.hpp"
#include "util.hpp"

namespace Naive
{
    namespace Http
    {
      Auth Auth::from_credentials(const std::string& m, const std::string& authenticate_header) {
        Auth ret(m);
        ret.parse_auth(authenticate_header);
        return ret;
      }
      Auth Auth::from_challenge(const std::string& m, const std::string& challenge_header, const std::string& uri, const Credentials& creds) {
        Auth ret(m);
        ret.m_uri = uri;
        ret.m_username = creds.username;
        ret.m_password = creds.password;
        ret.m_challenge_header = challenge_header;
        ret.parse_auth(challenge_header);
        ret.m_authorization = ret.generate_authorize_header(challenge_header);
        return ret;
      }
      void Auth::set_cnonce(const std::string& cnonce) {
        m_cnonce = cnonce;
        parse_auth(m_challenge_header);
        m_authorization = generate_authorize_header(m_challenge_header);
      }

      bool Auth::is_valid(AuthScheme input_scheme, Credentials creds) {
          if (input_scheme != m_scheme) {
              return false;
          }
          switch (m_scheme) {
              case Basic:
              {
                  if (    m_username != creds.username
                       || m_password != creds.password)
                  {
                      return false;
                  }
                  return true;
              }
              case Digest:
                  return check_digest(creds);
          }
          return true;
      }

      void Auth::parse_auth(const std::string &authorization)
      {
          std::vector<std::string> type_and_value;
          split_first(type_and_value, authorization, " ");
          if (type_and_value[0] == "Digest") {
              m_scheme = Digest;
              parse_digest(type_and_value[1]);
          }
          else if (type_and_value[0] == "Basic") {
              m_scheme = Basic;
              parse_basic(type_and_value[1]);
          }
      }
      void Auth::parse_basic(const std::string &basic_auth)
      {
          auto bin_auth_data = b64_decode(basic_auth);
          std::string combined_auth_data(bin_auth_data.begin(), bin_auth_data.end());
          std::vector<std::string> user_and_pass;
          split(user_and_pass, combined_auth_data, ":");
          m_username = user_and_pass[0];
          m_password = user_and_pass[1];
      }
      void Auth::parse_digest(const std::string &digest_auth)
      {
          std::vector<std::string> digest_fields;
          split(digest_fields, digest_auth, ", ");
          for (const auto &pair : digest_fields)
          {
              std::vector<std::string> key_value;
              split_first(key_value, pair, "=");
              std::string k(key_value[0]);
              std::string v(unquote(key_value[1]));
              if (k == "username") {
                  m_username = v;
              }
              else if (k == "realm") {
                  m_realm = v;
              }
              else if (k == "nonce") {
                  m_nonce = v;
              }
              else if (k == "uri") {
                  m_uri = v;
              }
              else if (k == "response") {
                  m_response = v;
              }
              else if (k == "qop") {
                  m_qop = v;
              }
              else if (k == "algorithm") {
                  m_algorithm = v;
              }
              else if (k == "opaque") {
                  m_opaque = v;
              }
          }
      }

      std::string lastpair(const std::string& k, const std::string& v) {
        return k + "=" + "\"" + v + "\"";
      }
      std::string pair(const std::string& k, const std::string& v) {
        return lastpair(k,v) + ", ";
      }
      std::string upair(const std::string& k, const std::string& v) {
        return k + "=" + v + ", ";

      }

      //given inputs, come up wit this:
      //Digest username="dev1", realm="midan.api.intel.com", nonce="MTQ4MTU4MjA1ODo5MWY4OGE4NjZmOTIzNjNiMzdiNzg0NmQ2MjZmOGU0Zg==", uri="/sumthin", qop=auth, nc=, cnonce="", response="b2a2db920c595b28c2b6cdc58a2f9ebd", opaque="32f271c3383e3405aae141ed50d7e774"
      //example challenge:Digest realm="midan.api.intel.com", qop="auth", algorithm=MD5, nonce="MTQ4MTU4MjA1ODo5MWY4OGE4NjZmOTIzNjNiMzdiNzg0NmQ2MjZmOGU0Zg==", opaque="32f271c3383e3405aae141ed50d7e774"

      std::string Auth::generate_authorize_header(const std::string& authenticate_header)
      {
          parse_auth(authenticate_header);
          m_nc = m_nc + 1;
          auto response = build_digest({m_username, m_password});
          std::string header(
            "Digest " +
            pair("username", m_username) +
            pair("realm", m_realm) +
            pair("nonce", m_nonce) +
            pair("uri", m_uri) +
            upair("qop", m_qop) +
            upair("nc", to_string(m_nc)) +
            pair("cnonce", m_cnonce) +
            pair("response", response) +
            lastpair("opaque", m_opaque)
          );
          return header;
      }
      bool Auth::check_digest(Credentials creds) {
          if (creds.username != m_username) {
              return false;
          }
          return build_digest(creds) == m_response;
      }
      std::string Auth::build_digest(Credentials creds) {
        std::string a1(creds.username + ":" + m_realm + ":" + creds.password);
        std::string a2(m_method + ":" + m_uri);
        if (m_qop == "auth") {
          std::string nonce_count = to_string(m_nc);
          return hash(
            join(std::vector<std::string> {
              hash(a1),
              m_nonce,
              nonce_count,
              m_cnonce,
              m_qop,
              hash(a2)
            }, ":"));
          //return hash(hash(a1) + ":" + m_nonce + ":" + nonce_count + ":" + m_cnonce + ":" + m_qop + ":" + hash(a2));
        }
        return hash(hash(a1) + ":" + m_nonce + ":" + hash(a2));
      }

    }
}

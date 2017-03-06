
#include <gtest/gtest.h>
#include <asio.hpp>
#include "http/util.hpp"
#include "http/server.hpp"
#include "gmock/gmock.h"
#include "gateway.h"
#include "storage.h"
#include "network_map.h"
#include "http_client.h"

using namespace Naive::Http;

TEST(http_server, http_server_can_answer_requests) {
    Server myServer;
    std::string expected_body("http_server_can_answer_requests");
    myServer.get("/test", [&expected_body](RequestPtr, ResponsePtr resp) {
        resp->set_response_body(expected_body);
    });
    myServer.start();
    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test");
    ASSERT_TRUE(resp->success());
    ASSERT_EQ(expected_body, resp->getPayload());
}
TEST(http_server, http_server_will_404_on_not_found) {
    Server myServer;
    myServer.get("/test", [](RequestPtr, ResponsePtr resp) {
        resp->set_response_body("http_server_can_answer_requests");
    });
    myServer.start();
    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/not_test");
    ASSERT_FALSE(resp->success());
}
TEST(http_server, http_server_blank_response_working_ok) {
    Server myServer;
    myServer.get("/test", Response::blank(200) );
    myServer.start();
    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test");
    ASSERT_TRUE(resp->success());
}
TEST(http_server, http_server_blank_response_working_403) {
    Server myServer;
    myServer.get("/test", Response::blank(403) );
    myServer.start();
    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test");
    ASSERT_EQ(403, resp->getResponseCode());
}
TEST(http_server, http_server_will_listen_on_another_port) {
    Server myServer("127.0.0.1", 46361);
    myServer.get("/test", Response::blank(200) );
    myServer.start();
    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:46361/test");
    ASSERT_TRUE(resp->success());
}

TEST(http_server, serve_map_with_no_nodes) {
    std::string nm("{}");
    Naive::Http::Server localServer;
    localServer.get("/map", [nm](RequestPtr, ResponsePtr resp) {
        resp->set_code(200);
        resp->set_response_body(nm);
    });
    localServer.start();

    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/map");
    ASSERT_EQ(true, resp->success());

    ASSERT_STREQ(nm.c_str(), resp->getPayload().c_str());
}

TEST(http_server, serve_map_with_nodes) {
    std::string nm("{\"ac:9a:96:f4:0a:f5\":{\"state\":2,\"ap_mac\":\"00:00:00:00:00:00\",\"type\":0,\"hostaps\":[{\"if_type\":16,\"bandwidth\":3,\"channel\":60,\"active\":1,\"mac\":\"ac:9a:96:f4:0b:10\"}],\"channel\":0,\"if_type\":0,\"ipv4\":\"192.168.1.1\",\"parent_mac\":\"00:00:00:00:00:00\",\"mac\":\"ac:9a:96:f4:0a:f5\",\"rssi\":-128,\"platform\":4},\"a0:99:9b:01:69:d1\":{\"state\":2,\"ap_mac\":\"ac:9a:96:f4:0b:10\",\"type\":3,\"channel\":60,\"if_type\":1768776992,\"ipv4\":\"192.168.1.244\",\"parent_mac\":\"ac:9a:96:f4:0a:f5\",\"mac\":\"a0:99:9b:01:69:d1\",\"rssi\":-41,\"platform\":1009087592}}");

    Naive::Http::Server localServer;
    localServer.get("/map", [nm](RequestPtr, ResponsePtr resp) {
        resp->set_code(200);
        resp->set_response_body(nm);
    });
    localServer.start();

    auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/map");
    ASSERT_EQ(true, resp->success());

    ASSERT_STREQ(nm.c_str(), resp->getPayload().c_str());
}

TEST(http_server, digest_authentication_produces_challenge) {
  Server myServer;
  myServer.get("/test", Response::protect(Digest, {"user", "pass"},
    Response::blank(200)));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test");
  ASSERT_FALSE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 401);
}
TEST(http_server, basic_authentication_accepts_correct_password) {
  Server myServer;
  myServer.get("/test", Response::protect(Basic, {"user", "pass"},
    Response::blank(200)));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    "user", "pass");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);
}
TEST(http_server, digest_authentication_accepts_correct_password) {
  Server myServer;
  myServer.get("/test", Response::protect(Digest, {"MyUserName", "pass4test"},
    Response::blank(200)));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    "MyUserName", "pass4test");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);
}
TEST(http_server, digest_authentication_rejects_incorrect_password) {
  Server myServer;
  myServer.get("/test", Response::protect(Digest, {"user", "pass"},
    Response::blank(200)));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    "user", "pass2");
  ASSERT_FALSE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 401);
}
TEST(http_server, basic_authentication_rejects_incorrect_password) {
  Server myServer;
  myServer.get("/test", Response::protect(Basic, {"user", "pass"},
    Response::blank(200)));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    "user", "pass2");
  ASSERT_FALSE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 401);
}
TEST(http_server, DISABLED_digest_authentication_allows_nonce_reuse) {
  Server myServer;
  std::string nonce1;
  std::string nonce2;
  myServer.get("/test1", Response::protect(Digest, {"user", "pass"},
    [&nonce1](RequestPtr req, ResponsePtr resp) {
      std::string extracted_nonce {req->authdata().m_nonce};
      //somehow checkout the request for a www-authenticate header or whatever.
      nonce1 = extracted_nonce;
      resp->set_code(200);
      return Stop;
    }));
  myServer.get("/test2", Response::protect(Digest, {"user", "pass"},
      [&nonce2](RequestPtr req, ResponsePtr resp) {
        std::string extracted_nonce {req->authdata().m_nonce};
        //somehow checkout the request for a www-authenticate header or whatever.
        nonce2 = extracted_nonce;
        resp->set_code(200);
        return Stop;
      }));

  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test1", Http::HeaderMap(), "",
    "user", "pass");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);
  resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test2", Http::HeaderMap(), "",
    "user", "pass");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);
  ASSERT_EQ(nonce1, nonce2);
}
TEST(http_server, digest_authentication_can_extract_username) {
  Server myServer;
  std::string username_expected {"user"};
  std::string username_actual;
  myServer.get("/test", Response::protect(Digest, {username_expected, "pass"},
    [&username_actual](RequestPtr req, ResponsePtr resp) {
      std::string extracted_username {req->authdata().m_username};
      username_actual = extracted_username;
      resp->set_code(200);
      return Stop;
    }));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    username_expected, "pass");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);
  ASSERT_EQ(username_expected, username_actual);
}
TEST(http_server, can_protect_with_many_users) {
  Server myServer;
  std::function<CredentialList()> resolver = []() {
    return CredentialList {
      {"user1", "pass1"},
      {"user2", "pass2"}
    };
  };
  myServer.get("/test", Response::protect(Digest, resolver,
    Response::blank(200)));
  myServer.start();
  auto resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    "user1", "pass1");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);
  resp = Http::Client::sendRequest(Http::REQUEST_GET, "http://127.0.0.1:55055/test", Http::HeaderMap(), "",
    "user2", "pass2");
  ASSERT_TRUE(resp->success());
  ASSERT_EQ(resp->getResponseCode(), 200);

}
TEST(http_server_auth_client, can_create_authorization_correctly) {
  std::string challenge_header = "Digest realm=\"midan.api.intel.com\", qop=auth, algorithm=MD5, nonce=\"MTQ4MTU4MjA1ODo5MWY4OGE4NjZmOTIzNjNiMzdiNzg0NmQ2MjZmOGU0Zg==\", opaque=\"32f271c3383e3405aae141ed50d7e774\"";
  std::string expected = "Digest username=\"dev1\", realm=\"midan.api.intel.com\", nonce=\"MTQ4MTU4MjA1ODo5MWY4OGE4NjZmOTIzNjNiMzdiNzg0NmQ2MjZmOGU0Zg==\", uri=\"/sumthin\", qop=auth, nc=2, cnonce=\"pW2cmyESm\", response=\"e52e37c83ed171a52c06b70cffe1d940\", opaque=\"32f271c3383e3405aae141ed50d7e774\"";

  Auth htua = Naive::Http::Auth::from_challenge("GET", challenge_header, "/sumthin", {"dev1", "6XKNvVboJpW2cmyESm8wYxYp"});
  htua.set_cnonce("pW2cmyESm");
  ASSERT_EQ(expected, htua.m_authorization);
}
TEST(b64, encode)
{
    std::string before("hello buddy");
    std::string expected("aGVsbG8gYnVkZHk=");
    std::string actual = Naive::Http::b64_encode(before);
    ASSERT_EQ(expected, actual);
}
TEST(b64, decode)
{
    std::string before("TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIHZlcnRlcmVtIGZhY2lsaXNpcyBwcm8gZXgsIGV4IHNpdCByZXF1ZSBob21lcm8uIEFkIHRlbXBvciBwZXJjaXBpdHVyIHNlYS4gVml4IGlkIG1lbnRpdHVtIGRlbGljYXRhLiBFeCB2aWRpdCBjb3Bpb3NhZSBwZXRlbnRpdW0gdXN1LCB2ZWwgbnVsbGFtIHBlcmNpcGl0dXIgZWkuIFVzdSBuZSBhc3N1bSBpdXZhcmV0IGNvbmNsdXNpb25lbXF1ZSwgZ3JhZWNlIGl1dmFyZXQgaW5zb2xlbnMgZXQgc2l0LCBubyBuZWMgaGluYyBkZWZpbml0aW9uZW0uIEVhIG1hZ25hIG1lbGl1cyB2aXJ0dXRlIGl1cywgaW4gcHJvIG9tbml1bSBtbmVzYXJjaHVtLiBJbXBldHVzIG1haWVzdGF0aXMgc2NyaXB0b3JlbSBlc3QgZXgsIGFkIHNlYSBsYXVkZW0gZXF1aWRlbS4gRXJyZW0gY29uc2VxdWF0IHRlIG1lbC4gTWVpcyBsYWJvcmUgZmFiZWxsYXMgbm8gc2VkLCB2ZXJpdHVzIHBlcnRpbmFjaWEgcHJpIGV1LCBoYXMgcmVjdXNhYm8gY29uc2VxdWF0IHV0LiBFeGVyY2kgaW50ZXJwcmV0YXJpcyB1c3UgZXQuIFNlZCBleCBmYWNpbGlzaXMgaG9uZXN0YXRpcy4gRXN0IHF1aWRhbSBzY2Fldm9sYSBwZXJjaXBpdCBleCwgcHVydG8gaWRxdWUgaGFzIHV0LCB0ZSBlbGl0IGludmVuaXJlIHNpdC4gRXN0IG51c3F1YW0gb21uZXNxdWUgcmVmb3JtaWRhbnMgZXguIE5hbSBoZW5kcmVyaXQgbmVnbGVnZW50dXIgZXQsIG1lYSBlcmF0IHNlbnNpYnVzIHRlLiBQcmkgcHJvbXB0YSBvbWl0dGFudHVyIGFkLiBDdSBtZWxpdXMgYWxidWNpdXMgbmVjZXNzaXRhdGlidXMgZW9zLiBDYXNlIGRvbG9ydW0gY3VtIGN1LCBoYXMgbm9taW5hdGkgdGluY2lkdW50IHZpdHVwZXJhdG9yaWJ1cyBjdS4gVmlzIGhhYmVvIG1lbGlvcmUgZXQsIHF1YWxpc3F1ZSBkZXNlcnVpc3NlIHBlciBhZCwgY3VtIHRlIHBvc3NpdCBwYXRyaW9xdWUgYXJndW1lbnR1bS4=");
    std::string expected("Lorem ipsum dolor sit amet, verterem facilisis pro ex, ex sit reque homero. Ad tempor percipitur sea. Vix id mentitum delicata. Ex vidit copiosae petentium usu, vel nullam percipitur ei. Usu ne assum iuvaret conclusionemque, graece iuvaret insolens et sit, no nec hinc definitionem. Ea magna melius virtute ius, in pro omnium mnesarchum. Impetus maiestatis scriptorem est ex, ad sea laudem equidem. Errem consequat te mel. Meis labore fabellas no sed, veritus pertinacia pri eu, has recusabo consequat ut. Exerci interpretaris usu et. Sed ex facilisis honestatis. Est quidam scaevola percipit ex, purto idque has ut, te elit invenire sit. Est nusquam omnesque reformidans ex. Nam hendrerit neglegentur et, mea erat sensibus te. Pri prompta omittantur ad. Cu melius albucius necessitatibus eos. Case dolorum cum cu, has nominati tincidunt vituperatoribus cu. Vis habeo meliore et, qualisque deseruisse per ad, cum te possit patrioque argumentum.");
    auto after_data = Naive::Http::b64_decode(before);
    std::string actual(after_data.begin(), after_data.end());
    ASSERT_EQ(expected, actual);
}
TEST(hash, md5)
{
    std::string before("hello");
    std::string expected("5d41402abc4b2a76b9719d911017c592");
    std::string actual = Naive::Http::to_hex(
        Naive::Http::md5sum(before)
    );
    ASSERT_EQ(expected, actual);
}
TEST(random, bytes)
{
    auto newbytes = Naive::Http::random_bytes(10);
    ASSERT_EQ((size_t)10, newbytes.size());
}
TEST(trimming, quotes)
{
    std::string before("\"a quoted string\"");
    std::string expected("a quoted string");
    std::string actual = Naive::Http::unquote(before);
    ASSERT_EQ(expected, actual);
}
TEST(trimming, leading_newline)
{
    std::string before("\na string.");
    std::string expected("a string.");
    std::string actual = Naive::Http::trim(before);
    ASSERT_EQ(expected, actual);
}
TEST(trimming, leading_space)
{
    std::string before(" a string.");
    std::string expected("a string.");
    std::string actual = Naive::Http::trim(before);
    ASSERT_EQ(expected, actual);
}
TEST(trimming, leading_spaces)
{
    std::string before("      a string.");
    std::string expected("a string.");
    std::string actual = Naive::Http::trim(before);
    ASSERT_EQ(expected, actual);
}
TEST(trimming, trailing_spaces)
{
    std::string before("a string.    ");
    std::string expected("a string.");
    std::string actual = Naive::Http::trim(before);
    ASSERT_EQ(expected, actual);
}

TEST(trimming, mixed_whitespace)
{
    std::string before(" \t   \na string. \t   ");
    std::string expected("a string.");
    std::string actual = Naive::Http::trim(before);
    ASSERT_EQ(expected, actual);
}

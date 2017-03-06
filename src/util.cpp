#include "util.hpp"
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>


namespace Naive
{
    namespace Http
    {
        std::string unquote(std::string input)
        {
            return remove_both(input, "\"");
        }
        std::string trim(std::string input)
        {
            std::vector<std::string> whitespace {" ", "\t", "\n", "\r"};
            std::string ret(input);
            do {
                input = ret;
                for (auto c : whitespace) {
                    ret = remove_both(ret, c);
                }
            } while (ret != input);
            return ret;
        }
        std::string remove_left(std::string input, const std::string &out)
        {
            auto begin_iter = input.begin();
            do {
                auto mm = std::mismatch(out.begin(), out.end(), begin_iter);
                if (mm.first != out.end()) {
                    break;
                }
                begin_iter = mm.second;
            } while(true);
            return std::string(begin_iter, input.end());
        }
        std::string remove_right(std::string input, const std::string &out)
        {
            // reverse iterators.
            auto begin_iter = input.rbegin();
            do {
                auto mm = std::mismatch(out.rbegin(), out.rend(), begin_iter);
                if (mm.first != out.rend()) {
                    break;
                }
                begin_iter = mm.second;
            } while(true);
            return std::string(input.begin(), begin_iter.base());
        }
        std::string remove_both(std::string input, const std::string &out)
        {
            return remove_right(remove_left(input, out), out);
        }
        std::vector<uint8_t> b64_decode(const std::string &data)
        {
            std::vector<uint8_t> buffer(data.length(), 0);
            BIO *b64 = BIO_new(BIO_f_base64());
            BIO *bmem = BIO_new_mem_buf((void*)data.data(), data.length());

            BIO_set_flags(b64,BIO_FLAGS_BASE64_NO_NL);
            bmem = BIO_push(b64, bmem);

            auto decodedDataLength = BIO_read(bmem, buffer.data(), data.length());

            BIO_free_all(bmem);

            return std::vector<uint8_t>(buffer.begin(), buffer.begin() + decodedDataLength);
        }
        std::string b64_encode(const std::string &data)
        {
            std::vector<uint8_t> new_data(data.begin(), data.end());
            return b64_encode(new_data);
        }
        std::string b64_encode(const std::vector<uint8_t> &data)
        {
            BIO *bmem, *b64;
            BUF_MEM *bptr;

            b64 = BIO_new(BIO_f_base64());
            bmem = BIO_new(BIO_s_mem());
            BIO_set_flags(b64,BIO_FLAGS_BASE64_NO_NL);
            b64 = BIO_push(b64, bmem);
            BIO_write(b64, data.data(), data.size());
            (void)BIO_flush(b64);
            BIO_get_mem_ptr(b64, &bptr);

            std::string decoded(bptr->data, bptr->length);
            BIO_free_all(b64);
            return decoded;
        }
        std::vector<uint8_t> md5sum(const std::string &data)
        {
            std::vector<uint8_t> hash(16, 0);
            auto input_ptr = (const unsigned char *) data.c_str();
            MD5(input_ptr, data.length(), hash.data());
            return hash;
        }
        // returns lowercase
        std::string to_hex(const std::vector<uint8_t> &data)
        {
            std::ostringstream oss;
            oss << std::hex;
            for (auto h : data) {
                oss << std::setfill('0') << std::setw(2) << (int)h;
            }
            return oss.str();
        }
        std::string hash(const std::string &data)
        {
            return to_hex(md5sum(data));
        }
        std::vector<uint8_t> random_bytes(int count) {
            std::vector<uint8_t> ret(count, 0);
            RAND_bytes((unsigned char*)ret.data(), count);
            return ret;
        }
    }
}

#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <deque>
#include <vector>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <unordered_map>

#include <sys/stat.h>

namespace Naive
{
    namespace Http
    {
        // transform "a string" -> a string.
        std::string unquote(std::string input);
        std::string trim(std::string input);
        std::string remove_left(std::string input, const std::string &out);
        std::string remove_right(std::string input, const std::string &out);
        std::string remove_both(std::string input, const std::string &out);

        /*
         * String manipulations
         */
        template<class T>
        void split(T &output, std::string input, std::string seperator)
        {
            output.clear();
            size_t next;
            size_t start = 0;
            do {
                next = input.find(seperator, start);
                output.push_back(input.substr(start, next - start));
                start = next + seperator.length();
            } while(next != std::string::npos);
        }

        template<class T>
        void trimsplit(T &output, std::string input, std::string seperator)
        {
            output.clear();
            size_t next;
            size_t start = 0;
            do {
                next = input.find(seperator, start);
                output.push_back(trim(input.substr(start, next - start)));
                start = next + seperator.length();
            } while(next != std::string::npos);
        }

        template<class T>
        std::string join(const T &input, const std::string &sep)
        {
            std::stringstream oss;
            for (size_t i = 0; i < input.size(); i++)
            {
                if (i != 0)
                {
                    oss << sep;
                }
                oss << input[i];
            }
            return oss.str();
        }

        // You get back either 1 or 2 strings, one if the pattern is not found
        // and two if it is.  first string will be the contents of input up to
        // the first occurance of seperator, second string is the remainder.
        template<class T>
        void split_first(T &output, std::string input, std::string seperator)
        {
            output.clear();
            std::deque<std::string> v;
            split(v, input, seperator);
            output.push_back(v.front());
            v.pop_front();
            if (v.size() > 0)
            {
                std::string part2 = join(v, seperator);
                output.push_back(part2);
            }
        }


        /*
         * to_string helpers
         */
         inline std::string to_string(uint16_t &number)
         {
             std::ostringstream oss;
             oss << number;
             return oss.str();
         }
         inline std::string to_string(std::basic_string<char>::size_type number)
         {
             std::ostringstream oss;
             oss << number;
             return oss.str();
         }
         inline bool path_is_dir(const std::string &path) {
             struct stat fsdata;
             auto result = stat(path.c_str(), &fsdata);
             if (result != 0) {
                 return false;
             }
             return fsdata.st_mode & S_IFDIR;
         }
        /*
         * Crypto
         */
        std::vector<uint8_t> b64_decode(const std::string &data);
        std::string b64_encode(const std::vector<uint8_t> &data);
        std::string b64_encode(const std::string &data);
        std::vector<uint8_t> md5sum(const std::string &data);
        std::string to_hex(const std::vector<uint8_t> &data);
        std::string hash(const std::string &data);
        std::vector<uint8_t> random_bytes(int count);
    }
}

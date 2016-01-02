#pragma once
#include <string>
#include <iostream>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/finder.hpp>

namespace Naive
{
    namespace Http
    {
       template<class T>
       void split(T &output, std::string input, std::string seperator)
       {
           iter_split(output, input, boost::algorithm::first_finder(seperator.c_str()));
       }

       template<class T>
       std::string join(const T &input, const std::string &sep)
       {
           std::stringstream oss;
           for (size_t i = 0; i < input.size(); ++i)
           {
               if (i != 0)
                   oss << sep;
               oss << input[i];
           }
           return oss.str();
       }
       void debug(std::string msg);
    }
}

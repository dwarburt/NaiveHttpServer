#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/finder.hpp>

namespace Naive
{
    namespace Http
    {
       template<class T>
       void split(T &output, std::string input, std::string seperator)
       {
           output.clear();
           iter_split(output, input, boost::algorithm::first_finder(seperator.c_str()));
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
       // the first occurance of seperator, second string is te remainder.
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
       void debug(std::string msg);
    }
}

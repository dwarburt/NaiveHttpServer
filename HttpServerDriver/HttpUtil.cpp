#include "HttpUtil.hpp"
//#include <deque>

namespace Naive
{
    namespace Http
    {
        void debug(std::string msg)
        {
#if DEBUG
            std::cout << msg << std::endl;
#endif
        }
    }
}

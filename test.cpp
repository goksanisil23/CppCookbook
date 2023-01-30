#include <iostream>
#include <tuple>

namespace top
{
    inline namespace mid
    {
        inline namespace bot
        {
            void myfunc(){std::cout << "myfunc\n";}
        }
    }
}

// using namespace top;

int main(void) {
    top::mid::bot::myfunc();
    return 0;
} 
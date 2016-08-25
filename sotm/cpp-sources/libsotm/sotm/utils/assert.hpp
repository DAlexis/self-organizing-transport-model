#ifndef ASSERT_HPP_INCLUDE
#define ASSERT_HPP_INCLUDE

#include <stdexcept>
#include <string>

#ifdef DEBUG
    /// @todo Add here assertion realizetion
    #define ASSERT(condition, message) if (not (condition)) throw std::runtime_error(std::string((message)));
#else
    #define ASSERT(condition, message)
#endif

namespace Stepmod
{

//void __assert(bool condition, );

}

#endif // ASSERT_HPP_INCLUDE

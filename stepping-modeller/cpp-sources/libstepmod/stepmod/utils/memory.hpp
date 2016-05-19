#ifndef MEMORY_HPP_INCLUDED
#define MEMORY_HPP_INCLUDED

#include <cstring>

namespace Stepmod
{

template <typename T>
void zerify(T& object)
{
    memset(&object, 0, sizeof(T));
}

}

#endif // MEMORY_HPP_INCLUDED

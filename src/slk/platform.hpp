#ifndef WC3LIB_SLK_PLATFORM_HPP
#define WC3LIB_SLK_PLATFORM_HPP

#include "../map/platform.hpp"
#include "../format.hpp"

namespace wc3lib
{
	
namespace slk
{

typedef map::int32 index;

typedef Format<map::byte> Format;
typedef std::basic_istream<map::byte> istream;
typedef std::basic_ostream<map::byte> ostream;

}

}

#endif

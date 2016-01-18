#ifndef _GRAPHICS_ELEMENT_TYPES_
#define _GRAPHICS_ELEMENT_TYPES_

#include <stdint.h>

namespace VELaSSCo{
	namespace Graphics{
		struct SphereElement
		{
			float     x, y, z;
			float     radius;
			int64_t   vertexID;	
		};
	}
}

#endif // _ELEMENT_TYPES_
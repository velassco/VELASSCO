#ifndef _GRAPHICS_ELEMENT_TYPES_
#define _GRAPHICS_ELEMENT_TYPES_

#include <stdint.h>

namespace VELaSSCo{
	namespace Graphics{
		struct VertexElement
		{
			int64_t  vertexID;
			double   x, y, z;
		};
		
		struct SphereElement
		{
			double    x, y, z;
			double    radius;
			int64_t   vertexID;	
		};
	}
}

#endif // _ELEMENT_TYPES_

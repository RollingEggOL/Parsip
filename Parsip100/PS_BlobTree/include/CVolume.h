#ifndef CVOLUME_H
#define CVOLUME_H

#include "PS_FrameWork/include/mathHelper.h"
#include "PS_FrameWork/include/PS_Vector.h"
#include "PS_FrameWork/include/PS_Matrix.h"
#include "PS_FrameWork/include/PS_Quaternion.h"

namespace PS{
namespace BLOBTREE{
namespace Vol{
	using namespace MATH;
		class  CVolume
		{
		public:
			float Min(vec3 v);

			float Max(vec3 v);

			vec3f vectorMin(vec3f a, vec3f b);
			vec3f vectorMax(vec3f a, vec3f b);			

			vec3 vectorAbs(vec3 v) const;

		public:	
			static CVolume* newFrom(const CVolume* other);

			static CVolume* emptyVolume();

			virtual float size() = 0;

			bool isEmpty() { return (size() == 0.0f); }

			virtual bool intersect(CVolume* bv) = 0;

			virtual bool isInside(vec3f pt) = 0;


			virtual void tranform(const CMatrix& m) = 0;

			virtual CVolume* scale(vec3 scaleVector) = 0;		
			virtual void scaleSelf(vec3 scaleVector) = 0;
			virtual CVolume* rotate(quat rot) = 0;			
			virtual CVolume* translate(vec3 translateVector) = 0;			
			virtual void translateSelf(vec3 translateVector) = 0;

			

			virtual bool isBox() = 0;

			virtual CVolume* Union(const CVolume* bv) = 0;			
			
			virtual CVolume* intersection(const CVolume* bv) = 0;
			
			virtual void operator= (const CVolume* other) = 0;
		};
	}
}
}
#endif
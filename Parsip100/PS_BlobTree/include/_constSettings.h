#ifndef _CONSTSETTINGS_H
#define _CONSTSETTINGS_H

#define BOUNDING_OCTREE_EXPANSION_FACTOR 0.6f
#define BOUNDING_VOLUME_PRECISION 0.001f
#define FIELD_VALUE_EPSILON 0.001f

#define DEFAULT_ITERATIONS 8

#define ISO_VALUE	    0.5f
#define ISO_DISTANCE	0.454202f
#define ISO_VALUE_EPSILON 0.1f

#define PCM_PROPAGATION_WIDTH 0.5f * ISO_DISTANCE
#define PCM_ATTENUATION 0.5f

#define MAX_FIELD_VALUE 1.0f
#define MIN_FIELD_VALUE 0.0f

#define MAX_NAME_LEN   32

//NORMAL DELTA
const float NORMAL_DELTA = 0.001f;

#define BLOBTREE_DEFINITIONS
typedef enum BlobNodeType{bntPrimCylinder, bntPrimDisc, bntPrimLine, bntPrimPoint, bntPrimRing, bntPrimPolygon,
                          bntPrimCube, bntPrimTriangle, bntPrimCatmullRomCurve, bntPrimSkeleton, bntPrimQuadricPoint,
                          bntPrimHalfPlane, bntPrimNull, bntPrimInstance,
                          bntOpUnion, bntOpIntersect, bntOpDif, bntOpSmoothDif, bntOpBlend, bntOpRicciBlend, bntOpGradientBlend,
                          bntOpFastQuadricPointSet, bntOpPCM,

                          //Unary Nodes
                          bntOpCache, bntOpWarpTwist, bntOpWarpTaper, bntOpWarpBend, bntOpWarpShear, bntOpTexture};

#endif

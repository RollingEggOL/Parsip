#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "CLayerManager.h"
#include "PS_FrameWork/include/_dataTypes.h"
#include "PS_FrameWork/include/PS_FileDirectory.h"
#include "PS_FrameWork/include/PS_DateTime.h"
#include "PS_FrameWork/include/PS_AppConfig.h"
#include "PS_FrameWork/include/PS_ErrorManager.h"

/*
#include "PS_BlobTree/include/CWarpBend.h"
#include "PS_BlobTree/include/CWarpShear.h"
#include "PS_BlobTree/include/CWarpTaper.h"
#include "PS_BlobTree/include/CWarpTwist.h"
#include "PS_BlobTree/include/CAffine.h"
#include "PS_BlobTree/include/CSkeletonPrimitive.h"
*/

#include "PS_BlobTree/include/BlobTreeLibraryAll.h"
#include "PS_BlobTree/include/BlobTreeScripting.h"

/*
Layer will group a set of BlobNodes so that they can be polygonized together.
Layer will also holds the resulting mesh. Attributes of Quality and Polygonization Settings
related to the current group
*/

namespace PS{
namespace BLOBTREE{

	using namespace PS::DATETIMEUTILS;
	using namespace PS::FILESTRINGUTILS;

CLayer::CLayer()
{
	m_lpMesh	 = NULL;		
	m_lpBlobTree = NULL;			
	m_bVisible = true;
	m_revision		 = 0;
	m_idLastBlobNode = 0;
}

CLayer::CLayer(CBlobTree * node)
{
	m_lpMesh     = NULL;	
	m_lpBlobTree = node;	
	m_bVisible = true;
	m_revision		 = 0;
	m_idLastBlobNode = 0;
}

CLayer::CLayer(const CMeshVV& mesh)
{
	setMesh(mesh);
	m_lpBlobTree = NULL;			
	m_bVisible = true;
	m_revision		 = 0;
	m_idLastBlobNode = 0;
}

CLayer::~CLayer()
{	
	cleanup();
}

void CLayer::cleanup()
{
	selRemoveItem();
	m_lstQuery.clear();
	m_lstSelected.clear();	
	removeAllSeeds();	

	m_polygonizer.removeAllMPUs();
	SAFE_DELETE(m_lpBlobTree);	
	SAFE_DELETE(m_lpMesh);	
}


int CLayer::queryBlobTree(bool bIncludePrims, bool bIncludeOps)
{
	if((!bIncludePrims) && (!bIncludeOps)) return 0;
	m_lstQuery.clear();
	return recursive_QueryBlobTree(bIncludePrims, bIncludeOps, m_lpBlobTree);
}

int CLayer::queryHitOctree(const CRay& ray, float t0, float t1) const
{
	COctree aOctree;

	int idxClosest = -1;
	float min_dist = FLT_MAX;
	float d;
	//Return closest query item
	for(size_t i=0; i<m_lstQuery.size(); i++)
	{
		aOctree = m_lstQuery[i]->getOctree();
		if(aOctree.intersect(ray, t0, t1))
		{	
			d = ray.start.distance(aOctree.center());
			if(d < min_dist) 
			{
				idxClosest = (int)i;
				min_dist = d;
			}			
		}
	}

	return idxClosest;
}
//==================================================================
CBlobTree* CLayer::selGetItem(int index) const
{
	if(m_lstSelected.isItemIndex(index))
		return static_cast<CBlobTree*>(m_lstSelected[index].first);
	else
		return NULL;
}
//==================================================================
CMeshVV* CLayer::selGetMesh(int index) const
{
	if(m_lstSelected.isItemIndex(index))
		return static_cast<CMeshVV*>(m_lstSelected[index].second);
	else
		return NULL;
}
//==================================================================
void CLayer::selRemoveItem(int index)
{
	if(index == -1)
	{
		for(size_t i=0; i<m_lstSelected.size(); i++)
		{
			CMeshVV* lpMesh = m_lstSelected[i].second;
			SAFE_DELETE(lpMesh);
		}
		m_lstSelected.clear();
	}
	else if(m_lstSelected.isItemIndex(index))
	{		
		CMeshVV* lpMesh  =  m_lstSelected[index].second;
		SAFE_DELETE(lpMesh);
		m_lstSelected.remove(index);
	}
}
//==================================================================
bool CLayer::selAddItem(CBlobTree* lpNode)
{
	if(lpNode == NULL) return false;

	CMeshVV* lpMesh = new CMeshVV();

	//Run Polygonizer and export mesh for selection
	bool bres = Run_PolygonizerExportMesh(lpNode, lpMesh, 0.2f, ISO_VALUE - 0.2f);
	PAIR_NODEMESH entry(lpNode, lpMesh);
	m_lstSelected.push_back(entry);
	return bres;
}
//==================================================================
bool CLayer::hasMesh() const
{
	if(m_lpMesh)
		if(m_lpMesh->countFaces() > 0)
			return true;
	return false;
}

void CLayer::removeAllSeeds()
{
	m_lstSeeds.resize(0);	
}

bool CLayer::getSeed(size_t index, PAIR_NODESEED &seed)
{
	if((index >=0)&&(index < m_lstSeeds.size()))
	{
		seed = m_lstSeeds[index];
		return true;
	}
	else return false;
}

size_t CLayer::countSeedPoints()
{
	return m_lstSeeds.size();
}

vec3 CLayer::getSeed(size_t index)
{
	if((index >=0)&&(index < m_lstSeeds.size()))
		return static_cast<vec3>(m_lstSeeds[index].second);
	else
		throw "Invalid index passed to fetch a seedpoint";
		//return vec3(0.0f, 0.0f, 0.0f);
}

size_t CLayer::getAllSeeds(size_t bufLen, vec3 arrSeeds[])
{
	size_t ctTotal = m_lstSeeds.size();
	if(bufLen < ctTotal) return -1;
	for(size_t i=0; i< ctTotal; i++)
		arrSeeds[i] = m_lstSeeds[i].second;
	return ctTotal;
}

size_t CLayer::getAllSeeds(DVec<vec3>& lstAllSeeds)
{
	size_t ctTotal = m_lstSeeds.size();	
	lstAllSeeds.resize(ctTotal);
	for(size_t i=0; i< ctTotal; i++)
		lstAllSeeds[i] = m_lstSeeds[i].second;
	return ctTotal;
}


void CLayer::setOctreeFromBlobTree()
{
	if(m_lpBlobTree == NULL) return;

	CMatrix mtx;
	recursive_RecomputeAllOctrees(m_lpBlobTree, mtx);
	m_octree = m_lpBlobTree->getOctree();
}

CBlobTree* CLayer::recursive_FindNodeByID(int id, CBlobTree* root)
{
	if(root == NULL) return NULL;

	if(root->getID() == id)
		return root;

	if(root->isOperator())
	{
		for(size_t i=0; i<root->countChildren(); i++)
		{
			CBlobTree* found = recursive_FindNodeByID(id, root->getChild(i));
			if(found != NULL) 
				return found;
		}
	}

	return NULL;
}

int CLayer::recursive_MaxNodeID(int maxID, CBlobTree* root)
{
	if(root == NULL)
		return maxID;

	if(root->getID() > maxID)
		maxID = root->getID();

	if(root->isOperator())
	{
		for(size_t i=0; i<root->countChildren(); i++)
		{
			int curID = recursive_MaxNodeID(maxID, root->getChild(i));
			if(curID > maxID) 
				maxID = curID;
		}
	}

	return maxID;
}

bool CLayer::recursive_ExecuteCmdBlobtreeNode(CBlobTree* root, 
	 										  CBlobTree* lpQueryNode, 
											  cmdBlobTree command, 
											  CmdBlobTreeParams* lpParam)
{
	if(root == NULL) return false;
	if((command != cbtDelete)&&(lpParam == NULL)) return false;

	//If query node is the root
	if(root == lpQueryNode)
	{
		if(command == cbtDelete)		
		{
			SAFE_DELETE(lpQueryNode);
		}
		else if(command == cbtTransformOperator)
		{
			if(lpParam->lpReplacementNode)
			{
				CBlobTree* replacement = lpParam->lpReplacementNode;
				//Prepare Replacement
				replacement->addChild(root->getChildren());
				replacement->setID(root->getID());

				//Replace now
				root->setDeleteChildrenUponCleanup(false);
				SAFE_DELETE(root);
				
				this->setBlob(replacement);
				return true;
			}

		}
		else 
		{		
			if(lpParam)
			{
				lpParam->lpOutParent = NULL;
				lpParam->depth = 0;		
				lpParam->idxChild = -1;
			}
		}
		return true;
	}
	
	/////////////////////////////////////////////////
	if(root->isOperator())
	{
		size_t ctKids = root->countChildren();
		if(lpParam) 
			lpParam->depth++;

		//Try current children
		CBlobTree* kid = NULL;
		for(size_t i=0; i<ctKids; i++)
		{
			kid = root->getChild(i);
			if(kid != lpQueryNode)
				continue;

			//Switch
			switch(command)
			{
			case cbtDelete:
				{
					root->removeChild(i);
					return true;
				}
				break;
			case cbtFindParent:
				{
					if(lpParam)
					{	
						lpParam->lpOutParent = root;
						lpParam->idxChild = i;
						return true;
					}
				}
				break;
			case cbtTransformOperator:
				{
					if(lpParam->lpReplacementNode)
					{
						CBlobTree* replacement = lpParam->lpReplacementNode;
						//Prepare Replacement
						replacement->addChild(kid->getChildren());
						replacement->setID(kid->getID());

						//Replace now
						kid->setDeleteChildrenUponCleanup(false);
						root->removeChild(i);
						root->addChild(replacement);
						return true;
					}
					break;
				}

			}
		}//End For

		//Recurse
		for(size_t i=0; i<ctKids; i++)
		{
			if(recursive_ExecuteCmdBlobtreeNode(root->getChild(i), lpQueryNode, command, lpParam) == true)
				return true;
		}		
	}

	return false;
}

void CLayer::recursive_FlattenTransformations(CBlobTree* node, const CAffineTransformation& transformBranch)
{
	if(node == NULL) return;
	if(node->isOperator())
	{		
		CAffineTransformation t = transformBranch;
		t.add(node->getTransform());
		node->getTransform().init();

		for(size_t i=0; i<node->countChildren(); i++)
		{
			recursive_FlattenTransformations(node->getChild(i), t);
		}		
	}
	else
	{
		CAffineTransformation t = transformBranch;
		t.add(node->getTransform());
		node->getTransform().set(t);
	}
}

void CLayer::recursive_RecomputeAllOctrees(CBlobTree* node, const CMatrix& mtxBranch)
{
	if(node == NULL) return;
	if(node->isOperator())
	{		
		CMatrix mtxOriginal = mtxBranch;
		mtxOriginal.multiply(node->getTransform().getForwardMatrix());

		for(size_t i=0; i<node->countChildren(); i++)
		{
			recursive_RecomputeAllOctrees(node->getChild(i), mtxOriginal);
		}

		node->computeOctree();
	}
	else
	{
		COctree oct = node->computeOctree();		
		oct.transform(mtxBranch);
		node->setOctree(oct.lower, oct.upper);
	}
}

int CLayer::recursive_QueryBlobTree(bool bIncludePrim, bool bIncludeOps, CBlobTree* node)
{
	if(node)
	{
		if(node->isOperator())
		{
			int res = 0;
			CBlobTree* kid = NULL;
			size_t ctKids = node->countChildren();			
			if(bIncludeOps)
			{				
				m_lstQuery.push_back(node);
			}
			
			for(size_t i=0; i<ctKids; i++)
			{
				kid = node->getChild(i);				
				res += recursive_QueryBlobTree(bIncludePrim, bIncludeOps, kid);
			}
			return res;
		}
		else
		{			
			if(bIncludePrim)
			{				
				m_lstQuery.push_back(node);
			}
			return 1;
		}
	}
	else 
		return -1;

}

int CLayer::recursive_WriteBlobNode(CSketchConfig* cfg, CBlobTree* node, int idOffset)
{
	if(cfg == NULL) return -1;
	if(node == NULL) return -1;

	DAnsiStr strNodeName;
	char chrName[MAX_NAME_LEN];	

	if(node->isOperator())
	{
		size_t ctKids = node->countChildren();
		
		//First write current operator
		//Use the same id passed in as parameter for writing the operator
		strNodeName = printToAStr("BLOBNODE %d", node->getID() + idOffset);
		node->getName(chrName);

		cfg->writeBool(strNodeName, "IsOperator", node->isOperator());				
		cfg->writeString(strNodeName, "OperatorType", DAnsiStr(chrName));
		cfg->writeInt(strNodeName, "ChildrenCount", ctKids);			

		//Writing Transformation
		cfg->writeVec3f(strNodeName, "AffineScale", node->getTransform().getScale());
		cfg->writeVec4f(strNodeName, "AffineRotate", node->getTransform().getRotationVec4());
		cfg->writeVec3f(strNodeName, "AffineTranslate", node->getTransform().getTranslate());

		//First write all the children		
		vector<int> arrayInt;		
		for(size_t i=0; i<ctKids; i++)
		{
			arrayInt.push_back(node->getChild(i)->getID());			
			recursive_WriteBlobNode(cfg, node->getChild(i), idOffset);			
		}

		cfg->writeIntArray(strNodeName, "ChildrenIDs", arrayInt);
		arrayInt.clear();

		//Write Operator specific properties
		switch(node->getNodeType())
		{
		case(bntOpRicciBlend):
			{
				CRicciBlend* ricci = dynamic_cast<CRicciBlend*>(node);
				cfg->writeFloat(strNodeName, "power", ricci->getN());
				break;
			}
		case(bntOpWarpTwist):
			{
				CWarpTwist* twist = dynamic_cast<CWarpTwist*>(node);
				cfg->writeFloat(strNodeName, "factor", twist->getWarpFactor());
				cfg->writeInt(strNodeName, "axis", static_cast<int>(twist->getMajorAxis()));
				break;
			}
		case(bntOpWarpTaper):
			{
				CWarpTaper* taper = dynamic_cast<CWarpTaper*>(node);
				cfg->writeFloat(strNodeName, "factor", taper->getWarpFactor());
				cfg->writeInt(strNodeName, "base axis", static_cast<int>(taper->getAxisAlong()));
				cfg->writeInt(strNodeName, "taper axis", static_cast<int>(taper->getAxisTaper()));
				break; 
			}
		case(bntOpWarpBend):
			{
				CWarpBend* bend = dynamic_cast<CWarpBend*>(node);
				cfg->writeFloat(strNodeName, "rate", bend->getBendRate());
				cfg->writeFloat(strNodeName, "center", bend->getBendCenter());
				cfg->writeFloat(strNodeName, "left bound", bend->getBendRegion().left);
				cfg->writeFloat(strNodeName, "right bound", bend->getBendRegion().right);
				break;
			}
		case(bntOpWarpShear):
			{
				CWarpShear* shear = dynamic_cast<CWarpShear*>(node);
				cfg->writeFloat(strNodeName, "factor", shear->getWarpFactor());
				cfg->writeInt(strNodeName, "base axis", static_cast<int>(shear->getAxisAlong()));
				cfg->writeInt(strNodeName, "shear axis", static_cast<int>(shear->getAxisDependent()));
				break;
			}
		}

		return ctKids;
	}
	else
	{				
		strNodeName = printToAStr("BLOBNODE %d", node->getID() + idOffset);		

		CSkeletonPrimitive* sprim = dynamic_cast<CSkeletonPrimitive*>(node);
			
		//General per each prim
		cfg->writeBool(strNodeName, "IsOperator", sprim->isOperator());
		//Write skeleton Name		
		sprim->getSkeleton()->getName(chrName);		
		cfg->writeString(strNodeName, "SkeletonType", DAnsiStr(chrName));

		//cfg->writeInt(strNodeName, "FieldFunctionType", sprim->getFieldFunction()->getType());
		//cfg->writeFloat(strNodeName, "FieldScale", sprim->getScale());
		//cfg->writeFloat(strNodeName, "Range", sprim->getRange());
		cfg->writeVec3f(strNodeName, "AffineScale", sprim->getTransform().getScale());
		cfg->writeVec4f(strNodeName, "AffineRotate", sprim->getTransform().getRotationVec4());
		cfg->writeVec3f(strNodeName, "AffineTranslate", sprim->getTransform().getTranslate());

		CMaterial m = sprim->baseMaterial(vec3f(0.0f, 0.0f, 0.0f));
		cfg->writeVec4f(strNodeName, "MtrlAmbient", m.ambient);
		cfg->writeVec4f(strNodeName, "MtrlDiffused", m.diffused);
		cfg->writeVec4f(strNodeName, "MtrlSpecular", m.specular);
		cfg->writeFloat(strNodeName, "MtrlShininess", m.shininess);


		switch(sprim->getSkeleton()->getType())
		{
		case(sktPoint):
			{
				CSkeletonPoint* skeletPoint = dynamic_cast<CSkeletonPoint*>(sprim->getSkeleton());			
				cfg->writeVec3f(strNodeName, "position", skeletPoint->getPosition());
			}
			break;
		case(sktLine):
			{			
				CSkeletonLine* skeletLine = dynamic_cast<CSkeletonLine*>(sprim->getSkeleton());			
				cfg->writeVec3f(strNodeName, "start", skeletLine->getStartPosition());
				cfg->writeVec3f(strNodeName, "end", skeletLine->getEndPosition());
			}
			break;			
		case(sktRing):
			{			
				CSkeletonRing* skeletRing = dynamic_cast<CSkeletonRing*>(sprim->getSkeleton());			
				cfg->writeVec3f(strNodeName, "position", skeletRing->getPosition());
				cfg->writeVec3f(strNodeName, "direction", skeletRing->getDirection());
				cfg->writeFloat(strNodeName, "radius", skeletRing->getRadius());
			}
			break;
		case(sktDisc):
			{			
				CSkeletonDisc* skeletDisc = dynamic_cast<CSkeletonDisc*>(sprim->getSkeleton());			
				cfg->writeVec3f(strNodeName, "position", skeletDisc->getPosition());
				cfg->writeVec3f(strNodeName, "direction", skeletDisc->getDirection());
				cfg->writeFloat(strNodeName, "radius", skeletDisc->getRadius());
			}
			break;
		case(sktCylinder):
			{			
				CSkeletonCylinder* skeletCyl = dynamic_cast<CSkeletonCylinder*>(sprim->getSkeleton());
				cfg->writeVec3f(strNodeName, "position", skeletCyl->getPosition());
				cfg->writeVec3f(strNodeName, "direction", skeletCyl->getDirection());
				cfg->writeFloat(strNodeName, "radius", skeletCyl->getRadius());
				cfg->writeFloat(strNodeName, "height", skeletCyl->getHeight());
			}
			break;			

		case(sktCube):
			{			
				CSkeletonCube* skeletCube = dynamic_cast<CSkeletonCube*>(sprim->getSkeleton());
				cfg->writeVec3f(strNodeName, "position", skeletCube->getPosition());
				cfg->writeFloat(strNodeName, "side", skeletCube->getSide());
			}
			break;	
		case(sktTriangle):
			{
				CSkeletonTriangle* skeletTriangle = dynamic_cast<CSkeletonTriangle*>(sprim->getSkeleton());
				cfg->writeVec3f(strNodeName, "corner0", skeletTriangle->getTriangleCorner(0));
				cfg->writeVec3f(strNodeName, "corner1", skeletTriangle->getTriangleCorner(1));
				cfg->writeVec3f(strNodeName, "corner2", skeletTriangle->getTriangleCorner(2));
			}

		}

		return 1;
	}
}

//////////////////////////////////////////////////////////////////////////
int CLayer::recursive_ReadBlobNode(CSketchConfig* cfg, CBlobTree* parent, int id, int idOffset)
{
	if(cfg == NULL) return 0;	

	DAnsiStr strNodeName = printToAStr("BLOBNODE %d", id);	
	
	if(cfg->hasSection(strNodeName) < 0) 
		return 0;
	
	//Read Node Transformation First
	vec3f affScale = cfg->readVec3f(strNodeName, "AffineScale");
	vec4f affRotate = cfg->readVec4f(strNodeName, "AffineRotate");
	quat quatRotation(affRotate);
	vec3f affTranslate = cfg->readVec3f(strNodeName, "AffineTranslate");	

	bool bOperator = cfg->readBool(strNodeName, "IsOperator", false);
	if(bOperator)
	{
		vector<int> arrayInt;
		DAnsiStr strOp = cfg->readString(strNodeName, "OperatorType");
		strOp.toUpper();


		int ctKids = cfg->readInt(strNodeName, "ChildrenCount", 0);
		cfg->readIntArray(strNodeName, "ChildrenIDs", ctKids, arrayInt);
		CBlobTree* opNode = NULL;

		//typedef enum BlobNodeType{bntPrimitive, bntOpUnion, bntOpIntersect, bntOpDif, bntOpSmoothDif, bntOpBlend, bntOpRicciBlend,
		//	bntOpAffine, bntOpWarpTwist, bntOpWarpTaper, bntOpWarpBend, bntOpWarpShear, bntOpCache, bntOpTexture, bntOpPCM};

		if(strOp == PS_UNION)
		{
			opNode = new CUnion();
		}
		else if(strOp == PS_BLEND)
		{
			opNode = new CBlend();
		}
		else if(strOp == PS_RICCIBLEND)
		{
			float power = cfg->readFloat(strNodeName, "power", 1.0f);
			opNode = new CRicciBlend(power);
			
		}
		else if(strOp == PS_INTERSECTION)
		{
			opNode = new CIntersection();
		}
		else if(strOp == PS_DIFFERENCE)
		{
			opNode = new CDifference();
		}
		else if(strOp == PS_SMOOTHDIF)
		{
			opNode = new CSmoothDifference();
		}
		else if(strOp == PS_AFFINE)
		{
			opNode = new CAffine();
		}
		else if(strOp == PS_WARPTWIST)
		{
			float factor = cfg->readFloat(strNodeName, "factor", 1.0f);
			MajorAxices axis = static_cast<MajorAxices>(cfg->readInt(strNodeName, "axis", 2));
			opNode = new CWarpTwist(factor, axis);
		}
		else if(strOp == PS_WARPTAPER)
		{
			float factor = cfg->readFloat(strNodeName, "factor", 1.0f);
			MajorAxices axisBase = static_cast<MajorAxices>(cfg->readInt(strNodeName, "base axis", 0));
			MajorAxices axisTaper = static_cast<MajorAxices>(cfg->readInt(strNodeName, "taper axis", 2));

			opNode = new CWarpTaper(factor, axisTaper, axisBase);
		}
		else if(strOp == PS_WARPBEND)
		{			
			float rate	 = cfg->readFloat(strNodeName, "rate", 1.0f);
			float center = cfg->readFloat(strNodeName, "center", 0.5f);
			float lbound = cfg->readFloat(strNodeName, "left bound", 0.0f);
			float rbound = cfg->readFloat(strNodeName, "right bound", 1.0f);
			
			opNode = new CWarpBend(rate, center, lbound, rbound);
		}
		else if(strOp == PS_WARPSHEAR)
		{
			float factor = cfg->readFloat(strNodeName, "factor", 1.0f);
			MajorAxices axisBase  = static_cast<MajorAxices>(cfg->readInt(strNodeName, "base axis", 0));
			MajorAxices axisShear = static_cast<MajorAxices>(cfg->readInt(strNodeName, "shear axis", 2));

			opNode = new CWarpShear(factor, axisBase, axisShear);
		}
		else if(strOp == PS_CACHE)
		{
			opNode = new CFieldCache();
		}
		else if(strOp == PS_PCM)
		{
			opNode = new CPcm();
		}
		else
		{
			DAnsiStr strMsg = printToAStr("[BlobTree Script] Unknown operator %s: [%s]", strOp.ptr());
			ReportError(strMsg.ptr());
			FlushAllErrors();

			SAFE_DELETE(cfg);
			SAFE_DELETE(parent);

			return false;
		}

		int res = 0;
		opNode->setID(id - idOffset);
		//Writing Transformation
		opNode->getTransform().set(affScale, quatRotation, affTranslate);

		//Find all the children for this operator		
		for(size_t i=0; i<arrayInt.size(); i++)		
			res += recursive_ReadBlobNode(cfg, opNode, arrayInt[i], idOffset);		

		arrayInt.clear();

		if(parent == NULL)
			m_lpBlobTree = opNode;		
		else
			parent->addChild(opNode);
		return res+1;
	}
	else
	{
		DAnsiStr strSkelet = cfg->readString(strNodeName, "SkeletonType");

		CMaterial mtrl;
		mtrl.ambient   = cfg->readVec4f(strNodeName, "MtrlAmbient");
		mtrl.diffused  = cfg->readVec4f(strNodeName, "MtrlDiffused");
		mtrl.specular  = cfg->readVec4f(strNodeName, "MtrlSpecular");
		mtrl.shininess = cfg->readFloat(strNodeName, "MtrlShininess");


		CSkeleton* skelet = NULL;
		if(strSkelet == PS_POINT)
		{
			vec3f pos = cfg->readVec3f(strNodeName, "position");
			skelet = new CSkeletonPoint(pos);			
		}
		else if(strSkelet == PS_LINE)
		{
			vec3f s = cfg->readVec3f(strNodeName, "start");
			vec3f e = cfg->readVec3f(strNodeName, "end");
			skelet = new CSkeletonLine(s, e);			
		}
		else if(strSkelet == PS_RING)
		{
			vec3f c = cfg->readVec3f(strNodeName, "position");
			vec3f d = cfg->readVec3f(strNodeName, "direction");
			float r = cfg->readFloat(strNodeName, "radius");
			skelet = new CSkeletonRing(c, d, r);			
		}
		else if(strSkelet == PS_DISC)
		{
			vec3f c = cfg->readVec3f(strNodeName, "position");
			vec3f d = cfg->readVec3f(strNodeName, "direction");
			float r = cfg->readFloat(strNodeName, "radius");
			skelet = new CSkeletonDisc(c, d, r);			
		}
		else if(strSkelet == PS_CYLINDER)
		{			
			vec3f p = cfg->readVec3f(strNodeName, "position");
			vec3f d = cfg->readVec3f(strNodeName, "direction");
			float r = cfg->readFloat(strNodeName, "radius");
			float h = cfg->readFloat(strNodeName, "height");
			skelet = new CSkeletonCylinder(p, d, r, h);			
		}
		else if(strSkelet == PS_CUBE)
		{			
			vec3f p = cfg->readVec3f(strNodeName, "position");
			float s = cfg->readFloat(strNodeName, "side");
			skelet = new CSkeletonCube(p, s);
		}
		else if(strSkelet == PS_TRIANGLE)
		{			
			vec3f a = cfg->readVec3f(strNodeName, "corner0");
			vec3f b = cfg->readVec3f(strNodeName, "corner1");
			vec3f c = cfg->readVec3f(strNodeName, "corner2");			
			skelet = new CSkeletonTriangle(a, b, c);
		}
		else
		{
			DAnsiStr strMsg = printToAStr("[BlobTree Script] Unknown primitive type: [%s]", strSkelet.ptr());
			ReportError(strMsg.ptr());
			FlushAllErrors();

			return 0;
		}

		CSkeletonPrimitive* sprim = new CSkeletonPrimitive(skelet, fftWyvill, 1.0f);
		sprim->setID(id - idOffset);
		sprim->getTransform().set(affScale, quatRotation, affTranslate);
		sprim->setMaterial(mtrl);
		sprim->setColor(mtrl.diffused);

		if(parent == NULL)
			m_lpBlobTree = sprim;		
		else
			parent->addChild(sprim);
		return 1;
	}

}

int CLayer::recursive_ReadBlobNodeV0( CSketchConfig* cfg, CBlobTree* parent, int id, int& idIncremental )
{
	if(cfg == NULL) return 0;	

	DAnsiStr strNodeName = printToAStr("BLOBNODE %d", id);	

	if(cfg->hasSection(strNodeName) < 0) 
		return 0;

	bool bOperator = cfg->readBool(strNodeName, "IsOperator", false);
	if(bOperator)
	{
		vector<int> arrayInt;
		DAnsiStr strOp = cfg->readString(strNodeName, "OperatorType");
		strOp.toUpper();

		int ctKids = cfg->readInt(strNodeName, "ChildrenCount", 0);
		cfg->readIntArray(strNodeName, "ChildrenIDs", ctKids, arrayInt);
		CBlobTree* opNode = NULL;

		//typedef enum BlobNodeType{bntPrimitive, bntOpUnion, bntOpIntersect, bntOpDif, bntOpSmoothDif, bntOpBlend, bntOpRicciBlend,
		//	bntOpAffine, bntOpWarpTwist, bntOpWarpTaper, bntOpWarpBend, bntOpWarpShear, bntOpCache, bntOpTexture, bntOpPCM};

		if(strOp == PS_UNION)
		{
			opNode = new CUnion();
		}
		else if(strOp == PS_BLEND)
		{
			opNode = new CBlend();
		}
		else if(strOp == PS_RICCIBLEND)
		{
			float power = cfg->readFloat(strNodeName, "power", 1.0f);
			opNode = new CRicciBlend(power);

		}
		else if(strOp == PS_INTERSECTION)
		{
			opNode = new CIntersection();
		}
		else if(strOp == PS_DIFFERENCE)
		{
			opNode = new CDifference();
		}
		else if(strOp == PS_SMOOTHDIF)
		{
			opNode = new CSmoothDifference();
		}
		else if(strOp == PS_AFFINE)
		{
			opNode = new CAffine();
		}
		else if(strOp == PS_WARPTWIST)
		{
			float factor = cfg->readFloat(strNodeName, "factor", 1.0f);
			MajorAxices axis = static_cast<MajorAxices>(cfg->readInt(strNodeName, "axis", 2));
			opNode = new CWarpTwist(factor, axis);
		}
		else if(strOp == PS_WARPTAPER)
		{
			float factor = cfg->readFloat(strNodeName, "factor", 1.0f);
			MajorAxices axisBase = static_cast<MajorAxices>(cfg->readInt(strNodeName, "base axis", 0));
			MajorAxices axisTaper = static_cast<MajorAxices>(cfg->readInt(strNodeName, "taper axis", 2));

			opNode = new CWarpTaper(factor, axisTaper, axisBase);
		}
		else if(strOp == PS_WARPBEND)
		{			
			float rate	 = cfg->readFloat(strNodeName, "rate", 1.0f);
			float center = cfg->readFloat(strNodeName, "center", 0.5f);
			float lbound = cfg->readFloat(strNodeName, "left bound", 0.0f);
			float rbound = cfg->readFloat(strNodeName, "right bound", 1.0f);

			opNode = new CWarpBend(rate, center, lbound, rbound);
		}
		else if(strOp == PS_WARPSHEAR)
		{
			float factor = cfg->readFloat(strNodeName, "factor", 1.0f);
			MajorAxices axisBase  = static_cast<MajorAxices>(cfg->readInt(strNodeName, "base axis", 0));
			MajorAxices axisShear = static_cast<MajorAxices>(cfg->readInt(strNodeName, "shear axis", 2));

			opNode = new CWarpShear(factor, axisBase, axisShear);
		}
		else if(strOp == PS_CACHE)
		{
			opNode = new CFieldCache();
		}
		else if(strOp == PS_PCM)
		{
			opNode = new CPcm();
		}
		else
		{
			DAnsiStr strMsg = printToAStr("[BlobTree Script] Unknown operator %s: [%s]", strOp.ptr());
			ReportError(strMsg.ptr());
			FlushAllErrors();

			SAFE_DELETE(cfg);
			SAFE_DELETE(parent);

			return false;
		}

		int res = 0;
		opNode->setID(idIncremental);
		idIncremental++;
		//Find all the children for this operator		
		for(size_t i=0; i<arrayInt.size(); i++)		
			res += recursive_ReadBlobNodeV0(cfg, opNode, arrayInt[i], idIncremental);		

		arrayInt.clear();

		if(parent == NULL)
			m_lpBlobTree = opNode;		
		else
			parent->addChild(opNode);
		return res+1;
	}
	else
	{
		DAnsiStr strSkelet = cfg->readString(strNodeName, "SkeletonType");
		vec3f affScale = cfg->readVec3f(strNodeName, "AffineScale");
		vec4f affRotate = cfg->readVec4f(strNodeName, "AffineRotate");
		quat quatRotation(affRotate);

		vec3f affTranslate = cfg->readVec3f(strNodeName, "AffineTranslate");	
		CMaterial mtrl;
		mtrl.ambient   = cfg->readVec4f(strNodeName, "MtrlAmbient");
		mtrl.diffused  = cfg->readVec4f(strNodeName, "MtrlDiffused");
		mtrl.specular  = cfg->readVec4f(strNodeName, "MtrlSpecular");
		mtrl.shininess = cfg->readFloat(strNodeName, "MtrlShininess");


		CSkeleton* skelet = NULL;
		if(strSkelet == PS_POINT)
		{
			vec3f pos = cfg->readVec3f(strNodeName, "position");
			skelet = new CSkeletonPoint(pos);			
		}
		else if(strSkelet == PS_LINE)
		{
			vec3f s = cfg->readVec3f(strNodeName, "start");
			vec3f e = cfg->readVec3f(strNodeName, "end");
			skelet = new CSkeletonLine(s, e);			
		}
		else if(strSkelet == PS_RING)
		{
			vec3f c = cfg->readVec3f(strNodeName, "position");
			vec3f d = cfg->readVec3f(strNodeName, "direction");
			float r = cfg->readFloat(strNodeName, "radius");
			skelet = new CSkeletonRing(c, d, r);			
		}
		else if(strSkelet == PS_DISC)
		{
			vec3f c = cfg->readVec3f(strNodeName, "position");
			vec3f d = cfg->readVec3f(strNodeName, "direction");
			float r = cfg->readFloat(strNodeName, "radius");
			skelet = new CSkeletonDisc(c, d, r);			
		}
		else if(strSkelet == PS_CYLINDER)
		{			
			vec3f p = cfg->readVec3f(strNodeName, "position");
			vec3f d = cfg->readVec3f(strNodeName, "direction");
			float r = cfg->readFloat(strNodeName, "radius");
			float h = cfg->readFloat(strNodeName, "height");
			skelet = new CSkeletonCylinder(p, d, r, h);			
		}
		else if(strSkelet == PS_CUBE)
		{			
			vec3f p = cfg->readVec3f(strNodeName, "position");
			float s = cfg->readFloat(strNodeName, "side");
			skelet = new CSkeletonCube(p, s);
		}
		else if(strSkelet == PS_TRIANGLE)
		{			
			vec3f a = cfg->readVec3f(strNodeName, "corner0");
			vec3f b = cfg->readVec3f(strNodeName, "corner1");
			vec3f c = cfg->readVec3f(strNodeName, "corner2");			
			skelet = new CSkeletonTriangle(a, b, c);
		}
		else
		{
			DAnsiStr strMsg = printToAStr("[BlobTree Script] Unknown primitive type: [%s]", strSkelet.ptr());
			ReportError(strMsg.ptr());
			FlushAllErrors();

			return 0;
		}

		CSkeletonPrimitive* sprim = new CSkeletonPrimitive(skelet, fftWyvill, 1.0f);
		sprim->setID(idIncremental);
		idIncremental++;

		sprim->getTransform().set(affScale, quatRotation, affTranslate);
		sprim->setMaterial(mtrl);
		sprim->setColor(mtrl.diffused);


		if(parent == NULL)
			m_lpBlobTree = sprim;		
		else
			parent->addChild(sprim);
		return 1;
	}
}


int CLayer::recursive_GetBlobTreeSeedPoints(CBlobTree* node, stack<CBlobTree*> &stkOperators)
{		
	if(node)
	{
		if(node->isOperator())
		{
			CBlobTree* kid = NULL;
			size_t ctKids = 0;

			stkOperators.push(node);
			ctKids = node->countChildren();
			int res = 0;
					
			
			for(size_t i=0; i<ctKids; i++)
			{
				kid = node->getChild(i);
				stack<CBlobTree*> stkLocal(stkOperators);
				res += recursive_GetBlobTreeSeedPoints(kid, stkLocal);
			}
			return res;
		}
		else
		{
			//It is an Skeletal Primitive
			CSkeletonPrimitive * prim = dynamic_cast<CSkeletonPrimitive*>(node);			
			vec3 seed = prim->getPolySeedPoint();
			
			CBlobTree* op = NULL;

			//Apply All operators on the seed point
			while(!stkOperators.empty())
			{
				op = stkOperators.top();
				stkOperators.pop();

				seed = op->getTransform().applyForwardTransform(seed);

				switch(op->getNodeType())
				{
				case(bntOpAffine):					
					seed = dynamic_cast<CAffine*>(op)->applyForwardTransform(seed);
					break;
				case(bntOpWarpTwist):
					seed = dynamic_cast<CWarpTwist*>(op)->warp(seed);
					break;
				case(bntOpWarpTaper):				
					seed = dynamic_cast<CWarpTaper*>(op)->warp(seed);
					break;
				case(bntOpWarpBend):					
					seed = dynamic_cast<CWarpBend*>(op)->warp(seed);
					break;
				case(bntOpWarpShear):					
					seed = dynamic_cast<CWarpShear*>(op)->warp(seed);					
					break;
                                default:
                                {

                                }

				}
			}

			//Add seed point to list of seeds
			PAIR_NODESEED MyNewPair(node, seed); 
			m_lstSeeds.push_back(MyNewPair);
			return 1;
		}
	}
	else 
		return -1;
}

//Finds all Seed Points
bool CLayer::setPolySeedPointAuto()
{
	removeAllSeeds();

	stack<CBlobTree*> stkOperators;	
	CBlobTree *node = getBlob();
	int ctRes = recursive_GetBlobTreeSeedPoints(node, stkOperators);	 
	if(ctRes > 0)
	{
		PAIR_NODESEED e = m_lstSeeds[0];
		m_ptStart = e.second;
		return true;
	}
	else
		return false;
}

int CLayer::recursive_TranslateSkeleton(CBlobTree* node, vec3f d)
{
	if(node == NULL) return 0;
	
	if(node->isOperator())
	{
		CBlobTree* kid = NULL;
		size_t ctKids = node->countChildren();
		int res = 0;

		for(size_t i=0; i<ctKids; i++)
		{
			kid = node->getChild(i);				
			res += recursive_TranslateSkeleton(kid, d);
		}
		return res;
	}
	else
	{
		//It is an Skeletal Primitive
		CSkeletonPrimitive * prim = dynamic_cast<CSkeletonPrimitive*>(node);
		prim->getSkeleton()->translate(d);
		return 1;
	}
}

int CLayer::skeletTranslate(vec3f d)
{
	return recursive_TranslateSkeleton(m_lpBlobTree, d);
}

void CLayer::setOctree(vec3 lower, vec3 upper)
{	
	m_octree.lower = lower;
	m_octree.upper = upper;
}

void CLayer::setOctreeFromMesh()
{	
	if(m_lpMesh == NULL) return;
	vec3 lo, hi;
	m_lpMesh->getExtremes(lo, hi);
	m_octree.lower = lo;
	m_octree.upper = hi;
}

bool CLayer::calcPolyBounds()
{	
	if(m_lpBlobTree == NULL) return false;
	if(m_polyCellSize <= 0.0f) return false;

	COctree oct = m_lpBlobTree->getOctree();
	m_polyBounds.x = static_cast<int>(ceil((oct.upper.x - oct.lower.x) / m_polyCellSize));
	m_polyBounds.y = static_cast<int>(ceil((oct.upper.y - oct.lower.y) / m_polyCellSize));
	m_polyBounds.z = static_cast<int>(ceil((oct.upper.z - oct.lower.z) / m_polyCellSize));
	
	return true;
}


void CLayer::setMesh()
{
	if(m_lpMesh == NULL)
		m_lpMesh = new CMeshVV();
	else
		m_lpMesh->removeAll();
}

//Copy from another mesh
void CLayer::setMesh(const CMeshVV& other) 
{	
	if(m_lpMesh == NULL)
		m_lpMesh = new CMeshVV(other);
	else
		m_lpMesh->copyFrom(other);
}


//load from file
void CLayer::setMesh(const DAnsiStr& strFileName)
{	
	setMesh();
	m_lpMesh->open(strFileName);
}

bool CLayer::saveAsVolumeData(const char* strFileName, int w, int h, int d)
{

	FILE* stream; 
	int err = fopen_s(&stream, strFileName, "w+");
	if( err != 0 )
		return false;

        U8* buffer = new U8[w*h*d];
	if(saveAsVolumeData(buffer, w, h, d))
	{
                size_t nWritten = fwrite(buffer, sizeof(U8), w*h*d, stream);
		fclose(stream);

		delete [] buffer; buffer = NULL;
		return (nWritten == w*h*d);
	}
	else
	{
		delete [] buffer; buffer = NULL;
		return false;
	}
	
}

bool CLayer::saveAsVolumeData(U8* buffer, int w, int h, int d)
{
	CBlobTree* root = getBlob();	
	if((root == NULL) || (m_lpMesh == NULL) || (buffer == NULL)) 
	{
		
		return false;
	}
	
	
	vec3 lower(0.0f, 0.0f, 0.0f);
	vec3 upper(0.5f, 0.5f, 0.5f);

	m_lpMesh->fitTo(lower, upper);
	m_lpMesh->getExtremes(lower, upper);	

	float stepX = (upper.x - lower.x) / w;
	float stepY = (upper.y - lower.y) / h;
	float stepZ = (upper.z - lower.z) / d;
	vec3 pt;

	for (int i=0; i < w; i++)			
	{	
		for (int j=0; j < h; j++)					
		{
			for (int k=0; k < d; k++)	
			{
				pt = lower + vec3(i*stepX, j*stepY, k*stepZ);
				float fv = root->fieldValue(pt);
				if(fv > 1.0f)
					throw "FieldValue overflow!";
                                U8 val = static_cast<U8>(fv * 255);
				//data[j*d*w + k*w + i] = val;
				//data[k*h*w + j*w + i ] = val;
				buffer[i*h*d + j*d + k] = val;
			}
		}
	}

	return true;
}

DAnsiStr CLayer::getMeshInfo() const
{
	DAnsiStr strMesh;
	if(hasMesh())
		strMesh = PS::printToAStr("F#%i, V#%i", m_lpMesh->countFaces(), m_lpMesh->countVertices());
	else
		strMesh = "Empty";
	return strMesh;
}

void CLayer::getMeshInfo( size_t& ctVertices, size_t& ctFaces )
{
	ctVertices = 0;
	ctFaces = 0;
	if(m_lpMesh)
	{
		ctVertices = m_lpMesh->countVertices();
		ctFaces = m_lpMesh->countFaces();
	}
}

int CLayer::queryGetAllOctrees( DVec<vec3f>& los, DVec<vec3f>& his ) const
{	
	COctree oct;

	size_t ctOctrees = m_lstQuery.size();
	//Resize once for better performance
	los.resize(ctOctrees);
	his.resize(ctOctrees);
	for(size_t i=0; i<ctOctrees; i++)
	{
		oct = m_lstQuery[i]->getOctree();
		los[i] = oct.lower;
		his[i] = oct.upper;
	}
	return (int)ctOctrees;
}

void CLayer::flattenTransformations()
{
	if(m_lpBlobTree == NULL) return;
	CAffineTransformation transformBranch;
	recursive_FlattenTransformations(m_lpBlobTree, transformBranch);
}

CBlobTree* CLayer::findNodeByID( int id )
{
	return recursive_FindNodeByID(id, m_lpBlobTree);
}

int CLayer::recursive_convertToBinaryTree( CBlobTree* node, CBlobTree* clonned )
{
	if ((node == NULL)||(clonned == NULL))
	{
		ReportError("Node or its clonned is NULL.");
		FlushAllErrors();
		return -1;
	}

	if(node->getNodeType() != clonned->getNodeType())
	{
		ReportError("Node and its clone are not identical.");
		FlushAllErrors();
		return -2;
	}

	if(node->isOperator())
	{
		int res = 0;

		//Replace nodes with more than 2 kids
		CBlobTree* clonnedChild = NULL;
		if(node->countChildren() > 2)
		{
			CBlobTree* cur = NULL;
			CBlobTree* prev = NULL;


			for(size_t i=0; i<node->countChildren() -1; i++)
			{
				if(i == 0)
					cur = clonned;
				else
					cur = cloneNode(clonned, this->fetchIncrementLastNodeID());

				//Clone child
				clonnedChild = cloneNode(node->getChild(i), this->fetchIncrementLastNodeID()); 

				//Add clonned child to parent
				cur->addChild(clonnedChild);

				if(prev)
					prev->addChild(cur);
				prev = cur;

				//Recurse to child
				res += recursive_convertToBinaryTree(node->getChild(i), clonnedChild);
			}

			if(prev) 
			{
				clonnedChild = cloneNode(node->getLastChild(), this->fetchIncrementLastNodeID());
				prev->addChild(clonnedChild);

				//Recurse to child
				res += recursive_convertToBinaryTree(node->getLastChild(), clonnedChild);
			}
		}
		else if(node->countChildren() <= 2)
		{
			for(size_t i=0; i<2; i++)
			{
				if(node->getChild(i))
				{
					//Clone child
					clonnedChild = cloneNode(node->getChild(i), this->fetchIncrementLastNodeID());

					//Add clone child to clonned parent
					clonned->addChild(clonnedChild);

					//Recurse to child
					res += recursive_convertToBinaryTree(node->getChild(i), clonnedChild);
				}
				else
				{
					CSkeletonPoint* point = new CSkeletonPoint(vec3f(0.0f, 0.0f, 0.0f));
					clonnedChild = new CSkeletonPrimitive(point);
					clonnedChild->setID(this->fetchIncrementLastNodeID());

					//Add clone child to clonned parent
					clonned->addChild(clonnedChild);

					ReportError("Found a unary operator while processing. Replaced second primitive with a sphere at origin.");
					FlushAllErrors();

					res++;
				}

			}
		}
		return res + 1;
	}
	else
		return 1;
}

int CLayer::recursive_countBinaryTreeErrors( CBlobTree* node )
{
	if(node == NULL) return 0;
	if(node->isOperator())
	{
		int res = 0;
		for (size_t i=0; i<node->countChildren(); i++)
			res += recursive_countBinaryTreeErrors(node->getChild(i));

		if(node->countChildren() > 2) 
			res++;
		else if(node->countChildren() == 1)
		{
			res++;

			char chrName[MAX_NAME_LEN];			
			node->getName(chrName);			
			DAnsiStr strMsg = printToAStr("Found a unary node in tree! Name:%s, ID:%d", chrName, node->getID());
			ReportError(strMsg.ptr());
			FlushAllErrors();
		}

		return res;
	}
	else
		return 0;

}


int CLayer::convertToBinaryTree()
{
	CBlobTree* root = this->getBlob();
	int ctErrors = recursive_countBinaryTreeErrors(root);
	if(ctErrors > 0)
	{
		this->resetLastBlobNodeID();
		CBlobTree* replacement = cloneNode(root, this->fetchIncrementLastNodeID());
		if(recursive_convertToBinaryTree(root, replacement) > 0)
		{
			SAFE_DELETE(root);
			this->setBlob(replacement);
			root = replacement;
		}
		else
		{
			SAFE_DELETE(replacement);
		}

		//Update data-structures
		this->setPolySeedPointAuto();
		this->setOctreeFromBlobTree();
		this->flattenTransformations();
		this->queryBlobTree(true, false);
	}	

	return ctErrors;
}

//////////////////////////////////////////////////////////////////////////
CLayerManager::CLayerManager(CLayer * aLayer)
{
	m_idxActiveLayer = -1;
	m_lstLayers.push_back(aLayer);
}

void CLayerManager::removeAllLayers()
{		
	for(size_t i=0; i< m_lstLayers.size(); i++)
		delete m_lstLayers[i];		
	m_lstLayers.clear();
	m_idxActiveLayer = -1;
}

size_t CLayerManager::countLayers() const {return m_lstLayers.size();}

bool CLayerManager::isLayerIndex(int index) const
{ 
	return ((index >=0)&&(index < (int)m_lstLayers.size()));
}

CLayer* CLayerManager::getLayer(int index) const
{
	return (isLayerIndex(index)) ? m_lstLayers[index] : NULL;
}

CLayer* CLayerManager::getLast() const
{
	int ctSize = (int)m_lstLayers.size();
	if(ctSize > 0)
		return m_lstLayers[ctSize - 1];
	else 
		return NULL;
}

bool CLayerManager::calcPolyBounds()
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		if(!m_lstLayers[i]->calcPolyBounds())
			return false;
	}
	return true;
}

void CLayerManager::setCellSize(float side)
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		m_lstLayers[i]->setCellSize(side);
	}
}

void CLayerManager::setCellShape(CellShape poly)
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		m_lstLayers[i]->setCellShape(poly);
	}
}

void CLayerManager::setPolyBounds(int bounds)
{
	setPolyBounds(vec3i(bounds, bounds, bounds));
}

void CLayerManager::setPolyBounds(vec3i bounds)
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		m_lstLayers[i]->setPolyBounds(bounds);
	}
}

void CLayerManager::setMeshQuality(int quality)
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		m_lstLayers[i]->setMeshQuality(quality);
	}
}

void CLayerManager::addLayer(const CBlobTree* blob, const char* chrLayerName)
{
	addLayer(blob, NULL, chrLayerName);
}

void CLayerManager::addLayer(const CBlobTree* blob, 					  	     
							 const char* chrMeshFileName, 
							 const char* chrLayerName)
{
	CLayer * aLayer = new CLayer(const_cast<CBlobTree*>(blob));	
	aLayer->setCellSize(DEFAULT_CELL_SIZE);
	aLayer->setPolyBounds(DEFAULT_BOUNDS);
	aLayer->setPolySeedPoint(vec3(0.0f, 0.0f, 0.0f));
	aLayer->setVisible(true);

	//Load Mesh
	if(chrMeshFileName != NULL)
	if(PS::FILESTRINGUTILS::FileExists(chrMeshFileName))
		aLayer->setMesh(DAnsiStr(chrMeshFileName));

	//Set Mesh FileName
	if(chrLayerName == NULL)
		aLayer->setGroupName("Blob Layer");
	else
		aLayer->setGroupName(DAnsiStr(chrLayerName));

	addLayer(aLayer);
}

void CLayerManager::addLayer(CLayer * alayer)
{
	if(alayer != NULL) m_lstLayers.push_back(alayer);
}

void CLayerManager::eraseAllMeshes()
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		if(m_lstLayers[i]->hasMesh())
			m_lstLayers[i]->getMesh()->removeAll();
	}
}

void CLayerManager::resetAllMeshes()
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
		m_lstLayers[i]->setMesh();
}

bool CLayerManager::saveAsVolumeData(const char* strDir, int w, int h, int d)
{

	for(size_t i=0; i< m_lstLayers.size(); i++)
	{	
            DAnsiStr strPath = DAnsiStr(strDir) + printToAStr("//LAYERNUM_%d.raw", i);
            if(!m_lstLayers[i]->saveAsVolumeData(strPath.ptr(), w, h, d))
                return false;
	}
	return true;
}
/*
bool CLayerManager::save(QString strFileName)
{
	QFile file(strFileName);
	if(!file.open(QIODevice::WriteOnly)) return false;

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_4);
	return save(out);
}

bool CLayerManager::save(QDataStream &out)
{
	if(out.status() != QDataStream::Ok) return false;
	size_t savedLayers = 0;
	out << m_lstLayers.size();
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		if(m_lstLayers[i]->save(out))
			savedLayers++;
	}

	return (savedLayers == m_lstLayers.size());
}

////////////////////////////////////////////////////////////////
bool CLayerManager::load(QString strFileName)
{
	QFile file(strFileName);
	if(!file.exists()) return false;		
	if(!file.open(QIODevice::ReadOnly)) return false;
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_4);
	return load(in);
}

bool CLayerManager::load(QDataStream &in)
{
	removeAllLayers();
	size_t ctExpected = 0;
	size_t ctLoaded = 0;
	in >> ctExpected;

	CLayer * aLayer;
	for(size_t i=0; i < ctExpected; i++)
	{
		aLayer = new CLayer();
		if(aLayer->load(in))
			ctLoaded++;
		addLayer(aLayer);
	}	

	return(ctExpected == ctLoaded);
}
*/

bool CLayerManager::setPolySeedPointAuto()
{
	for(size_t i=0; i < m_lstLayers.size(); i++)
	{
		if(!getLayer(i)->setPolySeedPointAuto()) 
			return false;
	}
	return true;
}

void CLayerManager::setOctreesFromBlobTree()
{
	for(size_t i=0; i < m_lstLayers.size(); i++)	
		getLayer(i)->setOctreeFromBlobTree();	
}

void CLayerManager::setOctreesFromMeshes()
{
	for(size_t i=0; i < m_lstLayers.size(); i++)	
		getLayer(i)->setOctreeFromMesh();	
}


CLayer* CLayerManager::getActiveLayer() const
{
	if(isLayerIndex(m_idxActiveLayer))
		return m_lstLayers[m_idxActiveLayer];
	else
		return NULL;
}

void CLayerManager::setActiveLayer( int idxLayer )
{
	if(isLayerIndex(idxLayer))
		m_idxActiveLayer = idxLayer;
	else
		m_idxActiveLayer = -1;
}

bool CLayerManager::hasActiveSelOctree() const
{
	CLayer* aLayer = getActiveLayer();
	if(aLayer)
		if(aLayer->selCountItems() > 0)
			return aLayer->selGetItem(0)->getOctree().isValid();
	return false;
}

COctree CLayerManager::getActiveSelOctree() const
{
	COctree result;
	CLayer* aLayer = getActiveLayer();
	if(aLayer)	
		if(aLayer->selCountItems() > 0)
			result = aLayer->selGetItem(0)->getOctree();
	return result;
}

void CLayerManager::removeLayer( int idxLayer )
{
	if(isLayerIndex(idxLayer))
	{
		CLayer* alayer = getLayer(idxLayer);			
		m_lstLayers.erase(m_lstLayers.begin() + idxLayer);				
		SAFE_DELETE(alayer);		
	}
}

size_t CLayerManager::countOctrees() const
{
	size_t ctOcts = 0;
	for(size_t i=0; i<m_lstLayers.size(); i++)
		if(getLayer(i)->hasOctree()) ctOcts++;
	return ctOcts;
}

void CLayerManager::bumpRevisions()
{
	for(size_t i=0; i<m_lstLayers.size(); i++)
		getLayer(i)->bumpRevision();
}

void CLayerManager::resetRevisions()
{
	for(size_t i=0; i<m_lstLayers.size(); i++)
		getLayer(i)->resetRevision();
}

int CLayerManager::hitLayerOctree( const CRay& ray, float t0, float t1 ) const
{
	for(size_t i=0; i<m_lstLayers.size(); i++)
	{
		if(getLayer(i)->hasOctree() && getLayer(i)->isVisible())
		{
			if(getLayer(i)->getOctree().intersect(ray, t0, t1))
			{
				return i;
			}
		}
	}
	
	return -1;		
}

int	CLayerManager::computeAllPrimitiveOctrees()
{
	int res = 0;
	for(size_t i=0; i<m_lstLayers.size(); i++)
		res += getLayer(i)->queryBlobTree(true, false);
	return res;
}

//First hit a layer then if successful hit a primitive within that layer
bool CLayerManager::queryHitOctree(const CRay& ray, float t0, float t1, int& idxLayer, int& idxPrimitive)
{
	idxPrimitive = -1;
	idxLayer = hitLayerOctree(ray, t0, t1);
	if(isLayerIndex(idxLayer))
	{
		idxPrimitive = getLayer(idxLayer)->queryHitOctree(ray, t0, t1);
		if(idxPrimitive >= 0)
			return true;
	}
	return false;
}

void CLayerManager::selRemoveItems()
{	
	for(size_t i=0; i<m_lstLayers.size(); i++)
		getLayer(i)->selRemoveItem(-1);
}

void CLayerManager::setAllVisible( bool bVisible )
{
	for(size_t i=0; i<m_lstLayers.size(); i++)
		getLayer(i)->setVisible(bVisible);
}

bool CLayerManager::saveScript( const DAnsiStr& strFN )
{
	if(m_lstLayers.size() == 0) return false;

	CSketchConfig* cfg = new CSketchConfig(strFN, CAppConfig::fmWrite);		
	bool bres = saveScript(cfg);
	SAFE_DELETE(cfg);
	return bres;
}

bool CLayerManager::saveScript( CSketchConfig* cfg )
{
	if(cfg == NULL) return false;

	CLayer* aLayer = NULL;
	
	vector<int> layersRoot;
	int offset = 0;
	for(size_t i=0; i<m_lstLayers.size(); i++)
	{
		aLayer = getLayer(i);
		if(aLayer->hasBlob())
		{
			layersRoot.push_back(aLayer->getBlob()->getID() + offset);
			aLayer->recursive_WriteBlobNode(cfg, aLayer->getBlob(), offset);
			offset += aLayer->fetchLastNodeID();
		}
		else
			layersRoot.push_back(-1);
	}

	cfg->writeInt("Global", "FileVersion", SCENE_FILE_VERSION);
	cfg->writeInt("Global", "NumLayers", m_lstLayers.size());
	cfg->writeIntArray("Global", "RootIDs", layersRoot);
	return true;
}

bool CLayerManager::loadScript( const DAnsiStr& strFN )
{
	if(!PS::FILESTRINGUTILS::FileExists(strFN.ptr())) return false;

	CSketchConfig* cfg = new CSketchConfig(strFN, CAppConfig::fmRead);
	bool bres = loadScript(cfg);
	SAFE_DELETE(cfg);	
	return bres;
}

bool CLayerManager::loadScript( CSketchConfig* cfg )
{
	removeAllLayers();
	int ctLayers = cfg->readInt("Global", "NumLayers");
	if(ctLayers <= 0) 	
		return false;	

	int fileVersion = cfg->readInt("Global", "FileVersion");
	
	vector<int> layersRoot;
	cfg->readIntArray("Global", "RootIDs", ctLayers, layersRoot);

	CLayer* aLayer = NULL;	
	for(size_t i=0; i<layersRoot.size(); i++)
	{		
		aLayer = new CLayer();		
		aLayer->setCellSize(DEFAULT_CELL_SIZE);
		aLayer->setPolyBounds(DEFAULT_BOUNDS);
		aLayer->setPolySeedPoint(vec3(0.0f, 0.0f, 0.0f));
		aLayer->setVisible(true);		
		aLayer->setGroupName(printToAStr("Layer %d", i));
		if(layersRoot[i] >= 0)
		{
			if(fileVersion == 0)
			{
				int idIncremental = 0;
				aLayer->recursive_ReadBlobNodeV0(cfg, NULL, layersRoot[i], idIncremental);
			}
			else
				aLayer->recursive_ReadBlobNode(cfg, NULL, layersRoot[i], layersRoot[i]);
			aLayer->bumpRevision();
		}

		//Update lastNodeID
		int lastUsedID = aLayer->recursive_MaxNodeID(-1, aLayer->getBlob());
		aLayer->setLastNodeID(lastUsedID + 1);

		addLayer(aLayer);
	}
	
	//Set Active Layer
	setActiveLayer(0);

	layersRoot.clear();
	return true;
}

void CLayerManager::setAdaptiveParam( float param )
{
	for(size_t i=0; i< m_lstLayers.size(); i++)
	{
		m_lstLayers[i]->setAdaptiveParam(param);
	}
}

}
}


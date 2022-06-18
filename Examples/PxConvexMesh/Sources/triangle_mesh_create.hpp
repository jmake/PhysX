#include <ctype.h>

#include "PxPhysicsAPI.h"

#include "load_file.hpp" // LoadFile

using namespace physx;

float rand2(float loVal, float hiVal)
{
  return loVal + (float(rand())/float(RAND_MAX))*(hiVal - loVal);
}

///////////////////////////////////////////////////////////////////////////////
//
// SEE: 
//   https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Geometry.html
// 
///////////////////////////////////////////////////////////////////////////////
static void GetTriangleMesh(const PxGeometry& geom, 
                            std::vector< std::vector<double> >& vertices, 
                            std::vector< std::vector<double> >& faces)
{
  if(geom.getType() == PxGeometryType::eTRIANGLEMESH)
  {
      const PxTriangleMeshGeometry& triGeom = static_cast<const PxTriangleMeshGeometry&>(geom);
      const PxVec3 scale = triGeom.scale.scale;

      const PxTriangleMesh& mesh = *triGeom.triangleMesh;

      const PxU32 nbVerts       = mesh.getNbVertices();
      const PxVec3* vertexBuffer = mesh.getVertices();
      for(int i=0; i<nbVerts; i++)
      {
        PxVec3 p = vertexBuffer[i];
        vertices.push_back( {p.x,p.y,p.z} );
      }       

      const void* indexBuffer = mesh.getTriangles();
      const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
      const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);

      const PxU32 triangleCount = mesh.getNbTriangles();
      const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
      for(PxU32 i=0; i < triangleCount; ++i)
      {
        PxVec3 triVert[3];

        if(has16BitIndices)
        {
          int id0 = *shortIndices++; 
          int id1 = *shortIndices++; 
          int id2 = *shortIndices++; 

          triVert[0] = vertexBuffer[id0];
          triVert[1] = vertexBuffer[id1];
          triVert[2] = vertexBuffer[id2];
faces.push_back( {id0,id1,id2});
        }
        else
        {
          int id0 = *shortIndices++; 
          int id1 = *shortIndices++; 
          int id2 = *shortIndices++; 

          triVert[0] = vertexBuffer[id0];
          triVert[1] = vertexBuffer[id1];
          triVert[2] = vertexBuffer[id2];
faces.push_back( {id0,id1,id2});
        }
      }       
  }
}


///////////////////////////////////////////////////////////////////////////////
// Triamesh -> Actor 
// Setup common cooking params
void setupCommonCookingParams(PxCookingParams& params, bool skipMeshCleanup, bool skipEdgeData)
{
    // we suppress the triangle mesh remap table computation to gain some speed, as we will not need it 
  // in this snippet
  params.suppressTriangleMeshRemapTable = true;

  // If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. The input mesh must be valid. 
  // The following conditions are true for a valid triangle mesh :
  //  1. There are no duplicate vertices(within specified vertexWeldTolerance.See PxCookingParams::meshWeldTolerance)
  //  2. There are no large triangles(within specified PxTolerancesScale.)
  // It is recommended to run a separate validation check in debug/checked builds, see below.

  if (!skipMeshCleanup)
    params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
  else
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

  // If DISABLE_ACTIVE_EDGES_PREDOCOMPUTE is set, the cooking does not compute the active (convex) edges, and instead 
  // marks all edges as active. This makes cooking faster but can slow down contact generation. This flag may change 
  // the collision behavior, as all edges of the triangle mesh will now be considered active.
  if (!skipEdgeData)
    params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);
  else
    params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
}


// Creates a triangle mesh using BVH33 midphase with different settings.
PxTriangleMesh* 
createBV33TriangleMesh(PxU32 numVertices, const PxVec3* vertices, PxU32 numTriangles, const PxU32* indices,
  bool skipMeshCleanup, bool skipEdgeData, bool inserted, bool cookingPerformance, bool meshSizePerfTradeoff,
PxPhysics *physics, PxCooking *cooking)
{
  PxTriangleMeshDesc meshDesc;
  meshDesc.points.count = numVertices;
  meshDesc.points.data = vertices;
  meshDesc.points.stride = sizeof(PxVec3);
  meshDesc.triangles.count = numTriangles;
  meshDesc.triangles.data = indices;
  meshDesc.triangles.stride = 3 * sizeof(PxU32);

  PxCookingParams params = cooking->getParams();

  // Create BVH33 midphase
  params.midphaseDesc = PxMeshMidPhase::eBVH33;

  // setup common cooking params
  setupCommonCookingParams(params, skipMeshCleanup, skipEdgeData);

  // The COOKING_PERFORMANCE flag for BVH33 midphase enables a fast cooking path at the expense of somewhat lower quality BVH construction. 
  if (cookingPerformance)
    params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
  else
    params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eSIM_PERFORMANCE;

  // If meshSizePerfTradeoff is set to true, smaller mesh cooked mesh is produced. The mesh size/performance trade-off
  // is controlled by setting the meshSizePerformanceTradeOff from 0.0f (smaller mesh) to 1.0f (larger mesh).
  if(meshSizePerfTradeoff)
  { 
    params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.0f;
  }
  else
  { 
    // using the default value
    params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.55f;
  }

  cooking->setParams(params);

#if defined(PX_CHECKED) || defined(PX_DEBUG)
  // If DISABLE_CLEAN_MESH is set, the mesh is not cleaned during the cooking. 
  // We should check the validity of provided triangles in debug/checked builds though.
  if (skipMeshCleanup)
  { 
    PX_ASSERT(cooking->validateTriangleMesh(meshDesc));
  }
#endif // DEBUG

  PxTriangleMesh* triMesh = NULL;
  PxU32 meshSize = 0;

  // The cooked mesh may either be saved to a stream for later loading, or inserted directly into PxPhysics.
  if (inserted)
  {
    triMesh = cooking->createTriangleMesh(meshDesc, physics->getPhysicsInsertionCallback());
  }
  else
  {
    PxDefaultMemoryOutputStream outBuffer;
    cooking->cookTriangleMesh(meshDesc, outBuffer);

    PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
    triMesh = physics->createTriangleMesh(stream);

    meshSize = outBuffer.getSize();
  }

  return triMesh; 
}


///////////////////////////////////////////////////////////////////////////////
// Create Random Terrain 
void createRandomTerrain(const PxVec3& origin, PxU32 numRows, PxU32 numColumns,
  PxReal cellSizeRow, PxReal cellSizeCol, PxReal heightScale,
  PxVec3*& vertices, PxU32*& indices)
{
  PxU32 numX = (numColumns + 1);
  PxU32 numZ = (numRows + 1);
  PxU32 numVertices = numX*numZ;
  PxU32 numTriangles = numRows*numColumns * 2;

  if (vertices == NULL)
    vertices = new PxVec3[numVertices];
  if (indices == NULL)
    indices = new PxU32[numTriangles * 3];

  PxU32 currentIdx = 0;
  for (PxU32 i = 0; i <= numRows; i++)
  {
    for (PxU32 j = 0; j <= numColumns; j++)
    {
      PxVec3 v(origin.x + PxReal(j)*cellSizeRow, origin.y, origin.z + PxReal(i)*cellSizeCol);
      vertices[currentIdx++] = v;
    }
  }

  currentIdx = 0;
  for (PxU32 i = 0; i < numRows; i++)
  {
    for (PxU32 j = 0; j < numColumns; j++)
    {
      PxU32 base = (numColumns + 1)*i + j;
      indices[currentIdx++] = base + 1;
      indices[currentIdx++] = base;
      indices[currentIdx++] = base + numColumns + 1;
      indices[currentIdx++] = base + numColumns + 2;
      indices[currentIdx++] = base + 1;
      indices[currentIdx++] = base + numColumns + 1;
    }
  }

  for (PxU32 i = 0; i < numVertices; i++)
  {
    PxVec3& v = vertices[i];
    v.y += heightScale * rand(-1.0f, 1.0f);
  }
}

PxTriangleMeshGeometry
createRandomTriangleMeshes(PxPhysics *physics, PxCooking *cooking)
{
  const PxU32 numColumns = 32;
  const PxU32 numRows = 32;
  const PxU32 numVertices = (numColumns + 1)*(numRows + 1);
  const PxU32 numTriangles = numColumns*numRows * 2;

  PxVec3* vertices = new PxVec3[numVertices];
  PxU32* indices = new PxU32[numTriangles * 3];

  srand(50);

  // -> vertices, indices
  createRandomTerrain(PxVec3(0.0f, 0.0f, 0.0f), numRows, numColumns, 0.1f, 0.1f, 1.0f, vertices, indices);

  // Favor cooking speed, skip mesh cleanup, and don't precompute the active edges. Insert into PxPhysics.
  // This is the fastest possible solution for runtime cooking, but all edges are marked as active, which can
  // further reduce runtime performance, and also affect behavior.
  PxTriangleMesh* mesh = createBV33TriangleMesh(numVertices, vertices, numTriangles, indices, false, true, true, true, false, physics, cooking); 

  PxTriangleMeshGeometry geom(mesh);
  return geom; 
}

///////////////////////////////////////////////////////////////////////////////
// Create Random Terrain 
PxTriangleMeshGeometry
createChassisTriangleMeshes(PxPhysics *physics, PxCooking *cooking)
{
  /*
def Arrays2D2PxVec3(arrays,arrayname,arraytype="PxVec3"): 
    arrays = ["%s(%s)" % (arraytype,",".join([str(a) for a in array])) for array in arrays]
    arrays = "std::vector<%s> %s={%s};" % (arraytype, arrayname,",".join(arrays))
    print(arrays)
    return arrays; 

def Arrays2D2PxU32(arrays,arrayname,arraytype="PxU32"): 
    arrays = [str(a) for array in arrays for a in array]
    arrays = "std::vector<%s> %s={%s};" % (arraytype, arrayname,",".join(arrays))
    print(arrays)
    return arrays;

Arrays2D2PxVec3(vertices,"vertices")
Arrays2D2PxU32(faces,"faces","PxU32")
  */ 

std::vector<PxVec3> vertices={PxVec3(-0.5,0.5,0.5),PxVec3(0.5,0.5,0.5),PxVec3(0.5,0.5,-0.5),PxVec3(-0.5,0.5,-0.5),PxVec3(0.5,-0.5,-0.5),PxVec3(-0.5,-0.5,-0.5),PxVec3(0.5,-0.5,0.5),PxVec3(-0.5,-0.5,0.5)};
std::vector<PxU32> faces={0,1,2,0,2,3,4,5,3,4,3,2,4,2,1,4,1,6,7,6,1,7,1,0,7,0,3,7,3,5,7,5,4,7,4,6};

  PxTriangleMesh* mesh = createBV33TriangleMesh(//numVertices, vertices, numTriangles, indices, 
                                                vertices.size(), vertices.data(), 
                                                faces.size(), faces.data(), 
                                                false, true, true, true, false, physics, cooking); 
  PxTriangleMeshGeometry geom(mesh);
  return geom; 
}


template<class F, class I>
PxTriangleMeshGeometry
CreateFromVectors2DTriangleMesh(
                                std::vector< std::vector<F> > vcoords,
                                std::vector< std::vector<I> > vtrias,
                                PxPhysics *physics, PxCooking *cooking
                               )
{ 
  std::vector<PxVec3> verts;
  for(auto c:vcoords)
  { 
    if(c.size() == 3) verts.push_back( PxVec3(c[0],c[1],c[2]) );
  }
  
  std::vector<PxU32> faces;
  for(auto tria:vtrias)
  { 
    if(tria.size() == 3) {for(auto t:tria) faces.push_back(t);};
  }

  PxTriangleMesh* mesh = createBV33TriangleMesh(
                                                verts.size(), verts.data(),
                                                faces.size(), faces.data(),
                                                false, true, true, true, false, physics, cooking);

  PxTriangleMeshGeometry geom(mesh);
  return geom;
}


PxTriangleMeshGeometry
CreateFromFilesTriangleMesh(
                            std::string fcoords, 
                            std::string ftrias, 
                            PxPhysics *physics, PxCooking *cooking
                           )
{ 
  std::vector< std::vector<float> > coords;
  LoadFile(fcoords, coords, false);

  std::vector< std::vector<int> > trias;
  LoadFile(ftrias, trias, false);

  return CreateFromVectors2DTriangleMesh<float,int>(coords, trias, physics, cooking);  
}


 
PxTriangleMeshGeometry
createBunnyMesh(std::string fcoords, std::string ftrias, PxPhysics *physics, PxCooking *cooking)
{
  std::vector< std::vector<float> > coords;
  LoadFile(fcoords, coords, false);

  std::vector< std::vector<int> > trias;
  LoadFile(ftrias, trias, false);

  std::vector<PxVec3> verts;
  for(auto c:coords) 
  {
    if(c.size() == 3) verts.push_back( PxVec3(c[0],c[1],c[2]) ); 
  }

  std::vector<PxU32> faces;
  for(auto tria:trias) 
  { 
    if(tria.size() == 3) {for(auto t:tria) faces.push_back(t);};
  } 

  PxTriangleMesh* mesh = createBV33TriangleMesh(
                                                verts.size(), verts.data(),
                                                faces.size(), faces.data(),
                                                false, true, true, true, false, physics, cooking);

  PxTriangleMeshGeometry geom(mesh);
  return geom; 
}





///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

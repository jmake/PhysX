#include <iostream>
#include <vector>

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxShapeExt.h>
#include <foundation/PxMat33.h>
#include <extensions/PxSimpleFactory.h>

using namespace physx;


void 
CreateGeometryFromPhysxGeometry(
				PxTriangleMeshGeometry g,
				const PxTransform &globalPose) 
{
  PxTriangleMesh *tm = g.triangleMesh;

  const PxU32 nbVerts = tm->getNbVertices();
  const PxVec3* PVertices = tm->getVertices();
  const void* Triangles = tm->getTriangles();
  PxU32 nbTris = tm->getNbTriangles();

  PxTriangleMeshFlags flag = tm->getTriangleMeshFlags(); 
  PxTriangleMeshFlag::e16_BIT_INDICES;
  PxTriangleMeshFlag::eADJACENCY_INFO;


  PxU32 I0, I1, I2;
  for (PxU32 TriIndex = 0; TriIndex < nbTris; ++TriIndex)
  {
    //if (tm->getTriangleMeshFlags() &amp; PxTriangleMeshFlag::eHAS_16BIT_TRIANGLE_INDICES)
    if(flag == PxTriangleMeshFlag::e16_BIT_INDICES)  
    {
        PxU16* P16BitIndices = (PxU16*)Triangles;
        I0 = P16BitIndices[(TriIndex * 3) + 0];
        I1 = P16BitIndices[(TriIndex * 3) + 1];
        I2 = P16BitIndices[(TriIndex * 3) + 2];
    }
    //else
    {
        PxU32* P32BitIndices = (PxU32*)Triangles;
        I0 = P32BitIndices[(TriIndex * 3) + 0];
        I1 = P32BitIndices[(TriIndex * 3) + 1];
        I2 = P32BitIndices[(TriIndex * 3) + 2];
    }

        // Local position
//    const FVector V0 = P2UVector(PVertices[I0]);
//    const FVector V1 = P2UVector(PVertices[I1]);
//    const FVector V2 = P2UVector(PVertices[I2]);
  }
}


/*
bool CreateGeometryFromPhysxGeometry(PxTriangleMeshGeometry &g, const PxTransform &globalPose) //, LevelGeometry::Mesh &mesh)
{
    PxTriangleMesh *tm = g.triangleMesh;

    const PxU32 nbVerts = tm->getNbVertices();
    const PxVec3* verts = tm->getVertices();
    PxU32 nbTris = tm->getNbTriangles();
    const void* typelessTris = tm->getTriangles();

    PxTriangleMeshFlags flag = tm->getTriangleMeshFlags(); 

physx::PxTriangleMeshFlag::e16_BIT_INDICES;
physx::PxTriangleMeshFlag::eADJACENCY_INFO;


//    flag.Enum;  
//    flag::e16_BIT_INDICES; 
//    flag::eADJACENCY_INFO; 

    PxU32 numIndices = nbTris * 3;
//    mesh.indices.Resize(numIndices);
 
    if (tm->has16BitTriangleIndices())
    {
      //  Convert indices to 32 bit
      const short *i16 = reinterpret_cast<const short*>(typelessTris);

      for (PxU32 i = 0; i < numIndices; ++i)
      {
//        mesh.indices[i] = i16[i];
      }
    }
    else
    {
//      int size = numIndices * sizeof(mesh.indices[0]);
//      memcpy_s(&mesh.indices[0], size, typelessTris, size);
    }

    //  Transform vertices to world space
//    mesh.vertices.Resize(nbVerts);
    for (PxU32 i = 0; i < nbVerts; ++i)
    {
      PxVec3 pos = globalPose.transform(verts[i]);
//      mesh.vertices[i].Assign(pos.x, pos.y, pos.z);
    }

    //  Reject out-of-regions triangles
//    numIndices = gConvexRegionsManager.FilterOutsideTriangles(&mesh.vertices[0], &mesh.indices[0], numIndices);
//    r3d_assert(numIndices % 3 == 0);

//    mesh.indices.Resize(numIndices);

    return true;
}
*/ 

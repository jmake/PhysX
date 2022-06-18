#include <ctype.h>

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>
#include <fstream>


#include "PxPhysicsAPI.h"
#include "convex_mesh_create.hpp"
#include "triangle_mesh_create.hpp"

#include "load_file.hpp" // LoadFile

#define PVD_HOST "127.0.0.1" 
#define PX_RELEASE(x)  if(x){x->release(); x = NULL;}

std::ofstream fout;
std::vector< std::ofstream > fouts(4); 

using namespace physx;

physx::PxQuat YZ(0.0,0.0,1.0,1.0);
physx::PxQuat XZ(0.0,1.0,0.0,1.0);
physx::PxQuat XY(1.0,0.0,0.0,1.0);

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;
PxCooking*        gCooking  = NULL;


PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene		= NULL;

PxMaterial*				gMaterial	= NULL;

PxPvd*                  gPvd        = NULL;

PxReal stackZ = 10.0f;


float Rand(float loVal, float hiVal)
{
  return loVal + (float(rand())/float(RAND_MAX))*(hiVal - loVal);
}



PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for(PxU32 i=0; i<size;i++)
	{
		for(PxU32 j=0;j<size-i;j++)
		{
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}


physx::PxRigidDynamic*
CreateRigidDynamicWithShape2(const physx::PxGeometry& g, const physx::PxTransform& t)
{
  physx::PxMaterial*  m = gMaterial;

  physx::PxRigidDynamic* actor = gPhysics->createRigidDynamic(t);
  physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, g, *m);
  return actor;
}


physx::PxRigidStatic*
CreateRigidStaticWithShape2(const physx::PxGeometry& g, const physx::PxTransform& t)
{ 
  physx::PxMaterial*  m = gMaterial;
  
  physx::PxRigidStatic* actor = gPhysics->createRigidStatic(t);
  physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, g, *m);
  return actor;
}


void AddRandomGeometries(const PxGeometry geom, int ngeoms, float max, float ymin, float ymax)
{
  for(int i=0; i<ngeoms; i++)
  { 
    float xr = Rand(-0.9*max,0.9*max);
    float zr = Rand(-0.9*max,0.9*max);
    float yr = Rand(ymin, ymax);
    physx::PxVec3 p(xr,yr,zr);
    
    float qx = Rand(-1.0,1.0);
    float qy = Rand(-1.0,1.0);
    float qz = Rand(-1.0,1.0);
    float qw = Rand(-3.1415,3.1415);
    physx::PxQuat q(qx,qy,qz,qw);
    
    physx::PxRigidDynamic* actor = CreateRigidDynamicWithShape2(geom, physx::PxTransform(p,q));
    gScene->addActor(*actor);
  }

}


void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
  gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
/* 
  sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
  sceneDesc.ccdMaxPasses = 4;
*/
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

  CreateTruncatedCubeMeshTest(gPhysics, gCooking); 

  // Add Plane 
  float MAX=20;

	PxRigidStatic* groundPlane0 = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane0);

  // Add Triangles
  PxTriangleMeshGeometry ramdomTriangles02Geom; 

  std::string fcoords;
  std::string ftrias; 

  fcoords = "";
  ftrias =  ""; 

/* FAIL 
  fcoords = "Objs/bunny_verts.dat";
  ftrias = "Objs/bunny_faces.dat";
  ramdomTriangles02Geom = createBunnyMesh(fcoords, ftrias, gPhysics, gCooking);
  physx::PxRigidDynamic* ramdomTriangles01Actor = CreateRigidDynamicWithShape2(ramdomTriangles02Geom, physx::PxTransform(physx::PxVec3(0.0,200.0,0.0)));
  gScene->addActor(*ramdomTriangles01Actor);
*/

/* // TOO HEAVY 
  fcoords = "Objs/buda2_verts.dat";
  ftrias =  "Objs/buda2_faces.dat";
  ramdomTriangles02Geom = createBunnyMesh(fcoords, ftrias, gPhysics, gCooking);
  physx::PxRigidDynamic* ramdomTriangles02Actor = CreateRigidDynamicWithShape2(ramdomTriangles02Geom, physx::PxTransform(physx::PxVec3(0.0,155.0,0.0)));
  gScene->addActor(*ramdomTriangles02Actor);
*/

  fcoords = "Objs/teapot_verts.dat";
  ftrias = "Objs/teapot_faces.dat";
  ramdomTriangles02Geom = createBunnyMesh(fcoords, ftrias, gPhysics, gCooking);
  for(int i=0; i<10; i++)
  {
    float xr = Rand(-1.1*MAX,1.1*MAX);
    float zr = Rand(-1.1*MAX,1.1*MAX);
    float yr = Rand(0.0,100.0);
    physx::PxVec3 p(xr,yr,zr);

    float qx = Rand(-1.0,1.0);
    float qy = Rand(-1.0,1.0);
    float qz = Rand(-1.0,1.0);
    float qw = Rand(-3.1415,3.1415);
    physx::PxQuat q(qx,qy,qz,qw);

    physx::PxRigidDynamic* ramdomTriangles02Actor = CreateRigidDynamicWithShape2(ramdomTriangles02Geom, physx::PxTransform(p,q));
    gScene->addActor(*ramdomTriangles02Actor);
  }

 
  fcoords = "Objs/teddy_verts.dat";
  ftrias =  "Objs/teddy_faces.dat"; 

  ramdomTriangles02Geom = createBunnyMesh(fcoords, ftrias, gPhysics, gCooking);

  for(int i=0; i<10; i++)
  {
    float xr = Rand(-1.1*MAX,1.1*MAX);
    float zr = Rand(-1.1*MAX,1.1*MAX);
    float yr = Rand(0.0,100.0);
    physx::PxVec3 p(xr,yr,zr);

    float qx = Rand(-1.0,1.0);
    float qy = Rand(-1.0,1.0);
    float qz = Rand(-1.0,1.0);
    float qw = Rand(-3.1415,3.1415);
    physx::PxQuat q(qx,qy,qz,qw);

    physx::PxRigidDynamic* ramdomTriangles02Actor = CreateRigidDynamicWithShape2(ramdomTriangles02Geom, physx::PxTransform(p,q));
    gScene->addActor(*ramdomTriangles02Actor);
  }


  // Coins 
  PxConvexMesh* wheel = createWheelMesh(0.2f, 1.0f, gPhysics, gCooking);
  PxConvexMeshGeometry wheelGeom(wheel);

  for(int i=0; i<50; i++)
  {
    float xr = Rand(-0.9*MAX,0.9*MAX);
    float zr = Rand(-0.9*MAX,0.9*MAX);
    float yr = Rand(30.0,100.0);
    physx::PxVec3 p(xr,yr,zr);

    float qx = Rand(-1.0,1.0);
    float qy = Rand(-1.0,1.0);
    float qz = Rand(-1.0,1.0);
    float qw = Rand(-3.1415,3.1415);
    physx::PxQuat q(qx,qy,qz,qw);

    physx::PxRigidDynamic* wheelActor1 = CreateRigidDynamicWithShape2(wheelGeom, physx::PxTransform(p,q));
    gScene->addActor(*wheelActor1);
  }

 
  // Boundaries  
  PxConvexMesh* poly02 = createPlaneMesh(gPhysics, gCooking);
  PxConvexMeshGeometry poly02Geom(poly02);

  physx::PxVec3 Zmax(0.0,-5.0, MAX); 
  physx::PxVec3 Zmin(0.0,-5.0,-MAX); 

  physx::PxVec3 Xmax( MAX,-5,0); 
  physx::PxVec3 Xmin(-MAX,-5,0);

  physx::PxVec3 Ymin(0.0,-5.0,0);
 
  physx::PxRigidStatic* poly02Actor;
/*  
  poly02Actor = CreateRigidStaticWithShape2(poly02Geom, physx::PxTransform(Zmax,XY));
  gScene->addActor(*poly02Actor);

  poly02Actor = CreateRigidStaticWithShape2(poly02Geom, physx::PxTransform(Xmax,YZ));
  gScene->addActor(*poly02Actor);

  poly02Actor = CreateRigidStaticWithShape2(poly02Geom, physx::PxTransform(Zmin,XY));
  gScene->addActor(*poly02Actor);

  poly02Actor = CreateRigidStaticWithShape2(poly02Geom, physx::PxTransform(Xmin,YZ));
  gScene->addActor(*poly02Actor);
*/
  poly02Actor = CreateRigidStaticWithShape2(poly02Geom, physx::PxTransform(Ymin,XZ)); // No remove!!
  gScene->addActor(*poly02Actor);


  // Truncated Cubes 
  fcoords = "Sources/truncated_cube.dat";
  PxConvexMesh* poly03 = CreateFromFileConvexMesh(fcoords, gPhysics, gCooking);
  PxConvexMeshGeometry poly03Geom(poly03);

  for(int i=0; i<150; i++)
  { 
    float xr = Rand(-0.9*MAX,0.9*MAX); 
    float zr = Rand(-0.9*MAX,0.9*MAX);
    float yr = Rand(0.0,145.0);
    physx::PxVec3 p(xr,yr,zr);

    float qx = Rand(-1.0,1.0);
    float qy = Rand(-1.0,1.0);
    float qz = Rand(-1.0,1.0);
    float qw = Rand(-3.1415,3.1415);
    physx::PxQuat q(qx,qy,qz,qw);

    physx::PxRigidDynamic* poly03Actor = CreateRigidDynamicWithShape2(poly03Geom, physx::PxTransform(p,q));
    gScene->addActor(*poly03Actor);
  }
 
}

	
void cleanupPhysics(bool /*interactive*/)
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
  PX_RELEASE(gCooking); 
	PX_RELEASE(gFoundation);
	
//	printf("#\t[SnippetHelloWorld] done.\n");
}


void renderActors(PxRigidActor** actors, const PxU32 numActors, int itime) 
{

fouts[2]<<"positions.set("<< itime <<",{"; 
fouts[3]<<"quaternions.set("<< itime <<",{";

  std::vector<int> Active; 

  PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
  for(PxU32 i=0,idx=0; i<numActors; i++)
  {
    const PxU32 nbShapes = actors[i]->getNbShapes();
    PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
    actors[i]->getShapes(shapes, nbShapes);
    const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

    for(PxU32 j=0;j<nbShapes;j++)
    {
      const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
      const PxGeometryHolder h = shapes[j]->getGeometry();

if(!itime)
{
  std::vector< std::vector<double> > vertices;
  std::vector< std::vector<double> > faces;

  GetConvexMesh(  h.any(), vertices, faces);
  GetTriangleMesh(h.any(), vertices, faces);
 
/* 
  // Python 
  fouts[0]<<"\n#vertices:" << vertices.size() <<" faces:"<< faces.size() <<" \n";

  fouts[0]<<"vertices["<< i <<"]=["; 
  for(auto v : vertices) fouts[0]<<"["<< v[0] <<","<< v[1] <<","<< v[2] <<"],"; 
  fouts[0]<<"] \n"; 

  fouts[0]<<"faces["<< i <<"]=[";
  for(auto f : faces) fouts[0]<<"["<< f.size() <<","<< f[0] <<","<< f[1] <<","<< f[2] <<"],";   
  fouts[0]<<"] \n";
*/
  // javascript
//  if( faces.size() ) Active.push_back(i);   
 
  fouts[1]<<"\n// vertices:" << vertices.size() <<" faces:"<< faces.size() <<" \n";

  fouts[1]<<"vertices.set("<< i <<",[";
  for(auto v : vertices) fouts[1]<<"["<< v[0] <<","<< v[1] <<","<< v[2] <<"],";
  fouts[1]<<"]); \n//\n";

  fouts[1]<<"faces.set("<< i <<",[";
  for(auto f : faces) fouts[1]<<"["<< f[0] <<","<< f[1] <<","<< f[2] <<"],";
  fouts[1]<<"]); \n//\n";

  if( faces.size() )
  {
    fouts[1]<<"polygonids.set("<< idx++ <<","<< i <<"); \n";
  } 

  faces.clear();
  vertices.clear();
} 

physx::PxTransform com(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));  
physx::PxVec3   p = com.p;
physx::PxQuat   q = com.q;

/* 
std::cout
<< itime <<" " 
<< i <<" "
<< p.x <<" "
<< p.y <<" "
<< p.z 
<<"\n";
*/ 

fouts[2] << i <<":["<< p.x <<","<< p.y <<","<< p.z <<"],";  
fouts[3] << i <<":["<< q.x <<","<< q.y <<","<< q.z <<","<< q.w <<"],";

    }
  } 

  fouts[2]<<"}); \n";
  fouts[3]<<"}); \n";
} 


void stepPhysics(int itime)
{ 
  gScene->simulate(1.0f/60.0f);
  gScene->fetchResults(true);

  PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
  if(nbActors)
  {
    std::vector<PxRigidActor*> actors(nbActors);
    gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
    renderActors(&actors[0], static_cast<PxU32>(actors.size()), itime);
  }
}


int snippetMain()
{
	static const PxU32 frameCount = 500;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++) stepPhysics(i);
	cleanupPhysics(false);
	return 0;
}


int main()
{
  // Meshes 
  //   python 
  fouts[0].open("polygons.py");
  fouts[0]<<"vertices={} \nfaces={}\n";  

  //   javascript 
  fouts[1].open("polygons.js");
  fouts[1]<<"faces=new Map(); \n";
  fouts[1]<<"vertices=new Map(); \n";
  fouts[1]<<"polygonids=new Map(); \n";

  fouts[2].open("positions.js");
  fouts[2]<<"positions=new Map(); \n";

  fouts[3].open("quaternions.js");
  fouts[3]<<"quaternions=new Map(); \n";

  snippetMain();

  fouts[2]<<"\n";
  fouts[2]<<"function GetNSteps(){return positions.size;}; \n";
  fouts[2]<<"console.log(`[Positions.js]:${positions.size} `); \n";

  for(auto &f: fouts) f.close(); 
  return 0; 
}




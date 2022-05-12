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

#include "extract_geometry.hpp"

#define PVD_HOST "127.0.0.1"  //Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.
#define PX_RELEASE(x)  if(x)  { x->release(); x = NULL;  }


using namespace physx;


PxPhysics*  gPhysics  = NULL;
PxMaterial* gMaterial  = NULL;
PxScene* gScene = NULL;

PxPvd* gPvd = NULL;
PxDefaultCpuDispatcher*  gDispatcher = NULL;
PxReal stackZ = 10.0f;

PxFoundation*  gFoundation = NULL;
PxDefaultAllocator    gAllocator;
PxDefaultErrorCallback  gErrorCallback;

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


void initPhysics(bool interactive)
{
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

  gPvd = PxCreatePvd(*gFoundation);
  PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
  gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

  gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

  PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
  sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
  gDispatcher = PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher  = gDispatcher;
  sceneDesc.filterShader  = PxDefaultSimulationFilterShader;
  gScene = gPhysics->createScene(sceneDesc);

  PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
  if(pvdClient)
  {
    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
  }

  gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
  PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
  gScene->addActor(*groundPlane);

  for(PxU32 i=0;i<5;i++)
    createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);

  if(!interactive)
    createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
}


/*
virtual PxU32 
PxScene::getActors(  
  PxActorTypeFlags   types,
  PxActor **   userBuffer,
  PxU32   bufferSize,
  PxU32   startIndex = 0   
)  
*/
void stepPhysics(bool /*interactive*/, int istep)
{
  gScene->simulate(1.0f/60.0f);
  gScene->fetchResults(true);

  PxU32 nStatics = gScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC); 
  PxU32 nDynamics = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);  
//  std::cout<<"nStatics nDynamics : "<< nStatics <<" "<< nDynamics <<"\n";


  PxActor **userBuffer = NULL;  
  PxU32   bufferSize = -1; 

  std::vector<PxActor*> Statics(nStatics); 
  std::vector<PxActor*> Dynamics(nDynamics); 


  nDynamics = gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, Dynamics.data(), Dynamics.size() ); 
  nStatics = gScene->getActors(PxActorTypeFlag::eRIGID_STATIC, Statics.data(), Dynamics.size() );
//  std::cout<<"nStatics nDynamics : "<< nStatics <<" "<< nDynamics <<"\n";

  //  eRIGID_STATIC:  1 << 0 = 0000 0001
  //  eRIGID_DYNAMIC: 1 << 1 = 0000 0010

  for(int i=0; i < nDynamics; i++)
  {
    PxActor* a = Dynamics[i];

    PxRigidDynamic *b = static_cast<PxRigidDynamic*>(a);

    PxReal mass = b->getMass();  
    PxVec3 vel = b->getLinearVelocity();

    PxTransform com = b->getGlobalPose(); 
    PxVec3   p = com.p; 
    PxQuat   q = com.q;

    //q.toRadiansAndUnitAxis(float& angle, PxVec3& axis); 

    std::cout<< istep <<" "<< i 
	     <<" "<< p.x <<" "<< p.y <<" "<< p.z 
             <<" "<< q.x <<" "<< q.y <<" "<< q.z <<" "<< q.w
             <<" "<< vel.x <<" "<< vel.y <<" "<< vel.z 
	     <<"\n";

    //CreateGeometryFromPhysxGeometry(PxBoxGeometry(2.0f,2.0f,2.0f), b->getGlobalPose());  
  }

  for(int i=0; i < nStatics; i++)
  {
    //std::cout<< i <<" "<< Statics[i]->getType() <<" \n";
  }  

  Statics.clear();
  Dynamics.clear(); 
}


void cleanupPhysics(bool /*interactive*/)
{
  PX_RELEASE(gScene);
  PX_RELEASE(gDispatcher);
  PX_RELEASE(gPhysics);
  if(gPvd)
  {
    PxPvdTransport* transport = gPvd->getTransport();
    gPvd->release();  gPvd = NULL;
    PX_RELEASE(transport);
  }
//PX_RELEASE(gFoundation);
  
//  printf("#SnippetHelloWorld done.\n");
}


//#include "extract_geometry.hpp"

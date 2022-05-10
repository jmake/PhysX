include <iostream>

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

static physx::PxFoundation* gFoundation = NULL;
static physx::PxPhysics* gPhysicsSDK = NULL;

static physx::PxDefaultErrorCallback gDefaultErrorCallback;
static physx::PxDefaultAllocator gDefaultAllocatorCallback;


void InitializePhysX()
{
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
  gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale());

  if(gPhysicsSDK==NULL)
  {
    std::cerr<<"Error creating PhysX device."<<std::endl;
    std::cerr<<"Exiting..."<<std::endl;
    std::exit(1);
  }

  std::cout<<"[InitializePhysX]"<<std::endl;
}


void ShutdownPhysX()
{
  gPhysicsSDK->release();
  std::cout<<"[ShutdownPhysX]"<<std::endl;
}


int
main(int argc, char** argv)
{
  InitializePhysX();
  ShutdownPhysX();
  return 0;
}




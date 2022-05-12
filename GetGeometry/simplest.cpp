#include "simulation.hpp"


int
main(int argc, char** argv)
{
  std::cout<<"istep iobj x y z qx qy qz qw vx vy vz\n"; 

  static const PxU32 frameCount = 500;
  initPhysics(false);
  for(PxU32 i=0; i<frameCount; i++) 
  {
//     std::cout<<"step:"<< i <<" ";
     stepPhysics(false, i);
//     std::cout<<"\n";
  } 
  cleanupPhysics(false);

  return 0;
}

/* 
static PxFoundation* gFoundation = NULL;
static PxPhysics* gPhysicsSDK = NULL;

static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;


void InitializePhysX() 
{
  gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
  gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());

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
*/

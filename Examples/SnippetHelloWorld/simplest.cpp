#include "simulation.hpp"


int
main(int argc, char** argv)
{
  std::cout<<"istep iobj x y z vx vy vz \n";

  static const PxU32 frameCount = 100;
  
  initPhysics(false);
  for(PxU32 i=0; i<frameCount; i++) stepPhysics(false, i);
  cleanupPhysics(false);

  return 0;
}

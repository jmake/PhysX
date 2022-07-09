#include <fstream>
#include <iterator>
#include <cstdlib>
#include <cassert>

#include "fields.hpp"
#include "simulation.hpp"




int main(int argc, char** argv)
{
  PxU32 frameCount = -1;

  assert(argc <= 2);
  frameCount = (argc < 2)?(3):(std::strtol(argv[1],nullptr,0));
  printf("[%s] nsteps:%d\n", argv[0], frameCount);

  SavePxFields savePxFields; 
  savePxFields.SetNSteps(frameCount); 

  PXFIELDS fields; 

  initPhysics(false);
  for(PxU32 it=0; it<frameCount; it++) 
  {
    fields.clear(); 
    int nparticles = stepPhysics(frameCount, it, fields);  
    savePxFields.SetArraySizes(nparticles); 
    savePxFields.SetFields(it, fields); 
  } 
  cleanupPhysics();

  savePxFields.WriteFields("fields_new.js");
  return 0;
}

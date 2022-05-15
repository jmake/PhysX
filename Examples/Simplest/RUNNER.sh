
CHECKING()
{
  ls -la physx/install/linux/PhysX/bin/linux.clang/release/
  ls physx/install/linux/PhysX/include/
  ls physx/install/linux/PxShared/include/foundation/
}


SIMULATION_RUN() 
{
  cd physx/Examples/Simplest

  mkdir BUILD
  cd BUILD

  cmake .. -DCMAKE_RULE_MESSAGES=OFF
  
  make

  ctest
}


CHECKING
SIMULATION_RUN


###

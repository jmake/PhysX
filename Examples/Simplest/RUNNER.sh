
CHECKING()
{

  ls -la /DUMMY/physx/install/linux/PhysX/bin/linux.clang/release/
  ls /DUMMY/physx/install/linux/PhysX/include/
  ls /DUMMY/physx/install/linux/PxShared/include/foundation/
}


SIMULATION_RUN() 
{
  cd /DUMMY/physx/Examples/Simplest

  mkdir BUILD
  cd BUILD

  cmake .. -DCMAKE_RULE_MESSAGES=OFF
  
  make

  ctest
}


CHECKING
SIMULATION_RUN


###

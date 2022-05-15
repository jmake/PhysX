
CHECKING()
{
  echo "[DOCKER] LIBRARIES"
  ls -la /DUMMY/physx/install/linux/PhysX/bin/linux.clang/release/
  
  echo "[DOCKER] INCLUDES:"
  ls /DUMMY/physx/install/linux/PhysX/include/PxPhysicsAPI.h
  ls /DUMMY/physx/install/linux/PxShared/include/foundation/Px.h
}


SIMULATION_RUN() 
{
  cd /home/Examples/Simplest

  mkdir BUILD
  cd BUILD

  cmake .. -DCMAKE_RULE_MESSAGES=OFF
  
  make

  ctest
}

echo "[DOCKER] ls"
ls

echo "[DOCKER] pwd"
pwd

echo "[DOCKER] ..."

CHECKING
SIMULATION_RUN


###

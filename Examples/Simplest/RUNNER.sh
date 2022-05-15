
CHECKING()
{
  echo "[DOCKER] LIBRARIES"
  ls -la /home/jovyan/work/physx/install/linux/PhysX/bin/linux.clang/release/
  
  echo "[DOCKER] INCLUDES:"
  ls /home/jovyan/work/physx/install/linux/PhysX/include/PxPhysicsAPI.h
  ls /home/jovyan/work/physx/install/linux/PxShared/include/foundation/Px.h
}


SIMULATION_RUN() 
{
  cd /home/jovyan/work/PhysX/Examples/Simplest

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

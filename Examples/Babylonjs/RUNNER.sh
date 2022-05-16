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
  cd /home/jovyan/work/PhysX/Babylonjs
  
  mkdir BUILD
  cd BUILD

  cmake .. -DCMAKE_RULE_MESSAGES=OFF
  
  make

  ctest
}


VISUALIZATION_RUN()
{  
  /home/jovyan/work/PhysX/Babylonjs/BUILD/snippetHelloWorld.x 200
  
  python3 /home/jovyan/work/PhysX/Babylonjs/physx2babylonjs.py
  
  ls 
  pwd 
}


echo "[DOCKER] ls"
ls

echo "[DOCKER] pwd"
pwd

echo "[DOCKER] ..."

CHECKING
SIMULATION_RUN
VISUALIZATION_RUN

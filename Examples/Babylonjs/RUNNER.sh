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
  cd /home/Examples/Babylonjs/

  mkdir BUILD
  cd BUILD

  cmake .. -DCMAKE_RULE_MESSAGES=OFF
  
  make

  ctest
}


VISUALIZATION_RUN()
{
  /home/Examples/Babylonjs/BUILD/snippetHelloWorld.x 200 
  
  ls 
  pwd 
  
  python3 /home/Examples/Babylonjs/physx2babylonjs.py
}


echo "[DOCKER] ls"
ls

echo "[DOCKER] pwd"
pwd

echo "[DOCKER] ..."

CHECKING
SIMULATION_RUN
VISUALIZATION_RUN

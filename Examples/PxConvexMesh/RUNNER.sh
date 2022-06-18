clear 

CHECKING()
{
  echo "[DOCKER] LIBRARIES"
  ls -la ${PHYSX}/bin/linux.clang/release/
  ls -la ${PHYSX}/bin/mac.x86_64/release/
 
  echo "[DOCKER] INCLUDES:"
  ls ${PHYSX}/install/linux/PhysX/include/PxPhysicsAPI.h
  ls ${PHYSX}/install/linux/PxShared/include/foundation/Px.h
}


SIMULATION_RUN() 
{
  rm -rf BUILD 
  mkdir BUILD
  cd BUILD

  cmake .. \
  -DCMAKE_RULE_MESSAGES=OFF \
  -DPHYSX_ROOT_DIR=${PHYSX}
 
  make

  ctest
}

##
export PATH="$PATH:/Users/poderozita/Downloads/DOCKERs/PhysX/Cmake3232/CMake.app/Contents/bin"
PHYSX=/Users/poderozita/Downloads/DOCKERs/PhysX/physx/

echo "[DOCKER] ls"
ls

echo "[DOCKER] pwd"
pwd

echo "[DOCKER] ..."
CHECKING
time SIMULATION_RUN

cd ..

time ./BUILD/snippetHelloWorld.x

rm Babylon/*.js 
mv *.js Babylon 

  

###

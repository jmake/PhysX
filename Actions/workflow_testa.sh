DOCKER_BUILD()
{
  docker build . \
    --file Dockerfile \
    --tag $IMAGE_NAME  \
    > /dev/null 
}


DOCKER_IMAGES()
{
  IMAGE_ID=$(docker images --filter=reference=$IMAGE_NAME --format "{{.ID}}") 
  echo "IMAGE_ID:", $IMAGE_ID    
}


DOCKER_RUN()
{
  docker run \
    --rm \
    --name ${CONTAINER_NAME} \
    --volume ${PWD}:/home \
    --workdir /home \
    -i ${IMAGE_NAME} \
    ${TOBEEXECUTED}
}


CONTAINER_NAME=physx_container
IMAGE_NAME=physx_image 

DOCKER_BUILD 

TOBEEXECUTED="ls -la"
DOCKER_RUN

TOBEEXECUTED="pwd"
DOCKER_RUN

TOBEEXECUTED="bash Examples/Simplest/RUNNER.sh"
DOCKER_RUN

TOBEEXECUTED="bash Examples/Babylonjs/RUNNER.sh"
DOCKER_RUN


DOCKER_RUN_DETACHED()
{
  echo "[DOCKER_RUN_DETACHED] ..."
  
  CONTAINER_ID=$(
     docker run \
        --detach \
        --rm \
        --name ${CONTAINER_NAME} \
        --workdir /home \
        --interactive ${IMAGE_NAME} \
        ${TOBEEXECUTED}
  )
  
  echo "[DOCKER_RUN_DETACHED] DONE!"  
}

DOCKER_STOP() 
{ 
  #echo "[DOCKER_STOP]" 
  docker stop ${CONTAINER_NAME}
  echo "[DOCKER_STOP] DONE!"
}


DOCKER_COPY_FROM_CONTAINER()
{
  echo "[DOCKER_COPY] ..."
  
  docker cp ${CONTAINER_NAME}:/home/Examples/Babylonjs/BUILD/babylon.html ./
  
  echo "[DOCKER_COPY] DONE!" 
}


DOCKER_RUN_DETACHED
DOCKER_COPY_FROM_CONTAINER
DOCKER_STOP

#
# docker run --rm --name nvcc_container --volume ${PWD}:/home --workdir /home -i nvcc_image bash GET_NVCC.sh
# 

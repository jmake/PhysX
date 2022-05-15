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


#
# docker run --rm --name nvcc_container --volume ${PWD}:/home --workdir /home -i nvcc_image bash GET_NVCC.sh
# 

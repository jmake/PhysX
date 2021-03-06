
DOCKER_PUSH() 
{
  echo "[DOCKER_PUSH] ..."

  ## Go To: 
  ##       https://hub.docker.com/repository/docker/${DUSERNAME}/${DREPOSITORY}
  ## 
  docker commit ${CONTAINER_NAME} ${IMAGE_NAME2}
  docker push ${IMAGE_NAME2}   

  echo "[DOCKER_PUSH] DONE!"
}

DOCKER_LOGIN() 
{
  docker login -u ${DUSERNAME} -p ${DPASSWORD}
  echo "[DOCKER_LOGIN] DONE!"
}

DOCKER_BUILD() 
{
  echo "[DOCKER_BUILD] ..."
  
  DOCKERFILE="./Actions/Dockerfile.physx"
  
  docker build . \
    --file ${DOCKERFILE} \
    --tag ${IMAGE_NAME}  

  echo "[DOCKER_BUILD] DONE!"
}


DOCKER_RUN_DETACHED()
{
  echo "[DOCKER_RUN_DETACHED] ..."
  
  CONTAINER_ID=$(
     docker run \
        --detach \
        --rm \
        --name ${CONTAINER_NAME} \
--volume ${PWD}/Examples:${DOCKER_WORDIR}/PhysX \
--workdir ${DOCKER_WORDIR} \
        --interactive ${IMAGE_NAME} \
        ${TOBEEXECUTED}
  )
#
# --volume ${PWD}:${DOCKER_WORDIR}
#
  docker ps -a
  echo "[DOCKER_RUN_DETACHED] DONE!"  
}


DOCKER_EXEC()
{
  echo "[DOCKER_EXEC] ..."

  echo "[${TOBEEXECUTED}] Runnnig ..."
  docker exec ${CONTAINER_NAME} ${TOBEEXECUTED}
  echo "[${TOBEEXECUTED}] Done!"
  
  echo "[DOCKER_EXEC] DONE!"  
}


DOCKER_COPY_TO_WORDIR()
{
  echo "[DOCKER_COPY] ..."

  #DOCKER_WORDIR=/DUMMY
  
  echo "${TOBECOPIED} -> ${DOCKER_WORDIR}"
  docker cp \
  ${TOBECOPIED} \
  ${CONTAINER_NAME}:${DOCKER_WORDIR}
  
  echo "[DOCKER_COPY] DONE!" 
}



DOCKER_COPY_FROM_CONTAINER()
{
  echo "[DOCKER_COPY] ..."
  
  docker cp ${CONTAINER_NAME}:${DOCKER_WORDIR}/PhysX/Babylonjs/BUILD/babylon.html ./
  
  echo "[DOCKER_COPY] DONE!" 
}

DOCKER_STOP() 
{ 
  #echo "[DOCKER_STOP]" 
  docker stop ${CONTAINER_NAME}
  echo "[DOCKER_STOP] DONE!"
}


## Input parameters 
#DPASSWORD=$1      # DOCKER_HUB_TOKEN 
#DUSERNAME=$2      # DOCKER_HUB_USERNAME
#DREPOSITORY=$3    # DOCKER_HUB_REPOSITORY 

##
#DVOLUME=$4        # github.workspace   

DOCKER_WORDIR=/home/jovyan/work/

## Docker ... 
CONTAINER_NAME=mybinder_container
IMAGE_NAME=mybinder_image
#${DUSERNAME}/${DREPOSITORY}
#IMAGE_NAME2=${DUSERNAME}/${DREPOSITORY}:$(date +%s)

#DOCKER_LOGIN ##
DOCKER_BUILD ##
DOCKER_RUN_DETACHED ## 

TOBEEXECUTED="ls -la"
DOCKER_EXEC

TOBEEXECUTED="bash PhysX/Simplest/RUNNER.sh"
DOCKER_EXEC

TOBEEXECUTED="bash PhysX/Babylonjs/RUNNER.sh"
DOCKER_EXEC


DOCKER_COPY_FROM_CONTAINER

## 
#TOBEEXECUTED="bash RUNNER.sh"
#DOCKER_EXEC

##
#DOCKER_PUSH ## 
DOCKER_STOP ## 

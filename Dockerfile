FROM jmake/mybinder:1652652023 AS notebook_setup 

## Just here to be able to make modifications
FROM notebook_setup AS notebook_execute 
ENV IPYNB_FILE="PhysX/simplest.ipynb"
ENV NB_USER="jovyan" 

USER root 
WORKDIR /home/jovyan/work 
#COPY ${IPYNB_FILE} /home/jovyan/work 
COPY * /home/jovyan/work/PhysX/
RUN ls -la /home/jovyan/work/PhysX/
RUN chown -R ${NB_USER} /home/jovyan/work

USER ${NB_USER}
RUN jupyter nbconvert --execute --clear-output ${IPYNB_FILE} 

## 
## SEE : 
##   jupyter/base-notebook @ https://jupyter-docker-stacks.readthedocs.io/en/latest/using/selecting.html
##
#FROM jupyter/base-notebook:latest AS notebook_setup 
#
####/var/lib/apt/lists/partial is missing. - Acquire (13: Permission denied)
#USER root
#RUN apt-get --yes -qq update
#RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata
#RUN apt-get --yes -qq update
#
#RUN apt-get --yes -qq install build-essential
#RUN apt-get --yes -qq install cmake
##RUN apt-get --yes -qq install cmake-curses-gui ## ccmake
#RUN apt-get --yes -qq install clang-tools-9
#RUN apt-get --yes -qq install vim  
#RUN apt-get --yes -qq install git

#FROM jmake/mybinder:1652646493 AS notebook_setup 
#
#FROM notebook_setup AS notebook_execute 
#ENV IPYNB_FILE="simplest.ipynb"
#ENV NB_USER="jovyan" 
#USER root 
#WORKDIR /home/jovyan/work 
#COPY ${IPYNB_FILE} /home/jovyan/work 
#RUN chown -R ${NB_USER} /home/jovyan/work
#USER ${NB_USER}
#RUN jupyter nbconvert --execute --clear-output ${IPYNB_FILE} 

##
##FROM jmake/mybinder:1652615015 AS PHYSICS 

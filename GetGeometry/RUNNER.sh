
SIMULATION_RUN() 
{
  rm -rf BUILD  
  mkdir BUILD 
  cd BUILD  

  cmake .. 

  make 

  ./simplest.x > simulation.dat 
 
  ctest
}


SIMULATION_NOTEBOOK()
{
  jupyter nbconvert --execute --clear-output simplest.ipynb
  jupyter-notebook --allow-root 
  #jupyter server list 
}


clear
pwd
SIMULATION_RUN
SIMULATION_NOTEBOOK


## docker cp physx41_container:/DUMMY/GetGeometry/BUILD/simulation.dat .

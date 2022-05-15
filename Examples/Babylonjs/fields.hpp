// fields.hpp
#ifndef FIELDS_H
#define FIELDS_H

#include <iostream>
#include <vector>
#include <map>
#include <limits>

using FLOAT =  float;

using VEC1D = std::vector<FLOAT>;  
using VEC2D = std::vector< std::vector<FLOAT> >;  

using PXFIELDS = std::map<std::string,VEC1D> ; 


void VEC1D_INIT(VEC1D &V1d, int ncols) 
{
   V1d = VEC1D(ncols,std::numeric_limits<FLOAT>::max()); 
}


void VEC2D_INIT(VEC2D &V2d, int nrows, int ncols) 
{
   V2d = VEC2D(); 
   V2d.assign(nrows, std::vector<FLOAT>(ncols,std::numeric_limits<FLOAT>::max()) ); 
}


void VEC2D_PRINT(VEC2D v2d, std::ofstream &output_file) 
{
  std::ostream_iterator<FLOAT> output_iterator(output_file, " ");

  int i = 1; 
  for(const auto& v1d: v2d) 
  {
    output_file<<"["; 
    int nrows = v1d.size();
    for(int j=0; j < nrows-1; j++) output_file<< v1d[j] <<","; 
    output_file<< v1d.back() <<"]"; 
    if(i < v2d.size()) output_file<<",";
    output_file<<"\n";
    i++; 
  } 
}


class SavePxFields
{
  public :

  int ntimes, nparticles; 
  std::ofstream *output_file;
  std::map<std::string,VEC2D> Fields; 


 ~SavePxFields()
  {
  }


  SavePxFields() 
  {
      ntimes = -1; 
      nparticles = -1;
  }


  void SetNSteps(int nsteps)
  {
      ntimes = nsteps; 
  }


  void SetArraySizes(int ncols)
  {
    nparticles = ncols;  
  }


  void SetFields(int itime, PXFIELDS fields) 
  {
    if(Fields.size() == 0) 
    {
        for(const auto& kv : fields) VEC2D_INIT(Fields[kv.first],ntimes,0);
    } 

    for(const auto& kv : fields) Fields[kv.first][itime] = kv.second;  
  }


  void WriteFields(std::string fname)
  {
    std::ofstream fout(fname);
    fout<<"var ntimesFunc=function(){return "<< ntimes <<";}\n\n"; 
    fout<<"var nparticlesFunc=function(){return "<< nparticles <<";}\n\n"; 

    for (const auto& kv : Fields) 
    {
        fout<<"var "<< kv.first <<"Fun2D=function(){\nreturn ["; 
        VEC2D_PRINT(kv.second, fout);
        fout<<"];}\n\n"; 
    }
    fout.close();
  }

};  

#endif /* FIELDS_H */

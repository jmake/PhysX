import os
import sys
import re
import numpy as np

import vtk
from vtk.util import numpy_support

#--------------------------------------------------------------------------||--#
#--------------------------------------------------------------------------||--#
contr  = vtk.vtkMultiProcessController.GetGlobalController()
nranks = contr.GetNumberOfProcesses() if contr else 1
rank   = contr.GetLocalProcessId()    if contr else 0

#--------------------------------------------------------------------------||--#
GetCoords    = lambda _ds : numpy_support.vtk_to_numpy( _ds.GetPoints().GetData() )
GetPointData = lambda _pd : {_pd.GetArrayName(k):numpy_support.vtk_to_numpy(_pd.GetArray(k)) for k in range(_pd.GetNumberOfArrays())}
GetCellData  = lambda _pd : {_pd.GetArrayName(k):numpy_support.vtk_to_numpy(_pd.GetArray(k)) for k in range(_pd.GetNumberOfArrays())}

GetNumbersFromString = lambda _s : [ float(n) for n in re.findall('\d+', _s) ]
getDicFromArray = lambda _A:{_u:np.nonzero(_A==_u)[0] for _u in np.unique(_A) }


#--------------------------------------------------------------------------||--#
def Reader( fname ):
  reader = None
  if("vtu"  in fname) : reader = vtk.vtkXMLUnstructuredGridReader()
  if("vtp"  in fname) : reader = vtk.vtkXMLPolyDataReader()
  if("pvd"  in fname) : reader = vtk.vtkPVDReader()
  if("pvtp" in fname) : reader = vtk.vtkXMLPPolyDataReader()
  if("pvtu" in fname) : reader = vtk.vtkXMLPUnstructuredGridReader()
  if("vti"  in fname) : reader = vtk.vtkXMLImageDataReader()
  if("vtm"  in fname) : reader = vtk.vtkXMLMultiBlockDataReader()
  if("obj"  in fname) : reader = vtk.vtkOBJReader()
  if("stl"  in fname) : reader = vtk.vtkSTLReader() 

  assert( reader )
  reader.SetFileName( fname );
  reader.UpdateInformation()
  reader.GetOutputInformation(0).Set(vtk.vtkStreamingDemandDrivenPipeline.UPDATE_NUMBER_OF_PIECES(), nranks)
  reader.GetOutputInformation(0).Set(vtk.vtkStreamingDemandDrivenPipeline.UPDATE_PIECE_NUMBER(), rank)
  reader.Update()

  dataObject     = reader.GetOutputDataObject(0);
  ClassName      = dataObject.GetClassName()
  print("\t[Reader] File:'%s' ClassName:'%s' " % (fname, ClassName) )

  try :
    NumberOfTimeSteps   = reader.GetNumberOfTimeSteps();
    NumberOfPointArrays = reader.GetNumberOfPointArrays();
  except :
    pass

  try :
    NumberOfPoints = dataObject.GetNumberOfPoints()
    NumberOfCells  = dataObject.GetNumberOfCells()
    print("\t         NumberOfPoints:%d NumberOfCells:%d NumberOfPointArrays:%d " % (NumberOfPoints, NumberOfCells, NumberOfPointArrays) )
  except :
    pass

  return dataObject 


#--------------------------------------------------------------------------||--#
def Writer( vtkObj, fname, Ascii=False):
  writer    = None
  extension = None

  if vtkObj.IsA("vtkPolyData"         ) : writer = vtk.vtkXMLPolyDataWriter()
  if vtkObj.IsA("vtkUnstructuredGrid" ) : writer = vtk.vtkXMLUnstructuredGridWriter()
  if vtkObj.IsA("vtkMultiBlockDataSet") : writer = vtk.vtkXMLMultiBlockDataWriter()
  if vtkObj.IsA("vtkHyperTreeGrid"    ) : writer = vtk.vtkXMLHyperTreeGridWriter()
  if vtkObj.IsA("vtkRectilinearGrid"  ) : writer = vtk.vtkXMLRectilinearGridWriter()
  if vtkObj.IsA("vtkImageData"        ) : writer = vtk.vtkXMLImageDataWriter()
  if vtkObj.IsA("vtkStructuredGrid"   ) : writer = vtk.vtkXMLStructuredGridWriter()
  #if vtkObj.IsA(""  ) : writer = vtk.vtkXML Writer()

  # DATA_TIME_STEP
  try :
    writer.SetFileName( "%s.%s" % (fname,writer.GetDefaultFileExtension()) )
    writer.SetInputData( vtkObj )
    if(Ascii): writer.SetDataModeToAscii()
    writer.Write()
    print("\t[Writer] '%s' Saved!!" %(writer.GetFileName()) )
  except :
    print("\t[Writer] Class '%s' Not Saved!!" %( vtkObj.GetClassName() ) )
    #exit(0)
  return writer.GetFileName()


#--------------------------------------------------------------------------||--#
def Transform(vtkObj, Translate=[0.0,0.0,0.0], Scale=[1.0,1.0,1.0], Rotate=[0.0,0.0,0.0,0.0]):
  transform = vtk.vtkTransform()
  transform.Translate( Translate )
  transform.Scale( Scale )
  #transform.RotateY(0.0)
  transform.RotateWXYZ(Rotate[0], Rotate[1:])

 #tf = vtk.vtkTransformPolyDataFilter()
  tf = vtk.vtkTransformFilter()
  tf.SetInputData( vtkObj )
  tf.SetTransform( transform )
  tf.Update()
  return tf.GetOutputDataObject(0)


#--------------------------------------------------------------------------||--#
def GetBlocks( vtm, block=None ):
  iBlock = 0
  Blocks = {}

  Iter   = vtm.NewIterator()
  Iter.InitTraversal();
  while not Iter.IsDoneWithTraversal() :
    Blocks[iBlock] = Iter.GetCurrentDataObject()
    Iter.GoToNextItem()
    iBlock += 1

  return Blocks


def AppendVtusDic( vtkObjDic ):
  append = vtk.vtkAppendFilter()
  for k in sorted(vtkObjDic) : append.AddInputData( vtkObjDic[k]  )
  append.Update()
  return append.GetOutputDataObject(0)


def Vtx2Vtp( vtkObj ) :
  geometryFilter = vtk.vtkGeometryFilter()
  geometryFilter.SetInputData( vtkObj )
  geometryFilter.Update()
  return geometryFilter.GetOutputDataObject(0)


#--------------------------------------------------------------------------||--#
def GetTriangles( vtkObj ) :
  geometryFilter = vtk.vtkGeometryFilter()
  geometryFilter.SetInputData( vtkObj )
  geometryFilter.Update()

  vtp  = geometryFilter.GetOutput();
  tria = vtk.vtkTriangleFilter()
  tria.SetInputData( geometryFilter.GetOutput() )
  tria.Update()
  return vtp

#--------------------------------------------------------------------------||--#
def AppendVtps( vtkObjList ) :
  appendPolyData = vtk.vtkAppendPolyData()

  for vtkObj in vtkObjList : appendPolyData.AddInputData(vtkObj)
  appendPolyData.Update()

  cleaned = vtk.vtkCleanPolyData()
  cleaned.SetInputConnection(appendPolyData.GetOutputPort());
  cleaned.Update()

  return cleaned.GetOutputDataObject(0)


#--------------------------------------------------------------------------||--#
def AppendVtps( vtkObjList ) :
  appendPolyData = vtk.vtkAppendPolyData()

  for vtkObj in vtkObjList : appendPolyData.AddInputData(vtkObj)
  appendPolyData.Update()

  cleaned = vtk.vtkCleanPolyData()
  cleaned.SetInputConnection(appendPolyData.GetOutputPort());
  cleaned.Update()

  return cleaned.GetOutputDataObject(0)



#--------------------------------------------------------------------------||--#
def GetPolyDataNormals( Obj, Flip) :
  ns = vtk.vtkPolyDataNormals()
  ns.SetInputData( Obj )
  ns.ConsistencyOn()
  if Flip : ns.FlipNormalsOn()
  ns.ComputePointNormalsOn()
  ns.ComputeCellNormalsOff()
  ns.Update()
  return ns.GetOutputDataObject(0)


#--------------------------------------------------------------------------||--#
def VtkWarning( fname ) :
  outwin = vtk.vtkFileOutputWindow();
  outwin.SetFileName( fname );
  outwin.SetInstance( outwin );
  return


#--------------------------------------------------------------------------||--#
def GetCellsList( vtkObj ) :
  VtkTypes     = {}
  VtkTypes[ 5] = 'VTK_TRIANGLE'
  VtkTypes[ 9] = 'VTK_QUAD'
  VtkTypes[10] = 'VTK_TETRA'
  VtkTypes[14] = 'VTK_PYRAMID'
  VtkTypes[13] = 'VTK_WEDGE'
  VtkTypes[12] = 'VTK_HEXAHEDRON'
  VtkTypes[42] = 'VTK_POLYHEDRON'
  VtkTypes[11] = 'VTK_VOXEL'


  CellVertices = []
  CellTypes = []
  vtk_n_cells = vtkObj.GetNumberOfCells()
  for idx in range(vtk_n_cells):
    cell_ids = vtk.vtkIdList()
    vtkObj.GetCellPoints(idx, cell_ids)
    n_cell_ids   = cell_ids.GetNumberOfIds()
    cellVertices = np.array([cell_ids.GetId(j) for j in range(n_cell_ids)])
    cellType     = vtkObj.GetCellType(idx)
    CellVertices.append( cellVertices )
    CellTypes.append( cellType )

  TypesDic = getDicFromArray(CellTypes); #print( TypesDic)
  TypesDic = {VtkTypes.get(k,"???"):v for k,v in TypesDic.items()}
  print("\t[GetCellsList] Found Types: ", list(TypesDic.keys()) )
  return CellVertices,CellTypes


#--------------------------------------------------------------------------||--#
#--------------------------------------------------------------------------||--#

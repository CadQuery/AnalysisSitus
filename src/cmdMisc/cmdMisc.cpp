//-----------------------------------------------------------------------------
// Created on: 23 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// cmdMisc includes
#include <cmdMisc.h>

// asiTcl includes
#include <asiTcl_PluginMacro.h>

// asiEngine includes
#include <asiEngine_Model.h>

// OCCT includes
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pln.hxx>
#include <STEPControl_Reader.hxx>
#include <Geom_Plane.hxx>

TopoDS_Shape StepReader(const char *filename)
{
  //cout << string(filename)<<endl;
  STEPControl_Reader reader;
  auto readerSuccess = reader.ReadFile(filename);
  if (readerSuccess!=IFSelect_ReturnStatus::IFSelect_RetDone){
  //string message = "Step Read Failed";
  //throw std::runtime_error(message);
  }
  // auto readerSuccess = reader.ReadFile(filename);
  cout << "Success = " << readerSuccess << endl;
  reader.PrintCheckLoad(Standard_False, IFSelect_PrintCount::IFSelect_CountByItem);
  // Loads file MyFile.stp

  // gets the number of transferable roots
  cout << "Number of roots in STEP file: " << reader.NbRootsForTransfer() << endl;
  Standard_Integer NbTrans = reader.TransferRoots();
  // translates all transferable roots, and returns the number of //successful translations
  cout << "STEP roots transferred: " << NbTrans << endl;

  cout << "Number of resulting shapes is: " << reader.NbShapes() << endl;
  auto result = reader.OneShape();
  return result;
}

//-----------------------------------------------------------------------------

int BOX(const Handle(asiTcl_Interp)& interp,
        int                          argc,
        const char**                 argv)
{
  TopoDS_Shape box = BRepPrimAPI_MakeBox(1, 1, 1);
  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "Box created");
  interp->GetPlotter().DRAW_SHAPE(box, "box");
  return 0;
}

//-----------------------------------------------------------------------------

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <TColgp_Array2OfPnt.hxx>

TopoDS_Shape intersectionShape(TopoDS_Shape srf1,TopoDS_Shape srf2)
{
	BRepAlgoAPI_Section asect(srf1,srf2,Standard_False);
	asect.Approximation(Standard_True);
	asect.Build();
	TopoDS_Shape R = asect.Shape();
	return R;
}

std::vector<gp_Pnt> slicePnts(TopoDS_Shape& sbendStep, gp_Pln& pln, gp_Dir& normalPl,
                              gp_Vec& refDir,
                              ActAPI_PlotterEntry plotter)
{
  TopoDS_Shape facePln = BRepBuilderAPI_MakeFace(pln);   
  TopoDS_Shape intShape = intersectionShape(sbendStep, facePln);
  //DBRep::Set("intshape", intShape);

  // extract slice edges
  
  TopTools_IndexedMapOfShape edgesWireMap;   
  TopExp::MapShapes(intShape, TopAbs_EDGE, edgesWireMap); 
  cout << "edge extent per slice is " << edgesWireMap.Extent() << endl;    
  //TopTools_Array1OfShape edgesWire(1, edgesWireMap.Extent());
  TopTools_ListOfShape listTool;
  Handle(TopTools_HSequenceOfShape) edgesList = new TopTools_HSequenceOfShape;
  
  Standard_Real first = 0.;
  Standard_Real last = 1.;
  BRep_Tool fromEdgeToCurve;
  std::vector<Handle(Geom_Curve)> edgeWireCurve;

  for ( int k = 0; k < edgesWireMap.Extent(); ++k)
    edgeWireCurve.push_back(NULL);

  std::vector<Standard_Real> listOfCurveLength;
  
  for(Standard_Integer k = 1; k <= edgesWireMap.Extent(); k++)
  {   
    listTool.Append(edgesWireMap.FindKey(k));
    edgesList->Append(TopoDS::Edge(edgesWireMap.FindKey(k)));
    //edgesWire.SetValue(k, TopoDS::Edge(edgesWireMap.FindKey(k)));
    edgeWireCurve[k-1] = fromEdgeToCurve.Curve(TopoDS::Edge(edgesWireMap.FindKey(k)), first, last);              
    BRepAdaptor_Curve BAC(TopoDS::Edge(edgesWireMap.FindKey(k)));          
    GCPnts_UniformAbscissa curveLengthTool (BAC, 2, 0., 1., -1);
    listOfCurveLength.push_back(curveLengthTool.Abscissa());
  }
  
  // get a single ordered wire
  
  Handle(TopTools_HSequenceOfShape) wireList;
  ShapeAnalysis_FreeBounds freeBounds;  
  freeBounds.ConnectEdgesToWires(edgesList, 3., true, wireList);
  cout << "wireList length is " << wireList->Length() << endl;
  TopoDS_Wire resultingWire = TopoDS::Wire(wireList->Value(1));
  
  
  // convert the wire to the bspline curve  
  
  BRepAdaptor_CompCurve wireConverter(resultingWire, false);
  Handle(Adaptor3d_HCurve) adaptor3d =  wireConverter.Trim(wireConverter.FirstParameter(), wireConverter.LastParameter(), 10e-6);   
  Handle(Geom_BSplineCurve) bsWire = GeomConvert_ApproxCurve(adaptor3d, 10e-6, GeomAbs_C1, 30, 14).Curve();
  //DrawTrSurf::Set("bspline" ,bsCurve);

  plotter.DRAW_CURVE(bsWire, Color_Red, "bsWire");
  
  // uniform sampling of the points onto the wire
  
  int totalSlicePnts = 48;
  std::vector<gp_Pnt>  sampledPoints;

  BRepAdaptor_Curve BAC(BRepBuilderAPI_MakeEdge(bsWire).Edge());           
  GCPnts_UniformAbscissa pntSamplingTool (BAC, totalSlicePnts, -1); 

  gp_Pnt P0;
  bsWire->D0(pntSamplingTool.Parameter(1), P0);
  sampledPoints.push_back(P0);

  gp_Vec ori;
  bool isReverse = false;
  for ( int pt_idx = 2; pt_idx <= pntSamplingTool.NbPoints(); ++pt_idx )
  {
    gp_Pnt aPnt;
    bsWire->D0(pntSamplingTool.Parameter(pt_idx), aPnt);

    if ( refDir.Magnitude() > RealEpsilon() && pt_idx == 2 )
    {
      ori = aPnt.XYZ() - P0.XYZ();
      double a1 = refDir.Angle( ori );
      double a2 = refDir.Angle( ori.Reversed() );
      //
      std::cout << "\ta1 = " << a1 << std::endl;
      std::cout << "\ta2 = " << a2 << std::endl;
      //
      if ( Abs(a2) < Abs(a1) )
        isReverse = true;
    }

    if ( isReverse )
      sampledPoints.insert(sampledPoints.begin(), aPnt);
    else
      sampledPoints.push_back(aPnt);
  }    

  refDir = sampledPoints[1].XYZ() - sampledPoints[0].XYZ();
  
  // pushback again the first point
  
  //sampledPoints.push_back(sampledPoints.at(0));
   
  // extract the design parameters of the generic section
  
  cout << "sampledPoints size is " << sampledPoints.size() << endl;
  
  // dump to draw
  
  //di.Eval("axo");
  //di.Eval("fit");  
  for(int pntInd = 0; pntInd < sampledPoints.size(); ++pntInd)
  {
    //di.Eval("wait 2");	
  
    TCollection_AsciiString name("projPnt"); name += (pntInd + 1);
   // DrawTrSurf::Set( name.ToCString(), sampledPoints[pntInd] );  
  }
  
 cout << "pnts displayed " << endl;
 
 return sampledPoints;
}

//-----------------------------------------------------------------------------

#define nbSlices 15
#define maxSlice nbSlices

int TEST(const Handle(asiTcl_Interp)& interp,
         int                          argc,
         const char**                 argv)
{
  TopoDS_Shape sbendStep = StepReader("D:/projects/IODA/sbend/sbend.stp");

  interp->GetProgress().SendLogMessage(LogInfo(Normal) << "sbend created");
  interp->GetPlotter().DRAW_SHAPE(sbendStep, "sbend");
   
  // extract sbend slice
  gp_Dir normalPl(0.437226519089472, -1.74144587639091e-11, -0.899351416858229);  
  std::vector<gp_Pln> pln; 
  std::vector<Handle(Geom_Plane)> planePln;  
  
  //pln.push_back(gp_Pln(gp_Pnt(570.570007014, 79.2900000001, (365.)), normalPl));
  //planePln.push_back(new Geom_Plane(pln[0]));  
    
  gp_Pnt firstPnt;
  std::vector<gp_Pnt> pnts;
    
  // evaluate first point of the first slice
  
  gp_Pln firstPln(gp_Pnt(570.570007014, 79.2900000001, (365.)), normalPl);
  
  gp_Vec refDir;
  std::vector<gp_Pnt> firstPnts = slicePnts(sbendStep, firstPln, normalPl, refDir, interp->GetPlotter());
  firstPnt = firstPnts[0];
  TColgp_Array2OfPnt pntsMatrix(1, firstPnts.size(), 1, maxSlice);   
  for(Standard_Integer n = 0; n < maxSlice; ++n)
  {
	pln.push_back(gp_Pln(gp_Pnt(570.570007014, 79.2900000001, (365. -n*5.)), normalPl));
    planePln.push_back(new Geom_Plane(pln.at(n)));  
        
    pnts = slicePnts(sbendStep, pln.at(n), normalPl, refDir, interp->GetPlotter());
        
    for(int firstIndx = 0; firstIndx < pnts.size(); ++firstIndx)
    {
	  pntsMatrix.SetValue(firstIndx+1, n+1, pnts[firstIndx]);
	  
    TCollection_AsciiString name("pnt s"); name += n;
    //interp->GetPlotter().DRAW_POINT(pnts[firstIndx], Color_Red, name);
	  //TCollection_AsciiString name("pnts"); name += (firstIndx + 1);
      //DrawTrSurf::Set( name.ToCString(), pntsMatrix.Value(firstIndx+1, n+1) );
	  			
	} 
  }
  
  cout << "pntsMatrix colLength is " << pntsMatrix.ColLength() << endl;
  cout << "pntsMatrix rowLength is " << pntsMatrix.RowLength() << endl;

  //return 1;

  // =============================================================================================================
  
  // PntsMatrix is the matrix of the pnts uniformly sampled onto the step file. I need to reorder them properly:
  
  // =============================================================================================================


	TColgp_Array2OfPnt finalPntsMatrix(1, firstPnts.size(), 1, maxSlice);
	//for(int k = 1; k <= pntsMatrix.ColLength(); ++k) finalPntsMatrix.SetValue(k, 1, pntsMatrix.Value(k, 1));
  //finalPntsMatrix = pntsMatrix;

  // Copy first column as-as. It will play as a reference.
  for ( int r = 1; r <= pntsMatrix.ColLength(); ++r )
    finalPntsMatrix.SetValue( r, 1, pntsMatrix(r, 1) );

  const int numPtsInCol = pntsMatrix.ColLength();

  gp_Pnt refPoint = pntsMatrix(1, 1);

  // Copy all other rows
  for ( int c = 2; c <= pntsMatrix.RowLength(); ++c )
  {
    double minDist = DBL_MAX;

    int startRow = 0;

    interp->GetPlotter().DRAW_POINT(refPoint, Color_Yellow, "refPoint");

    // Find index to start
    for ( int r = 1; r < pntsMatrix.ColLength(); ++r )
    {
      if ( pntsMatrix(r, c).Distance(refPoint) < minDist )
      {
        minDist = pntsMatrix(r, c).Distance(refPoint);
        startRow = r;
      }
    }

    refPoint = pntsMatrix(startRow, c);

    // Copy points starting from the found column index
    int pt = 0;
    int rr = startRow - 1;
    do
    {
      pt++;
      rr++;

      if ( rr > pntsMatrix.ColLength() )
        rr -= pntsMatrix.ColLength();

      TCollection_AsciiString name("reordered pnt col = "); name += c;
      //interp->GetPlotter().DRAW_POINT(pntsMatrix(rr, c), Color_Red, name);

      finalPntsMatrix.SetValue( pt, c, pntsMatrix(rr, c) );
    }
    while ( pt < numPtsInCol );
  }



	//std::vector<gp_Pnt> pntsVec;
	//double minDist[maxSlice];	  
	//int minDistInx[maxSlice];	
	//gp_Pnt pnt1[maxSlice];
	//for (int arraInd = 0; arraInd < maxSlice; ++arraInd)
	//{
	//  minDist[arraInd] = 0.;	  
	//  minDistInx[arraInd] = 1;
	//}
	//int n = 1;
	//while(n < maxSlice)  //for(int n = 1; n < maxSlice; ++n)
	//{
	//  pnt1[n-1] = pntsMatrix.Value(1, n);
	//  	
	//  for(int cmpInd = 2; cmpInd <= pntsMatrix.ColLength(); ++cmpInd) 
	//  {
	//	//minDist.assign(n-1, 0.);	  
	//    //minDistInx.assign(n-1, 1);
	//    minDist[n-1] = pnt1[n-1].Distance(pntsMatrix.Value(1, n+1));
	//    if(pnt1[n-1].Distance(pntsMatrix.Value(cmpInd, n+1)) < minDist[n-1]) 
	//    {
	//       minDist[n-1] = pnt1[n-1].Distance(pntsMatrix.Value(cmpInd, n+1));
	//       minDistInx[n-1] = cmpInd;
	//    }		    
	//  }
	//  
	//  cout << "minDist is " << minDist[n-1] << endl;
	//  cout << "minDistInx is " << minDistInx[n-1] << endl;
	//  
	//  int indPnts2 = 0;
 //     for(indPnts2 = minDistInx[n-1]; indPnts2 <= pntsMatrix.ColLength(); ++indPnts2) 
 //     {
	//    //pntsVec.push_back(pntsMatrix(indPnts2, n+2));
	//    finalPntsMatrix.SetValue(indPnts2-minDistInx[n-1]+1, n+1, pntsMatrix.Value(indPnts2, n+1));
 //     }
 //     int indPnts3 = 0;
	//  for(indPnts3 = 2; indPnts3 <= minDistInx[n-1]; ++indPnts3) finalPntsMatrix.SetValue(pntsMatrix.ColLength()-minDistInx[n-1]+indPnts3, n+1, pntsMatrix.Value(indPnts3-1, n+1)); 
	//  //pntsVec.push_back(pntsMatrix(indPnts3, n+2));//		  
	//++n;
	//} 
  
  Standard_Integer uDegree = 3;	
  Standard_Integer uNumberCP = finalPntsMatrix.ColLength();
  Standard_Real numberUknots = uNumberCP - (uDegree - 1);

  TColStd_Array1OfReal uKnots(1, numberUknots);
  uKnots(1)= 0.;
  uKnots(numberUknots)= 1.;
   
  for (Standard_Integer uknotParam = 2; uknotParam < numberUknots; ++uknotParam)   
  {
    uKnots(uknotParam)= (uknotParam-1)/(numberUknots-1); 
    //cout << "knots(" << knotParam << ") is " << (knotParam-1)/(numberKnots-1)  << endl;
  }
   
  TColStd_Array1OfInteger uMults(1,numberUknots);
  uMults(1) = uDegree+1;
  uMults(numberUknots) = uDegree+1;
    
  for (Standard_Integer u_multParam = 2; u_multParam < numberUknots; ++u_multParam)
  {
	uMults(u_multParam) = 1; 
  }  
  
  Standard_Integer vDegree = 3;	
  Standard_Integer vNumberCP = finalPntsMatrix.RowLength();
  Standard_Real numberVknots = vNumberCP - (vDegree - 1);

  TColStd_Array1OfReal vKnots(1, numberVknots);
  vKnots(1)= 0.;
  vKnots(numberVknots)= 1.;
   
  for (Standard_Integer vknotParam = 2; vknotParam < numberVknots; ++vknotParam)   
  {
    vKnots(vknotParam)= (vknotParam-1)/(numberVknots-1); 
    //cout << "knots(" << knotParam << ") is " << (knotParam-1)/(numberKnots-1)  << endl;
  }
   
  TColStd_Array1OfInteger vMults(1,numberVknots);
  vMults(1) = vDegree+1;
  vMults(numberVknots) = vDegree+1;
    
  for (Standard_Integer v_multParam = 2; v_multParam < numberVknots; ++v_multParam)
  {
	vMults(v_multParam) = 1; 
  }    
  
  //GeomAPI_PointsToBSplineSurface pntsToBS(finalPntsMatrix, 0.1, 0.1, 0.1, 9, GeomAbs_C1, 1.0e-3);
  //Handle(Geom_BSplineSurface) bsplineSurf = pntsToBS.Surface();
  Handle(Geom_BSplineSurface) bsplineSurf = new Geom_BSplineSurface(finalPntsMatrix, uKnots, vKnots, uMults, vMults, uDegree, vDegree, false, false); //pntsToBS.Surface();
  //if(pntsToBS.IsDone())cout << "surface done " << endl;
  //DrawTrSurf::Set("finalSurf", bsplineSurf);
  interp->GetPlotter().REDRAW_SURFACE("finalSurf", bsplineSurf, Color_White);
  
  TopoDS_Face face = BRepBuilderAPI_MakeFace (bsplineSurf, 10e-3);
  interp->GetPlotter().REDRAW_SHAPE("finalFace", face, Color_White);
  //DBRep::Set("face", face);*/
  
    
  return 0;
}

//-----------------------------------------------------------------------------

void cmdMisc::Factory(const Handle(asiTcl_Interp)&      interp,
                      const Handle(Standard_Transient)& data)
{
  // Add commands
  interp->AddCommand("box", "", __FILE__, BOX);
  //
  interp->AddCommand("test", "", __FILE__, TEST);
}

// Declare entry point PLUGINFACTORY
ASIPLUGIN(cmdMisc)

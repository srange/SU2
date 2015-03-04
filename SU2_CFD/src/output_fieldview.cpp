/*!
 * \file output_fieldview.cpp
 * \brief Main subroutines for output solver information.
 * \author F. Palacios, T. Economon, M. Colonno
 * \version 3.2.8.3 "eagle"
 *
 * SU2 Lead Developers: Dr. Francisco Palacios (fpalacios@stanford.edu).
 *                      Dr. Thomas D. Economon (economon@stanford.edu).
 *
 * SU2 Developers: Prof. Juan J. Alonso's group at Stanford University.
 *                 Prof. Piero Colonna's group at Delft University of Technology.
 *                 Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *                 Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *                 Prof. Rafael Palacios' group at Imperial College London.
 *
 * Copyright (C) 2012-2015 SU2, the open-source CFD code.
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/output_structure.hpp"

void COutput::SetFieldView_ASCII(CConfig *config, CGeometry *geometry, CSolver **solver, unsigned short val_iZone, unsigned short val_nZone, bool surf_sol) {
  
  /*--- Local variables and initialization ---*/
  
  unsigned short iDim, iVar, nDim = geometry->GetnDim();
  unsigned short Kind_Solver = config->GetKind_Solver();
  
  unsigned long iPoint, iElem, iNode;
  unsigned long iExtIter = config->GetExtIter();
  bool adjoint = config->GetAdjoint();
  
  char cstr[200], buffer[50];
  string filename;
  
  /*--- Write file name with extension ---*/
  
  if (surf_sol) {
    if (adjoint) filename = config->GetSurfAdjCoeff_FileName();
    else filename = config->GetSurfFlowCoeff_FileName();
  }
  else {
    if (adjoint)
      filename = config->GetAdj_FileName();
    else filename = config->GetFlow_FileName();
  }
  
  if (Kind_Solver == LINEAR_ELASTICITY) {
    if (surf_sol) filename = config->GetSurfStructure_FileName().c_str();
    else filename = config->GetStructure_FileName().c_str();
  }
  
  if (Kind_Solver == WAVE_EQUATION) {
    if (surf_sol) filename = config->GetSurfWave_FileName().c_str();
    else filename = config->GetWave_FileName().c_str();
  }
  
  if (Kind_Solver == HEAT_EQUATION) {
    if (surf_sol) filename = config->GetSurfHeat_FileName().c_str();
    else filename = config->GetHeat_FileName().c_str();
  }
  
  if (Kind_Solver == POISSON_EQUATION)
    filename = config->GetStructure_FileName().c_str();
  
  strcpy (cstr, filename.c_str());
  if (Kind_Solver == POISSON_EQUATION) strcpy (cstr, config->GetStructure_FileName().c_str());
  
  /*--- Special cases where a number needs to be appended to the file name. ---*/
  if ((Kind_Solver == EULER || Kind_Solver == NAVIER_STOKES || Kind_Solver == RANS) &&
      (val_nZone > 1) && (config->GetUnsteady_Simulation() != TIME_SPECTRAL)) {
    sprintf (buffer, "_%d", int(val_iZone));
    strcat(cstr,buffer);
  }
  
  /*--- Special cases where a number needs to be appended to the file name. ---*/
  if (((Kind_Solver == ADJ_EULER) || (Kind_Solver == ADJ_NAVIER_STOKES) || (Kind_Solver == ADJ_RANS)) &&
      (val_nZone > 1) && (config->GetUnsteady_Simulation() != TIME_SPECTRAL)) {
    sprintf (buffer, "_%d", int(val_iZone));
    strcat(cstr,buffer);
  }
  
  if (config->GetUnsteady_Simulation() == TIME_SPECTRAL) {
    
    /*--- SU2_SOL requires different names. It is only called for parallel cases. ---*/
    if (config->GetKind_SU2() == SU2_SOL) {
      val_iZone = iExtIter;
    }
    if (int(val_iZone) < 10) sprintf (buffer, "_0000%d.uns", int(val_iZone));
    if ((int(val_iZone) >= 10) && (int(val_iZone) < 100)) sprintf (buffer, "_000%d.uns", int(val_iZone));
    if ((int(val_iZone) >= 100) && (int(val_iZone) < 1000)) sprintf (buffer, "_00%d.uns", int(val_iZone));
    if ((int(val_iZone) >= 1000) && (int(val_iZone) < 10000)) sprintf (buffer, "_0%d.uns", int(val_iZone));
    if (int(val_iZone) >= 10000) sprintf (buffer, "_%d.uns", int(val_iZone));
    
  } else if (config->GetUnsteady_Simulation() && config->GetWrt_Unsteady()) {
    if (int(iExtIter) < 10) sprintf (buffer, "_0000%d.uns", int(iExtIter));
    if ((int(iExtIter) >= 10) && (int(iExtIter) < 100)) sprintf (buffer, "_000%d.uns", int(iExtIter));
    if ((int(iExtIter) >= 100) && (int(iExtIter) < 1000)) sprintf (buffer, "_00%d.uns", int(iExtIter));
    if ((int(iExtIter) >= 1000) && (int(iExtIter) < 10000)) sprintf (buffer, "_0%d.uns", int(iExtIter));
    if (int(iExtIter) >= 10000) sprintf (buffer, "_%d.uns", int(iExtIter));
  } else {
    sprintf (buffer, ".uns");
  }
  
  strcat(cstr,buffer);
  
  /*--- Open FieldView ASCII file and write the header. ---*/
  ofstream FieldView_File;
  FieldView_File.open(cstr, ios::out);
  FieldView_File.precision(6);
  
  FieldView_File << "FIELDVIEW 3 0" << endl;
  
  /*--- Output constants for time, fsmach, alpha and re. ---*/
  
  FieldView_File << "Constants" << endl;
  FieldView_File << config->GetExtIter() <<"\t"<< config->GetMach() <<"\t"<< config->GetAoA() <<"\t"<< config->GetReynolds() << endl;
  
  /*--- Output the number of grids. ---*/
  
  unsigned short ngrids = 1;
  FieldView_File << "Grids\t" << ngrids << endl;
  
  /*--- Output the table of boundary types, starting with the number of types.
   Note that this differs from the binary/unformatted specification.
   Each boundary type name is preceded by 3 integer flags.
   The first flag indicates whether this boundary type is a wall.
   A flag value of 1 indicates a wall, and a value of 0 indicates
   a non-wall.  Walls are significant for streamline calculation.
   The second flag indicates whether the boundary type has surface
   results.  A value of 1 means surface results will be present for
   this boundary type (if any boundary variables are specified in the
   Boundary Variable Names section below).  A value of 0 means no surface
   results will be present.
   The third flag indicates whether boundary faces of this type have
   consistent "clockness" for the purpose of calculating a surface
   normal.  A value of 1 means that all faces of this type are
   written following a "right hand rule" for clockness.  In other
   words, if the vertices are written on counter-clockwise:
   4 --- 3
   |     |
   1 --- 2
   then the normal to the face is pointing towards you (not away
   from you).  A value of 0 means that the faces do not have any
   consistent clockness.  The "clockness" of surface normals is
   only used for calculating certain special surface integrals
   that involve surface normals.  If the surface normals flag
   is 0, these special integrals will not be available. ---*/
  
  FieldView_File << "Boundary Table\t1" << endl;
  FieldView_File << "1\t0\t1\tBoundary" << endl;

  /*--- Output the table of variable names, starting with the number of
   variables.  The number of variables can be zero.
   Note that vector variables are specified by a ';' and vector name
   following the first scalar name of 3 scalar components of the
   vector.  If writing 2-D results, the third component must still
   be provided here, and its values must be written in the variables
   section below (typically padded with zeros.) ---*/
  
  unsigned short nbvars, nvars;
  
  if (config->GetKind_SU2() == SU2_SOL) {
    
    /*--- If SU2_SOL called this routine, we already have a set of output
     variables with the appropriate string tags stored in the config class. ---*/
    
    nvars = config->fields.size() - 1 - nDim;
    
    FieldView_File << "Variable Names\t" << nvars << endl;
    
    for (unsigned short iField = 1+nDim; iField < config->fields.size(); iField++) {
      FieldView_File << config->fields[iField] << endl;
    }
    
    nbvars = config->fields.size() - 1 - nDim;

    FieldView_File << "Boundary Variable Names\t" << nbvars << endl;
    
    for (unsigned short iField = 1+nDim; iField < config->fields.size(); iField++) {
      FieldView_File << config->fields[iField] << endl;
    }
    
  } else {
    
    nvars = nVar_Consv + nDim;
    
    FieldView_File << "Variable Names\t" << nvars-nDim << endl;
    
    for (iVar = 0; iVar < nVar_Consv; iVar++) {
      FieldView_File << "Conservative_" << iVar+1 << endl;
    }
    
    nbvars = nVar_Consv + nDim;

    FieldView_File << "Boundary Variable Names\t" << nbvars-nDim << endl;
    
    for (iVar = 0; iVar < nVar_Consv; iVar++) {
      FieldView_File << "Conservative_" << iVar+1 << endl;
    }
    
  }
  
//        Output the node definition section for this grid.
  
  FieldView_File << "Nodes\t" << nGlobal_Poin << endl;

//        Output the X, Y, Z coordinates of successive nodes.
//        Note that this differs from the binary/unformatted specification.
  
  for (iPoint = 0; iPoint < nGlobal_Poin; iPoint++) {
    
    if (config->GetKind_SU2() != SU2_SOL) {
      for (iDim = 0; iDim < nDim; iDim++)
        FieldView_File << scientific << Coords[iDim][iPoint] << "\t";
    }
    else {
      for (iVar = 0; iVar < nVar_Total; iVar++)
        FieldView_File << scientific << Data[iVar][iPoint] << "\t";
    }
    FieldView_File << endl;
    
  }

//        Output the boundary face definitions.
//        Note that this differs from the binary/unformatted specification.
//        Each face is preceded by an index into the boundary table at the
//        top of the file and the number of face vertices, 3 or 4.
//        All faces here have 4 vertices.  If the face is triangular,
//        the last vertex should be zero.
//        TIP: FIELDVIEW assumes that boundary faces are not in random
//        order.  It assumes that faces of the same type tend to occur
//        in groups.  If your boundary faces are in random order, you
//        may want to output them one boundary type at a time.  This
//        will give you better performance (less memory, greater speed)
//        in FIELDVIEW.
  
  unsigned long nbfaces = nGlobal_Line + nGlobal_BoundTria + nGlobal_BoundQuad;
  
  FieldView_File << "Boundary Faces\t" << nbfaces << endl;

  for (iElem = 0; iElem < nGlobal_Line; iElem++) {
    iNode = iElem*N_POINTS_LINE;
    FieldView_File << "1\t2\t" << Conn_Line[iNode+0] << "\t";
    FieldView_File << "1\t2\t" << Conn_Line[iNode+1] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_BoundTria; iElem++) {
    iNode = iElem*N_POINTS_TRIANGLE;
    FieldView_File << "1\t3\t" << Conn_BoundTria[iNode+0] << "\t";
    FieldView_File << Conn_BoundTria[iNode+1] << "\t";
    FieldView_File << Conn_BoundTria[iNode+2] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_BoundQuad; iElem++) {
    iNode = iElem*N_POINTS_QUADRILATERAL;
    FieldView_File << "1\t4\t" << Conn_BoundQuad[iNode+0] << "\t";
    FieldView_File << Conn_BoundQuad[iNode+1] << "\t";
    FieldView_File << Conn_BoundQuad[iNode+2] << "\t";
    FieldView_File << Conn_BoundQuad[iNode+3] << "\n";
  }
  
//        Output the elements section for this grid.
//        Note that this differs from the binary/unformatted specification.
//        It contains the headers and node definitions of all elements.
//        In this example, each element starts with 2 for type 'hex',
//        with a subtype of 1 (the only subtype currently supported).
//        This is followed by the node indices for the element.
//
  
  FieldView_File << "Elements" << endl;
  
  for (iElem = 0; iElem < nGlobal_Tria; iElem++) {
    iNode = iElem*N_POINTS_TRIANGLE;
    FieldView_File <<"2\t1\t"<< Conn_Tria[iNode+0] << "\t";
    FieldView_File << Conn_Tria[iNode+1] << "\t";
    FieldView_File << Conn_Tria[iNode+2] << "\t";
    FieldView_File << Conn_Tria[iNode+2] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_Quad; iElem++) {
    iNode = iElem*N_POINTS_QUADRILATERAL;
    FieldView_File <<"2\t1\t"<< Conn_Quad[iNode+0] << "\t";
    FieldView_File << Conn_Quad[iNode+1] << "\t";
    FieldView_File << Conn_Quad[iNode+2] << "\t";
    FieldView_File << Conn_Quad[iNode+3] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_Tetr; iElem++) {
    iNode = iElem*N_POINTS_TETRAHEDRON;
    FieldView_File <<"2\t1\t"<< Conn_Tetr[iNode+0] << "\t" << Conn_Tetr[iNode+1] << "\t";
    FieldView_File << Conn_Tetr[iNode+2] << "\t" << Conn_Tetr[iNode+2] << "\t";
    FieldView_File << Conn_Tetr[iNode+3] << "\t" << Conn_Tetr[iNode+3] << "\t";
    FieldView_File << Conn_Tetr[iNode+3] << "\t" << Conn_Tetr[iNode+3] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_Hexa; iElem++) {
    iNode = iElem*N_POINTS_HEXAHEDRON;
    FieldView_File <<"2\t1\t"<< Conn_Hexa[iNode+0] << "\t" << Conn_Hexa[iNode+1] << "\t";
    FieldView_File << Conn_Hexa[iNode+2] << "\t" << Conn_Hexa[iNode+3] << "\t";
    FieldView_File << Conn_Hexa[iNode+4] << "\t" << Conn_Hexa[iNode+5] << "\t";
    FieldView_File << Conn_Hexa[iNode+6] << "\t" << Conn_Hexa[iNode+7] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_Wedg; iElem++) {
    iNode = iElem*N_POINTS_WEDGE;
    FieldView_File <<"2\t1\t"<< Conn_Wedg[iNode+0] << "\t" << Conn_Wedg[iNode+1] << "\t";
    FieldView_File << Conn_Wedg[iNode+1] << "\t" << Conn_Wedg[iNode+2] << "\t";
    FieldView_File << Conn_Wedg[iNode+3] << "\t" << Conn_Wedg[iNode+4] << "\t";
    FieldView_File << Conn_Wedg[iNode+4] << "\t" << Conn_Wedg[iNode+5] << "\n";
  }
  
  for (iElem = 0; iElem < nGlobal_Pyra; iElem++) {
    iNode = iElem*N_POINTS_PYRAMID;
    FieldView_File <<"2\t1\t"<< Conn_Pyra[iNode+0] << "\t" << Conn_Pyra[iNode+1] << "\t";
    FieldView_File << Conn_Pyra[iNode+2] << "\t" << Conn_Pyra[iNode+3] << "\t";
    FieldView_File << Conn_Pyra[iNode+4] << "\t" << Conn_Pyra[iNode+4] << "\t";
    FieldView_File << Conn_Pyra[iNode+4] << "\t" << Conn_Pyra[iNode+4] << "\n";
  }
  
//        Output the variables data for this grid.
//        Note that all of the data for the first variable is output
//        before any of the data for the second variable.
//        You should skip this section if the number of variables is zero.
//        The variables must be in the same order as the "Variable Names"
//        section.
  
  FieldView_File << "Variables" << endl;

  /*--- Loop over the vars/residuals and write the values to file ---*/
  for (iVar = nDim; iVar < nVar_Total; iVar++) {
    for (iPoint = 0; iPoint < nGlobal_Poin; iPoint++) {
      FieldView_File << scientific << Data[iVar][iPoint] << endl;
    }
  }
  
//        Output the boundary variables data for this grid.
//        Note that all of the data for the first variable is output
//        before any of the data for the second variable.
//        Remember that the Boundary Table above has a "surface results
//        flag" indicating which boundary types have surface results.
//        The data should be written in the same order as the faces in
//        the Boundary Faces section, skipping over faces whose boundary
//        type has a surface results flag of zero (false).
//        For each variable, you should write one number per boundary face.
//        You should skip this section if the number of boundary
//        variables is zero.
  
  FieldView_File << "Boundary Variables" << endl;

  
  FieldView_File.close();
  
}

void COutput::SetFieldView_MeshASCII(CConfig *config, CGeometry *geometry, bool surf_sol, bool new_file) { }

void COutput::SetFieldView_MeshBinary(CConfig *config, CGeometry *geometry, unsigned short val_iZone) { }

void COutput::SetFieldView_SurfaceMesh(CConfig *config, CGeometry *geometry, unsigned short val_iZone) { }

void COutput::SetFieldView_Solution(CConfig *config, CGeometry *geometry, unsigned short val_iZone) { }

void COutput::SetFieldView_SurfaceSolution(CConfig *config, CGeometry *geometry, unsigned short val_iZone) { }
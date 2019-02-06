// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.

#include "../config/config.hpp"

#include "fem.hpp"
#include "fespace_ext.hpp"
#include "kernels/fespace.hpp"

// *****************************************************************************
namespace mfem
{

// *****************************************************************************
// * FiniteElementSpaceExtension
// *****************************************************************************
FiniteElementSpaceExtension::FiniteElementSpaceExtension(FiniteElementSpace *f)
   :fes(f),
    globalDofs(f->GetNDofs()),
    localDofs(f->GetFE(0)->GetDof()),
    offsets(globalDofs+1),
    indices(localDofs*f->GetNE()),
    map(localDofs*f->GetNE())
{
   const FiniteElement *fe = f->GetFE(0);
   const TensorBasisElement* el = dynamic_cast<const TensorBasisElement*>(fe);
   MFEM_ASSERT(el, "Finite element not supported with partial assembly");
   const Array<int> &dof_map = el->GetDofMap();
   const bool dof_map_is_identity = (dof_map.Size()==0);
   const Table& e2dTable = f->GetElementToDofTable();
   const int* elementMap = e2dTable.GetJ();
   const int elements = f->GetNE();
   // We'll be keeping a count of how many local nodes point to its global dof
   for (int i = 0; i <= globalDofs; ++i)
   {
      offsets[i] = 0;
   }
   for (int e = 0; e < elements; ++e)
   {
      for (int d = 0; d < localDofs; ++d)
      {
         const int gid = elementMap[localDofs*e + d];
         ++offsets[gid + 1];
      }
   }
   // Aggregate to find offsets for each global dof
   for (int i = 1; i <= globalDofs; ++i)
   {
      offsets[i] += offsets[i - 1];
   }
   // For each global dof, fill in all local nodes that point   to it
   for (int e = 0; e < elements; ++e)
   {
      for (int d = 0; d < localDofs; ++d)
      {
         const int did = dof_map_is_identity?d:dof_map[d];
         const int gid = elementMap[localDofs*e + did];
         const int lid = localDofs*e + d;
         indices[offsets[gid]++] = lid;
         map[lid] = gid;
      }
   }
   // We shifted the offsets vector by 1 by using it as a counter
   // Now we shift it back.
   for (int i = globalDofs; i > 0; --i)
   {
      offsets[i] = offsets[i - 1];
   }
   offsets[0] = 0;
}

// ***************************************************************************
void FiniteElementSpaceExtension::L2E(const Vector& lVec,
                                      Vector& eVec) const
{
   const int vdim = fes->GetVDim();
   const int localEntries = localDofs * fes->GetNE();
   const bool vdim_ordering = fes->GetOrdering() == Ordering::byVDIM;
   kernels::fem::L2E(vdim,
                     vdim_ordering,
                     globalDofs,
                     localEntries,
                     offsets,
                     indices,
                     lVec,
                     eVec);
}

// ***************************************************************************
// Aggregate local node values to their respective global dofs
void FiniteElementSpaceExtension::E2L(const Vector& eVec,
                                      Vector& lVec) const
{
   const int vdim = fes->GetVDim();
   const int localEntries = localDofs * fes->GetNE();
   const bool vdim_ordering = fes->GetOrdering() == Ordering::byVDIM;
   kernels::fem::E2L(vdim,
                     vdim_ordering,
                     globalDofs,
                     localEntries,
                     offsets,
                     indices,
                     eVec,
                     lVec);
}

} // mfem

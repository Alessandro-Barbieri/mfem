// Copyright (c) 2010-2022, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#ifndef MFEM_ELASTICITY_GRADIENT_TYPE_HPP
#define MFEM_ELASTICITY_GRADIENT_TYPE_HPP

enum class GradientType
{
   Symbolic,
   EnzymeFwd, // Enzyme forward mode
   EnzymeRev, // Enzyme reverse mode
   FiniteDiff, // finite differences
   DualNumbers // native dual number forward mode
};

#endif

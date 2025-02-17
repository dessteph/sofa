/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/gpu/cuda/CudaTypes.h>
#include <sofa/gpu/cuda/CudaStandardTetrahedralFEMForceField.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/behavior/ForceField.inl>
#include <sofa/component/solidmechanics/fem/hyperelastic/StandardTetrahedralFEMForceField.inl>

namespace sofa
{

namespace gpu
{

namespace cuda
{

int StandardTetrahedralFEMForceFieldCudaClass = core::RegisterObject("Supports GPU-side computations using CUDA")
.add< sofa::component::solidmechanics::fem::hyperelastic::StandardTetrahedralFEMForceField<CudaVec3fTypes> >()
#ifdef SOFA_GPU_CUDA_DOUBLE
.add< sofa::component::solidmechanics::fem::hyperelastic::StandardTetrahedralFEMForceField<CudaVec3dTypes> >()
#endif
;


} // namespace cuda

} // namespace gpu


template class SOFA_GPU_CUDA_API sofa::component::solidmechanics::fem::hyperelastic::StandardTetrahedralFEMForceField<sofa::gpu::cuda::CudaVec3fTypes>;
#ifdef SOFA_GPU_CUDA_DOUBLE
template class SOFA_GPU_CUDA_API sofa::component::solidmechanics::fem::hyperelastic::StandardTetrahedralFEMForceField<sofa::gpu::cuda::CudaVec3dTypes>;
#endif

} // namespace sofa

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
#include <sofa/linearalgebra/EigenMatrixManipulator.h>

namespace sofa::linearalgebra
{

LLineManipulator& LLineManipulator::addCombination(unsigned int idxConstraint, SReal factor)
{
    _data.push_back(std::make_pair(idxConstraint, factor));
    return *this;
}

void LMatrixManipulator::init(const SparseMatrixEigen& L)
{
    const auto numConstraint = L.rows();
    const auto numDofs = L.cols();
    LMatrix.resize(numConstraint,SparseVectorEigen(numDofs));
    for (unsigned int i=0; i<LMatrix.size(); ++i) LMatrix[i].reserve(numDofs*3/10);
    for (int k=0; k<L.outerSize(); ++k)
    {
        for (SparseMatrixEigen::InnerIterator it(L,k); it; ++it)
        {
            const auto row=it.row();
            const auto col=it.col();
            const SReal value=it.value();
            LMatrix[row].insert(col)=value;
        }
    }
    for (unsigned int i=0; i<LMatrix.size(); ++i) LMatrix[i].finalize();
}



void LMatrixManipulator::buildLMatrix(const type::vector<LLineManipulator> &lines, SparseMatrixEigen& matrix) const
{
    for (unsigned int l=0; l<lines.size(); ++l)
    {
        const LLineManipulator& lManip=lines[l];
        SparseVectorEigen vector;
        lManip.buildCombination(LMatrix,vector);
        matrix.startVec(l);
        for (SparseVectorEigen::InnerIterator it(vector); it; ++it)
        {
            matrix.insertBack(l,it.index())=it.value();
        }
    }
}

type::vector< SparseVectorEigen > LMatrix;

} // namespace sofa::linearalgebra

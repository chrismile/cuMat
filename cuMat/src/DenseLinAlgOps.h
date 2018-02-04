#ifndef __CUMAT_DENSE_LIN_ALG_OPS_H__
#define __CUMAT_DENSE_LIN_ALG_OPS_H__

#include "Macros.h"
#include "ForwardDeclarations.h"
#include "Constants.h"
#include "Context.h"
#include "MatrixBase.h"

/*
 * This file contains general expression for linear algebra operations:
 * Determinant, inverse, solver.
 * The purpose is to delegate to explicit solutions for matrices up to 4x4
 * and then to delegate to decompositions for larger matrices
 */

CUMAT_NAMESPACE_BEGIN


namespace
{

    // MATRIX CLASS

    template<typename _Scalar, int _Dims, int _Flags>
    struct DeviceMatrix;

    template<typename _Scalar, int _Flags>
    struct alignas(_Scalar) DeviceMatrix<_Scalar, 1, _Flags>
    {
        _Scalar m00;

        template<typename T>
        __device__ CUMAT_STRONG_INLINE void load(const T& mat, Index batch)
        {
            m00 = mat.coeff(0, 0, batch);
        }
        template<typename T>
        __device__ CUMAT_STRONG_INLINE void store(T& matrix, Index batch)
        {
            matrix.setRawCoeff(batch, m00);
        }
    };

    template<typename _Scalar>
    struct alignas(_Scalar) DeviceMatrix<_Scalar, 2, Flags::RowMajor>
    {
        _Scalar m00; //1st row
        _Scalar m01;
        _Scalar m10; //2nd row
        _Scalar m11;

        template<typename T>
        __device__ CUMAT_STRONG_INLINE void load(const T& mat, Index batch)
        {
            m00 = mat.coeff(0, 0, batch);
            m01 = mat.coeff(0, 1, batch);
            m10 = mat.coeff(1, 0, batch);
            m11 = mat.coeff(1, 1, batch);
        }
        template<typename T>
        __device__ CUMAT_STRONG_INLINE void store(T& matrix, Index batch)
        {
            matrix.setRawCoeff(0 + 4*batch, m00);
            matrix.setRawCoeff(1 + 4 * batch, m01);
            matrix.setRawCoeff(2 + 4 * batch, m10);
            matrix.setRawCoeff(3 + 4 * batch, m11);
        }
    };
    template<typename _Scalar>
    struct alignas(_Scalar) DeviceMatrix<_Scalar, 2, Flags::ColumnMajor>
    {
        _Scalar m00; //1st column
        _Scalar m10;
        _Scalar m01; //2nd column
        _Scalar m11;

        template<typename T>
        __device__ CUMAT_STRONG_INLINE void load(const T& mat, Index batch)
        {
            m00 = mat.coeff(0, 0, batch);
            m10 = mat.coeff(1, 0, batch);
            m01 = mat.coeff(0, 1, batch);
            m11 = mat.coeff(1, 1, batch);
        }
        template<typename T>
        __device__ CUMAT_STRONG_INLINE void store(T& matrix, Index batch)
        {
            matrix.setRawCoeff(0 + 4 * batch, m00);
            matrix.setRawCoeff(1 + 4 * batch, m10);
            matrix.setRawCoeff(2 + 4 * batch, m01);
            matrix.setRawCoeff(3 + 4 * batch, m11);
        }
    };

    template<typename _Scalar>
    struct alignas(_Scalar) DeviceMatrix<_Scalar, 3, Flags::RowMajor>
    {
        _Scalar m00; //1st row
        _Scalar m01;
        _Scalar m02;
        _Scalar m10; //2nd row
        _Scalar m11;
        _Scalar m12;
        _Scalar m20; //3rd row
        _Scalar m21;
        _Scalar m22;

        template<typename T>
        __device__ CUMAT_STRONG_INLINE void load(const T& mat, Index batch)
        {
            m00 = mat.coeff(0, 0, batch);
            m01 = mat.coeff(0, 1, batch);
            m02 = mat.coeff(0, 2, batch);
            m10 = mat.coeff(1, 0, batch);
            m11 = mat.coeff(1, 1, batch);
            m12 = mat.coeff(1, 2, batch);
            m20 = mat.coeff(2, 0, batch);
            m21 = mat.coeff(2, 1, batch);
            m22 = mat.coeff(2, 2, batch);
        }
        template<typename T>
        __device__ CUMAT_STRONG_INLINE void store(T& matrix, Index batch)
        {
            matrix.setRawCoeff(0 + 9 * batch, m00);
            matrix.setRawCoeff(1 + 9 * batch, m01);
            matrix.setRawCoeff(2 + 9 * batch, m02);
            matrix.setRawCoeff(3 + 9 * batch, m10);
            matrix.setRawCoeff(4 + 9 * batch, m11);
            matrix.setRawCoeff(5 + 9 * batch, m12);
            matrix.setRawCoeff(6 + 9 * batch, m20);
            matrix.setRawCoeff(7 + 9 * batch, m21);
            matrix.setRawCoeff(8 + 9 * batch, m22);
        }
    };
    template<typename _Scalar>
    struct alignas(_Scalar) DeviceMatrix<_Scalar, 3, Flags::ColumnMajor>
    {
        _Scalar m00; //1st column
        _Scalar m10;
        _Scalar m20;
        _Scalar m01; //2nd column
        _Scalar m11;
        _Scalar m21;
        _Scalar m02; //3rd column
        _Scalar m12;
        _Scalar m22;

        template<typename T>
        __device__ CUMAT_STRONG_INLINE void load(const T& mat, Index batch)
        {
            m00 = mat.coeff(0, 0, batch);
            m10 = mat.coeff(1, 0, batch);
            m20 = mat.coeff(2, 0, batch);
            m01 = mat.coeff(0, 1, batch);
            m11 = mat.coeff(1, 1, batch);
            m21 = mat.coeff(2, 1, batch);
            m02 = mat.coeff(0, 2, batch);
            m12 = mat.coeff(1, 2, batch);
            m22 = mat.coeff(2, 2, batch);
        }
        template<typename T>
        __device__ CUMAT_STRONG_INLINE void store(T& matrix, Index batch)
        {
            matrix.setRawCoeff(0 + 9 * batch, m00);
            matrix.setRawCoeff(1 + 9 * batch, m10);
            matrix.setRawCoeff(2 + 9 * batch, m20);
            matrix.setRawCoeff(3 + 9 * batch, m01);
            matrix.setRawCoeff(4 + 9 * batch, m11);
            matrix.setRawCoeff(5 + 9 * batch, m21);
            matrix.setRawCoeff(6 + 9 * batch, m02);
            matrix.setRawCoeff(7 + 9 * batch, m12);
            matrix.setRawCoeff(8 + 9 * batch, m22);
        }
    };

    // LOAD + STORE

    template <int Dims, typename Scalar, int Rows, int Cols, int Batches, int Flags>
    __device__ CUMAT_STRONG_INLINE DeviceMatrix<Scalar, Dims, Flags> loadMat(const Matrix<Scalar, Rows, Cols, Batches, Flags>& mat, Index index)
    {
        const DeviceMatrix<Scalar, Dims, Flags>* data = reinterpret_cast<DeviceMatrix<Scalar, Dims, Flags>*>(mat.data());
        return data[index];
    }
    template <int Dims, typename T, 
        typename Scalar = typename internal::traits<T>::Scalar, int Flags = internal::traits<T>::Flags>
    __device__ CUMAT_STRONG_INLINE DeviceMatrix<Scalar, Dims, Flags> loadMat(const T& mat, Index index)
    {
        DeviceMatrix<Scalar, Dims, Flags> m;
        m.load(mat, index);
        return m;
    }

    template <int Dims, typename Scalar, int Rows, int Cols, int Batches, int Flags>
    __device__ CUMAT_STRONG_INLINE void storeMat(Matrix<Scalar, Rows, Cols, Batches, Flags>& mat, const DeviceMatrix<Scalar, Dims, Flags>& out, Index index)
    {
        DeviceMatrix<Scalar, Dims, Flags>* data = reinterpret_cast<DeviceMatrix<Scalar, Dims, Flags>*>(mat.data());
        data[index] = out;
    }
    template <int Dims, typename T,
        typename Scalar = typename internal::traits<T>::Scalar, int Flags = internal::traits<T>::Flags>
    __device__ CUMAT_STRONG_INLINE void storeMat(T& mat, const DeviceMatrix<Scalar, Dims, Flags>& out, Index index)
    {
        out.store(mat, index);
    }

    // DETERMINANT

    template<typename Scalar, int Dims, typename Input>
    struct DeterminantFunctor;
    template<typename Scalar, typename Input>
    struct DeterminantFunctor<Scalar, 1, Input>
    {
        static __device__ CUMAT_STRONG_INLINE Scalar run(const Input& mat)
        {
            return mat.m00;
        }
    };
    template<typename Scalar, typename Input>
    struct DeterminantFunctor<Scalar, 2, Input>
    {
        static __device__ CUMAT_STRONG_INLINE Scalar run(const Input& mat)
        {
            return mat.m00*mat.m11 - mat.m10*mat.m01;
        }
    };
    template<typename Scalar, typename Input>
    struct DeterminantFunctor<Scalar, 3, Input>
    {
        static __device__ CUMAT_STRONG_INLINE Scalar run(const Input& mat)
        {
            return mat.m00*mat.m11*mat.m22 + mat.m01*mat.m12*mat.m20 + mat.m02*mat.m10*mat.m21
                 - mat.m02*mat.m11*mat.m20 - mat.m01*mat.m10*mat.m22 - mat.m00*mat.m12*mat.m21;
        }
    };
    template <typename T, typename M, int Dims,
        typename Scalar = typename internal::traits<T>::Scalar,
        int TFlags = internal::traits<T>::Flags, int MFlags = internal::traits<M>::Flags>
    __global__ void DeterminantKernel(dim3 virtual_size, const T expr, M matrix)
    {
        CUMAT_KERNEL_1D_LOOP(index, virtual_size) {
            DeviceMatrix<Scalar, Dims, TFlags> in = loadMat<Dims, T, Scalar>(expr, index);
            Scalar det = DeterminantFunctor<Scalar, Dims, DeviceMatrix<Scalar, Dims, TFlags> >::run(in);
            matrix.setRawCoeff(index, det);
        }
    }
}


namespace internal
{
    template<typename _Child>
    struct traits<DeterminantOp<_Child> >
    {
        using Scalar = typename internal::traits<_Child>::Scalar;
        enum
        {
            Flags = internal::traits<_Child>::Flags,
            RowsAtCompileTime = 1,
            ColsAtCompileTime = 1,
            BatchesAtCompileTime = internal::traits<_Child>::BatchesAtCompileTime,
            InputSize = std::max(internal::traits<_Child>::RowsAtCompileTime, internal::traits<_Child>::ColsAtCompileTime),
            AccessFlags = InputSize<=3 ? ReadCwise : 0 //for matrices <=3, we also support cwise evaluation
        };
    };
}

template<typename _Child>
class DeterminantOp : public MatrixBase<DeterminantOp<_Child> >
{
public:
    typedef MatrixBase<DeterminantOp<_Child> > Base;
    using Scalar = typename internal::traits<_Child>::Scalar;
    enum
    {
        Flags = internal::traits<_Child>::Flags,
        Rows = 1,
        Columns = 1,
        Batches = internal::traits<_Child>::BatchesAtCompileTime,
        InputSize = std::max(internal::traits<_Child>::RowsAtCompileTime, internal::traits<_Child>::ColsAtCompileTime)
    };

private:
    const _Child matrix_;

public:
    explicit DeterminantOp(const MatrixBase<_Child>& matrix)
        : matrix_(matrix.derived())
    {}

    __host__ __device__ CUMAT_STRONG_INLINE Index rows() const { return 1; }
    __host__ __device__ CUMAT_STRONG_INLINE Index cols() const { return 1; }
    __host__ __device__ CUMAT_STRONG_INLINE Index batches() const { return matrix_.batches(); }

private:
    template<typename Derived>
    void evalImpl(Derived& m, std::integral_constant<int, 1>) const
    {
        //we need at least cwise-read
        typedef typename MatrixReadWrapper<_Child, ReadCwise>::type Child_wrapped;
        Child_wrapped inWrapped(matrix_);

        //launch kernel
        Context& ctx = Context::current();
        KernelLaunchConfig cfg = ctx.createLaunchConfig1D(m.batches());
        DeterminantKernel<Child_wrapped, Derived, 1> <<<cfg.block_count, cfg.thread_per_block, 0, ctx.stream() >>>(cfg.virtual_size, inWrapped, m);
        CUMAT_CHECK_ERROR();
    }

    template<typename Derived>
    void evalImpl(Derived& m, std::integral_constant<int, 2>) const
    {
        //we need at least cwise-read
        typedef typename MatrixReadWrapper<_Child, ReadCwise>::type Child_wrapped;
        Child_wrapped inWrapped(matrix_);

        //launch kernel
        Context& ctx = Context::current();
        KernelLaunchConfig cfg = ctx.createLaunchConfig1D(m.batches());
        DeterminantKernel<Child_wrapped, Derived, 2> <<<cfg.block_count, cfg.thread_per_block, 0, ctx.stream() >>>(cfg.virtual_size, inWrapped, m);
        CUMAT_CHECK_ERROR();
    }

    template<typename Derived>
    void evalImpl(Derived& m, std::integral_constant<int, 3>) const
    {
        //we need at least cwise-read
        typedef typename MatrixReadWrapper<_Child, ReadCwise>::type Child_wrapped;
        Child_wrapped inWrapped(matrix_);

        //launch kernel
        Context& ctx = Context::current();
        KernelLaunchConfig cfg = ctx.createLaunchConfig1D(m.batches());
        DeterminantKernel<Child_wrapped, Derived, 3> <<<cfg.block_count, cfg.thread_per_block, 0, ctx.stream() >>>(cfg.virtual_size, inWrapped, m);
        CUMAT_CHECK_ERROR();
    }

    template<typename Derived, int DynamicSize>
    void evalImpl(Derived& m, std::integral_constant<int, DynamicSize>) const
    {
        int size = matrix_.rows();
        CUMAT_ASSERT(matrix_.rows() == matrix_.cols());

        if (size <= 3)
        {
            //now we need to evaluate the input to direct read
            typedef typename MatrixReadWrapper<_Child, ReadDirect>::type Child_wrapped;
            Child_wrapped inWrapped(matrix_);
            //short-cuts
            Context& ctx = Context::current();
            KernelLaunchConfig cfg = ctx.createLaunchConfig1D(m.batches());
            if (size == 1)
            {
                DeterminantKernel<Child_wrapped, Derived, 1> <<<cfg.block_count, cfg.thread_per_block, 0, ctx.stream() >>>(cfg.virtual_size, inWrapped, m);
            } else if (size == 2)
            {
                DeterminantKernel<Child_wrapped, Derived, 2> <<<cfg.block_count, cfg.thread_per_block, 0, ctx.stream() >>>(cfg.virtual_size, inWrapped, m);
            } else if (size == 3)
            {
                DeterminantKernel<Child_wrapped, Derived, 3> <<<cfg.block_count, cfg.thread_per_block, 0, ctx.stream() >>>(cfg.virtual_size, inWrapped, m);
            }
        } else
        {
            //use LU Decomposition
            LUDecomposition<_Child> lu(matrix_);
            lu.determinant().evalTo(m);
        }
    }

public:
    template<typename Derived>
    void evalTo(MatrixBase<Derived>& m) const
    {
        evalImpl(m.derived(), std::integral_constant<int, std::max(internal::traits<_Child>::RowsAtCompileTime, internal::traits<_Child>::ColsAtCompileTime)>());
    }

    template<int Dims = InputSize>
    __device__ CUMAT_STRONG_INLINE Scalar coeff(Index row, Index col, Index batch) const
    {
        CUMAT_STATIC_ASSERT(Dims <= 3, "Cwise-evaluation of the determinant is only supported for matrices of size <= 3x3");
        typedef typename DeviceMatrix<Scalar, Dims, internal::traits<_Child>::Flags> Mat_t;
        Mat_t in = loadMat(matrix_, batch);
        Scalar det = DeterminantFunctor<Scalar, Dims, Mat_t>::run(in);
        return det;
    }
};


CUMAT_NAMESPACE_END


#endif
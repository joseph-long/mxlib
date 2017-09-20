/** \file psdFilter.hpp
  * \brief Declares and defines a class for filtering with PSDs
  * \ingroup signal_processing_files
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  */


#ifndef psdFilter_hpp
#define psdFilter_hpp

#include <Eigen/Dense>

#include "../mxError.hpp"
#include "../fft/fft.hpp"

namespace mx
{
namespace sigproc 
{
   
/// A class for filtering noise with PSDs
/** The square-root of the PSD is maintained by this class, either as a pointer to an external array or using internally allocated memory (which will be
  * de-allocated on destruction. 
  * 
  * PSD Normalization: the PSD used for this needs to be normalized properly to produce filtered noise with the correct statistics.  Given an
  * array of size N which contains the PSD as "power per unit frequency" vs frequency (i.e. the PSD) on some frequency scale with uniform spacing \f$ \Delta f \f$,
  * the normalization is
  \f[
  PSD_{norm} = PSD * N * \Delta f 
  \f]
  * for a 1-dimensional PSD and  
  \f[
  PSD_{norm} = PSD * N_0 * \Delta f_0 * N_1 * \Delta f_1 
  \f]
  * for a 2-dimensional PSD. Remember that these are applied before taking the square root.
  *
  * 
  * \ingroup psds
  *
  * \todo once fftT has a plan interface with pointers for working memory, use it.
  */
template<typename _realT>
class psdFilter
{
public:
   
   typedef _realT realT; ///< Real floating point type
   typedef std::complex<_realT> complexT; ///< Complex floating point type.
   typedef Eigen::Array<realT, Eigen::Dynamic, Eigen::Dynamic> realArrayT; ///< Eigen array type with Scalar==realT 
   typedef Eigen::Array<complexT, Eigen::Dynamic, Eigen::Dynamic> complexArrayT; ///< Eigen array type with Scalar==complexT
   
   
protected:

   int _rows; ///< The number of rows in the filter, and the required number of rows in the noise array.
   int _cols; ///< The number of columns in the filter, and the required number of columns in the noise array.

   realArrayT * _psdSqrt; ///< Pointer to the real array containing the square root of the PSD.
   bool _owner; ///< Flag indicates whether or not _psdSqrt was allocated by this instance, and so must be deallocated.

   complexArrayT _ftWork;   ///< Working memory for the FFT.
   
   fftT< complexT, complexT,2,0> fft_fwd; ///< FFT object for the forward transform.
   fftT< complexT, complexT,2,0> fft_back; ///< FFT object for the backward transfsorm.
   

public:
   
   ///C'tor.
   psdFilter();
   
   ///Destructor
   ~psdFilter();
   
protected:

   ///Set the size of the filter.
   /** Handles allocation of the _ftWork array and fftw planning.
     *
     * Requires _psdSqrt to be set first.  This is called by the psdSqrt() and psd() methods.
     * 
     */
   int setSize();

public:   

   ///Get the number of rows in the filter
   /**
     * \returns the current value of _rows.
     */ 
   int rows();
   
   ///Get the number of columns in the filter
   /**
     * \returns the current value of _cols.
     */
   int cols();
   
   ///Set the sqaure-root of the PSD to be a pointer to an array containing the square root of the properly normalized PSD.
   /** This does not allocate _npsdSqrt, it merely points to the specified array, which remains your responsibility for deallocation, etc.
     *
     * See the discussion of PSD normalization above.
     * 
     * \returns 0 on success
     * \returns -1 on error
     * 
     */  
   int psdSqrt( realArrayT * npsdSqrt /**< [in] a pointer to an array containing the square root of the PSD. */ );
   
   ///Set the sqaure-root of the PSD.
   /** This allocates _npsdSqrt and fills it with th evalues in the array.
     *
     * See the discussion of PSD normalization above.
     * 
     * \returns 0 on success
     * \returns -1 on error
     */  
   int psdSqrt( const realArrayT & npsdSqrt /**< [in] an array containing the square root of the PSD.*/ );
   
   ///Set the sqaure-root of the PSD from the PSD.
   /** This allocates _npsdSqrt and fills it with the square root of the values in the array.
     *
     * See the discussion of PSD normalization above.
     * 
     * \returns 0 on success
     * \returns -1 on error
     */  
   int psd( const realArrayT & npsd /**< [in] an array containing the PSD */ );

   ///De-allocate all working memory and reset to initial state.
   void clear();
   
   
   ///Apply the filter.
   /**
     * 
     * \returns 0 on success
     * \returns -1 on error
     */ 
   int filter( realArrayT & noise /**< [in/out] the noise field of size rows() X cols(), which is filtered in-place. */);
   
   ///Apply the filter.
   /**
     * \returns 0 on success
     * \returns -1 on error
     */ 
   int operator()( realArrayT & noise /**< [in/out] the noise field of size rows() X cols(), which is filtered in-place. */ );
   
};

template<typename realT>
psdFilter<realT>::psdFilter()
{
   _rows = 0;
   _cols = 0;
   
   _psdSqrt = 0;
   _owner = false;
}

template<typename realT>
psdFilter<realT>::~psdFilter()
{
   if(_psdSqrt && _owner)
   {
      delete _psdSqrt;
   }
}

template<typename realT>
int psdFilter<realT>::setSize()
{
   if( _psdSqrt == 0)
   {
      mxError("psdFilter", MXE_PARAMNOTSET, "_psdSqrt has not been set yet, is still NULL.");
      return -1;
   }
   
   if( _rows == _psdSqrt->rows() && _cols == _psdSqrt->cols())
   {
      return 0;
   }
   
   _rows = _psdSqrt->rows();
   _cols = _psdSqrt->cols();
   
   _ftWork.resize(_rows, _cols);

   fft_fwd.plan(_rows, _cols, MXFFT_FORWARD, true);
      
   fft_back.plan(_rows, _cols, MXFFT_BACKWARD, true);      

   return 0;
}



template<typename realT>
int psdFilter<realT>::rows()
{
   return _rows;
}
   
template<typename realT>
int psdFilter<realT>::cols()
{
   return _cols;
}
   
template<typename realT>
int psdFilter<realT>::psdSqrt( realArrayT * npsdSqrt )
{
   if(_psdSqrt && _owner)
   {
      delete _psdSqrt;
   }
   
   _psdSqrt = npsdSqrt;
   _owner = false;
   
   setSize();
   
   return 0;
}

template<typename realT>
int psdFilter<realT>::psdSqrt( const realArrayT & npsdSqrt )
{
   if(_psdSqrt && _owner)
   {
      delete _psdSqrt;
   }
   
   _psdSqrt = new realArrayT;
   
   (*_psdSqrt) = npsdSqrt;
   _owner = true;
   
   setSize();
      
   return 0;
}

template<typename realT>
int psdFilter<realT>::psd( const realArrayT & npsd )
{
   if(_psdSqrt && _owner)
   {
      delete _psdSqrt;
   }
   
   _psdSqrt = new realArrayT;
   
   (*_psdSqrt) = npsd.sqrt();
   _owner = true;
   
   setSize();
      
   return 0;
}

template<typename realT>
void psdFilter<realT>::clear()
{
   _ftWork.resize(0,0);
   _rows = 0;
   _cols = 0;
      
   if(_psdSqrt && _owner)
   {
      delete _psdSqrt;
      _psdSqrt = 0;
   }
}
   
template<typename realT>
int psdFilter<realT>::filter( realArrayT & noise )
{
   //Make noise a complex number
   for(int ii=0;ii<noise.rows();++ii)
   {
      for(int jj=0; jj<noise.cols(); ++jj)
      {
         _ftWork(ii,jj) = complexT(noise(ii,jj),0);
      }
   }
   
   //Transform complex noise to Fourier domain.
   fft_fwd(_ftWork.data(), _ftWork.data() );
   
   //Apply the filter.
   _ftWork *= *_psdSqrt;
        
   fft_back(_ftWork.data(), _ftWork.data());
   
   //Now take the real part, and normalize.
   noise = _ftWork.real()/(noise.rows()*noise.cols());
   
   return 0;
}

template<typename realT>
int psdFilter<realT>::operator()( realArrayT & noise )
{
   return filter(noise);
}

} //namespace sigproc 
} //namespace mx

#endif //psdFilter_hpp
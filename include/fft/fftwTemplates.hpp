/** \file fftwTemplates.hpp
  * \brief Declares and defines templatized wrappers for the fftw library
  * \ingroup fft_files
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  */

//***********************************************************************//
// Copyright 2015, 2016, 2017 Jared R. Males (jaredmales@gmail.com)
//
// This file is part of mxlib.
//
// mxlib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// mxlib is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with mxlib.  If not, see <http://www.gnu.org/licenses/>.
//***********************************************************************//

#ifndef fftwTemplates_hpp
#define fftwTemplates_hpp

#include <complex>
#include <vector>

#include <fftw3.h>

#include "../mxlib.hpp"

namespace mx
{
/** \addtogroup fftw_templates
  *
  * A templatized interface to the fftw library.
  * 
  */

/** \defgroup fftw_template_types Types 
  * \brief Types and type specifications for FFTW Templates.
  *
  * \ingroup fftw_templates
  * 
  * @{
  */ 


///The complex float data type.
typedef std::complex<float>       complexFT;   

///The complex double data type.
typedef std::complex<double>      complexDT;

///The complex long double data type.
typedef std::complex<long double> complexLT;

#ifdef HASQUAD
///The complex __float128 data type.
typedef std::complex<__float128>  complexQT;
#endif

//****** Plans ******//

/** \defgroup fftw_template_plan_specs Plan Specification 
  * \brief Plan specifications for FFTW Templates.
  *
  * \ingroup fftw_template_types
  * 
  * @{
  */

///Specify the type of the plan based on the real type of the data
/**
  * 
  * \tparam realT is the real floating point type.
  */ 
template<typename realT>
struct fftwPlanSpec;

//For doxygen only:
#ifdef __DOXY_ONLY__
template<typename realT>
struct fftwPlanSpec
{
   /// Specializations typedef planT as fftwf_plan, fftw_plan, fftwl_plan, or fftwq_plan.
   typedef fftwX_plan planT;
};
#endif

///Specialization of fftwPlanSpec for float
template<>
struct fftwPlanSpec<float>
{
   /// Specifies fftwf_plan as planT
   typedef fftwf_plan planT;
};

///Specialization of fftwPlanSpec for double
template<>
struct fftwPlanSpec<double>
{
   /// Specifies fftw_plan as planT
   typedef fftw_plan planT;/// Specifies fftwf_plan as planT
};

///Specialization of fftwPlanSpec for long double
template<>
struct fftwPlanSpec<long double>
{
   /// Specifies fftwl_plan as planT
   typedef fftwl_plan planT;
};

#ifdef HASQUAD
///Specialization of fftwPlanSpec for __float128
template<>
struct fftwPlanSpec<__float128>
{
   /// Specifies fftwq_plan as planT
   typedef fftwq_plan planT;
};
#endif

///@}

//******* Type Specification *********//

/** \defgroup fftw_template_type_specs Type Specification 
  * \brief Type specifications for FFTW Templates.
  *
  * \ingroup fftw_template_types
  * 
  * @{
  */ 

///A structure specifying various types based on the FFT input and output data types.
/** Provides various typedefs to specify which fftw functions to compile.
  * 
  * See the specializations: 
  * \li   fftwTypeSpec<complexFT,complexFT>
  * \li   fftwTypeSpec<float, complexFT>
  * \li   fftwTypeSpec<complexFT, float>
  * \li   fftwTypeSpec<complexDT,complexDT>
  * \li   fftwTypeSpec<double, complexDT>
  * \li   fftwTypeSpec<complexDT, double>
  * \li   fftwTypeSpec<complexLT,complexLT>
  * \li   fftwTypeSpec<long double, complexLT>
  * \li   fftwTypeSpec<complexLT, long double>
  * \li   fftwTypeSpec<complexQT,complexQT>
  * \li   fftwTypeSpec<__float128, complexQT>
  * \li   fftwTypeSpec<complexQT, __float128>
  * 
  * \tparam _inputDataT is the data type of the input array
  * \tparam _outputDataT is the data type of the output array
  * 
  */
template<typename _inputDataT, typename _outputDataT>
struct fftwTypeSpec;

//For doxygen only:
#ifdef __DOXY_ONLY__
template<typename _inputDataT, typename _outputDataT>
struct fftwTypeSpec
{
   typedef _realT realT; ///< The real data type (_realT is actually defined in specializations).
   typedef std::complex<realT> complexT; ///< The complex data type
   typedef _inputDataT inputDataT; ///< The input array data type
   typedef _outputDataT outputDataT; ///< The output array data type
   typedef fftwPlanSpec<realT>::planT planT; ///< The plan type
};
#endif

///Specialization of fftwTypeSpec for complex-float input and complex-float output.
/** 
  */
template<>
struct fftwTypeSpec<complexFT, complexFT>
{
   typedef float realT; ///< The real data type
   typedef complexFT complexT; ///< The complex data type
   typedef complexFT inputDataT; ///< The input array data type
   typedef complexFT outputDataT; ///< The output array data type
   typedef fftwPlanSpec<float>::planT planT; ///< The plan type
};

///Specialization of fftwTypeSpec for real-float input and complex-float output
template<>
struct fftwTypeSpec<float, complexFT>
{
   typedef float realT; ///< The real data type
   typedef complexFT complexT; ///< The complex data type
   typedef float inputDataT; ///< The input array data type
   typedef complexFT outputDataT; ///< The output array data type
   typedef fftwPlanSpec<float>::planT planT; ///< The plan type
};

///Specialization of fftwTypeSpec for complex-float input and real-float output
template<>
struct fftwTypeSpec<complexFT, float>
{
   typedef float realT; ///< The real data type
   typedef complexFT complexT;
   typedef complexFT inputDataT;
   typedef float outputDataT;
   typedef fftwPlanSpec<float>::planT planT;
};

///Specialization of fftwTypeSpec for complex-double input and complex-double output
template<>
struct fftwTypeSpec<complexDT,complexDT>
{
   typedef double realT; ///< The real data type
   typedef complexDT complexT;
   typedef complexDT inputDataT;
   typedef complexDT outputDataT;
   typedef fftwPlanSpec<double>::planT planT;
};

///Specialization of fftwTypeSpec for real-double input and complex-double output
template<>
struct fftwTypeSpec<double,complexDT>
{
   typedef double realT; ///< The real data type
   typedef complexDT complexT;
   typedef double inputDataT;
   typedef complexDT outputDataT;
   typedef fftwPlanSpec<double>::planT planT;
};

///Specialization of fftwTypeSpec for complex-double input and double output
template<>
struct fftwTypeSpec<complexDT,double>
{
   typedef double realT; ///< The real data type
   typedef complexDT complexT;
   typedef complexDT inputDataT;
   typedef double outputDataT;
   typedef fftwPlanSpec<double>::planT planT;
};

///Specialization of fftwTypeSpec for complex-long-double input and complex-long-double output
template<>
struct fftwTypeSpec<complexLT,complexLT>
{
   typedef long double realT; ///< The real data type
   typedef complexLT complexT;
   typedef complexLT inputDataT;
   typedef complexLT outputDataT;
   typedef fftwPlanSpec<long double>::planT planT;
};

///Specialization of fftwTypeSpec for real-long-double input and complex-long-double output
template<>
struct fftwTypeSpec<long double,complexLT>
{
   typedef long double realT; ///< The real data type
   typedef complexLT complexT;
   typedef long double inputDataT;
   typedef complexLT outputDataT;
   typedef fftwPlanSpec<long double>::planT planT;
};

///Specialization of fftwTypeSpec for complex-long-double input and real-long-double output
template<>
struct fftwTypeSpec<complexLT,long double>
{
   typedef long double realT; ///< The real data type
   typedef complexLT complexT;
   typedef complexLT inputDataT;
   typedef long double outputDataT;
   typedef fftwPlanSpec<long double>::planT planT;
};

#ifdef HASQUAD
///Specialization of fftwTypeSpec for complex-quad input and complex-quad output
template<>
struct fftwTypeSpec<complexQT,complexQT>
{
   typedef __float128 realT; ///< The real data type
   typedef complexQT complexT;
   typedef complexQT inputDataT;
   typedef complexQT outputDataT;
   typedef fftwPlanSpec<__float128>::planT planT;
};

///Specialization of fftwTypeSpec for real-quad input and complex-quad output
template<>
struct fftwTypeSpec<__float128,complexQT>
{
   typedef __float128 realT; ///< The real data type
   typedef complexQT complexT;
   typedef __float128 inputDataT;
   typedef complexQT outputDataT;
   typedef fftwPlanSpec<__float128>::planT planT;
};

///Specialization of fftwTypeSpec for complex-quad input and real-quad output
template<>
struct fftwTypeSpec<complexQT,__float128>
{
   typedef __float128 realT; ///< The real data type
   typedef complexQT complexT;
   typedef complexQT inputDataT;
   typedef __float128 outputDataT;
   typedef fftwPlanSpec<__float128>::planT planT;
};
#endif

///@}
///@}

/** \defgroup fftw_template_wisdom Wisdom 
  * \brief Wrappers for working with fftw wisdom.
  *
  * \ingroup fftw_templates.
  * 
  * @{
  */ 

///Template wrapper for fftwX_import_system_wisdom();
/**
  * \retval 0 on failure
  * \retval 1 on success
  * 
  * \tparam realT the real floating point type.
  */ 
template<typename realT>
int fftw_import_system_wisdom();

//specializations don't need to be doxygen-ed:

//Template wrapper for fftwf_import_system_wisdom();
template<>
inline int fftw_import_system_wisdom<float>()
{
   return ::fftwf_import_system_wisdom();
}

//Template wrapper for fftw_import_system_wisdom();
template<>
inline int fftw_import_system_wisdom<double>()
{
   return ::fftw_import_system_wisdom();
}

//Template wrapper for fftwl_import_system_wisdom();
template<>
inline int fftw_import_system_wisdom<long double>()
{
   return ::fftwl_import_system_wisdom();
}

#ifdef HASQUAD
//Template wrapper for fftwq_import_system_wisdom();
template<>
inline int fftw_import_system_wisdom<__float128>()
{
   return ::fftwq_import_system_wisdom();
}
#endif

///Template wrapper for fftwX_import_wisdom_from_filename(const char *);
/**
  * \param filename is the name of the wisdom file
  * 
  * \retval 0 on failure
  * \retval 1 on success
  * 
  * \tparam realT the real floating point type.
  */
template<typename realT>
inline int fftw_import_wisdom_from_filename( const char * filename );


template<>
inline int fftw_import_wisdom_from_filename<float>( const char * filename )
{
   return ::fftwf_import_wisdom_from_filename( filename );
}


template<>
inline int fftw_import_wisdom_from_filename<double>( const char * filename )
{
   return ::fftw_import_wisdom_from_filename( filename );
}


template<>
inline int fftw_import_wisdom_from_filename<long double>( const char * filename )
{
   return ::fftwl_import_wisdom_from_filename( filename );
}

#ifdef HASQUAD
template<>
inline int fftw_import_wisdom_from_filename<__float128>( const char * filename )
{
   return ::fftwq_import_wisdom_from_filename( filename );
}
#endif

///Template wrapper for fftwX_export_wisdom_to_filename(const char *);
/**
  * \param filename is the name of the wisdom file
  * 
  * \retval 0 on failure
  * \retval 1 on success
  * 
  * \tparam realT the real floating point type.
  */
template<typename realT>
int fftw_export_wisdom_to_filename(const char * filename);


template<>
inline int fftw_export_wisdom_to_filename<float>(const char *filename)
{
   return ::fftwf_export_wisdom_to_filename( filename );
}


template<>
inline int fftw_export_wisdom_to_filename<double>(const char *filename)
{
   return ::fftw_export_wisdom_to_filename( filename );
}


template<>
inline int fftw_export_wisdom_to_filename<long double>(const char *filename)
{
   return ::fftwl_export_wisdom_to_filename( filename );
}

#ifdef HASQUAD
template<>
inline int fftw_export_wisdom_to_filename<__float128>(const char *filename)
{
   return ::fftwq_export_wisdom_to_filename( filename );
}
#endif

///@} fftw_template_wisdom


/** \defgroup fftw_template_alloc Allocation
  * \brief Wrappers for fftw memory allocation and de-allocation functions.
  *
  * \note Here the call to sizeof() is implicit, so you only specify the number of elements, not the number of bytes.
  * 
  * \ingroup fftw_templates.
  * 
  * @{
  */ 

//************ Allocation ************************//

///Call to fftw_malloc, with type cast.
/**
  * \note Here the call to sizeof() is implicit, so you only specify the number of elements, not the number of bytes.
  * 
  * \param n is the number of type realT elements, not the number of bytes.
  */ 
template<typename realT>
realT * fftw_malloc( size_t n);

template<>
inline float * fftw_malloc<float>(size_t n)
{
   return (float *) ::fftwf_malloc(n*sizeof(float));
}

template<>
inline complexFT * fftw_malloc<complexFT>(size_t n)
{
   return (complexFT *) ::fftwf_malloc(n*sizeof(complexFT));
}

template<>
inline double * fftw_malloc<double>(size_t n)
{
   return (double *) ::fftw_malloc(n*sizeof(double));
}

template<>
inline complexDT * fftw_malloc<complexDT>(size_t n)
{
   return (complexDT *) ::fftw_malloc(n*sizeof(complexDT));
}

template<>
inline long double * fftw_malloc<long double>(size_t n)
{
   return (long double *) ::fftwl_malloc(n*sizeof(long double));
}

template<>
inline complexLT * fftw_malloc<complexLT>(size_t n)
{
   return (complexLT *) ::fftwl_malloc(n*sizeof(complexLT));
}

#ifdef HASQUAD
template<>
inline __float128 * fftw_malloc<__float128>(size_t n)
{
   return (__float128 *) ::fftwq_malloc(n*sizeof(__float128));
}

template<>
inline complexQT * fftw_malloc<complexQT>(size_t n)
{
   return (complexQT *) ::fftwq_malloc(n*sizeof(complexQT));
}

#endif

//************ De-Allocation ************************//

///Call to fftw_free.
/**
  * \param p is a pointer to the array to de-allocate.
  */ 
template<typename realT>
void fftw_free( realT * p);

template<>
inline void fftw_free<float>( float * p)
{
   ::fftwf_free( p );
}

template<>
inline void fftw_free<complexFT>( complexFT * p)
{
   ::fftwf_free( p );
}

template<>
inline void fftw_free<double>( double * p)
{
   ::fftw_free( p );
}

template<>
inline void fftw_free<complexDT>( complexDT * p)
{
   ::fftw_free( p );
}

template<>
inline void fftw_free<long double>( long double * p)
{
   ::fftwl_free( p );
}

template<>
inline void fftw_free<complexLT>( complexLT * p)
{
   ::fftwl_free( p );
}

#ifdef HASQUAD
template<>
inline void fftw_free<__float128>( __float128 * p)
{
   ::fftwq_free( p );
}

template<>
inline void fftw_free<complexQT>( complexQT * p)
{
   ::fftwq_free( p );
}
#endif

///@} fftw_template_alloc

/** \defgroup fftw_template_plans Planning 
  * \brief Wrappers for working with fftw plans.
  *
  * \ingroup fftw_templates.
  * 
  * @{
  */ 

///Wrapper for fftwX_make_planner_thread_safe()
/**
  * \tparam realT the real floating point type
  */ 
template<typename realT>
void fftw_make_planner_thread_safe();


template<>
inline void fftw_make_planner_thread_safe<float>()
{
   ::fftwf_make_planner_thread_safe();
}


template<>
inline void fftw_make_planner_thread_safe<double>()
{
   ::fftw_make_planner_thread_safe();
}


template<>
inline void fftw_make_planner_thread_safe<long double>()
{
   ::fftwl_make_planner_thread_safe();
}

#ifdef HASQUAD
template<>
inline void fftw_make_planner_thread_safe<__float128>()
{
   ::fftwl_make_planner_thread_safe();
}
#endif

//********** Threads ****************//
/// Tell the FFTW planner how many threads to use.
/**
  * \tparam realT the real floating point type.
  */ 
template<typename realT>
void fftw_plan_with_nthreads(int nthreads /**< [in] the number of threads. Set to 1 to disable threads. */);

template<>
inline void fftw_plan_with_nthreads<float>(int nthreads)
{
   ::fftwf_plan_with_nthreads(nthreads);
}

template<>
inline void fftw_plan_with_nthreads<double>(int nthreads)
{
   ::fftw_plan_with_nthreads(nthreads);
}

template<>
inline void fftw_plan_with_nthreads<long double>(int nthreads)
{
   ::fftwl_plan_with_nthreads(nthreads);
}

#ifdef HASQUAD
template<>
inline void fftw_plan_with_nthreads<__float128>(int nthreads)
{
   ::fftwq_plan_with_nthreads(nthreads);
}
#endif

//********* Plan Creation **************//

///Wrapper for the fftwX_plan_dft functions
/**
  * \param n is a vector of ints containing the size of each dimension.
  * \param in is the input data array
  * \param out is the output data array
  * \param sign specifies forward or backwards, i.e. FFTW_FORWARD or FFTW_BACKWARD.
  * \param flags other fftw flags
  *
  * \returns an fftw plan for the types specified.
  * 
  * \tparam inputDataT the data type of the input array
  * \tparam outputDataT the data type of the output array
  */ 
template<typename inputDataT, typename outputDataT>
typename fftwTypeSpec<inputDataT,outputDataT>::planT fftw_plan_dft( std::vector<int> n, 
                                                                    inputDataT * in, 
                                                                    outputDataT * out,
                                                                    int sign,
                                                                    unsigned flags );

template<>
inline fftwTypeSpec<complexFT, complexFT>::planT fftw_plan_dft<complexFT, complexFT>( std::vector<int> n, 
                                                                                      complexFT * in, 
                                                                                      complexFT * out,
                                                                                      int sign,
                                                                                      unsigned flags )
{
   return ::fftwf_plan_dft( n.size(), n.data(), reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out), sign, flags);
}


template<>
inline fftwTypeSpec<float, complexFT>::planT fftw_plan_dft<float, complexFT>( std::vector<int> n, 
                                                                              float * in, 
                                                                              complexFT * out,
                                                                              int UNUSED(sign),
                                                                              unsigned flags )
{
   return ::fftwf_plan_dft_r2c( n.size(), n.data(), in, reinterpret_cast<fftwf_complex*>(out), flags);
}

template<>
inline fftwTypeSpec<complexFT, float>::planT fftw_plan_dft<complexFT, float>( std::vector<int> n, 
                                                                              complexFT * in, 
                                                                              float * out,
                                                                              int UNUSED(sign),
                                                                              unsigned flags )
{
   return ::fftwf_plan_dft_c2r( n.size(), n.data(), reinterpret_cast<fftwf_complex*>(in), out, flags);
}



template<>
inline fftwTypeSpec<complexDT, complexDT>::planT fftw_plan_dft<complexDT, complexDT>( std::vector<int> n, 
                                                                                      complexDT * in, 
                                                                                      complexDT * out,
                                                                                      int sign,
                                                                                      unsigned flags )
{
   return ::fftw_plan_dft( n.size(), n.data(), reinterpret_cast<fftw_complex*>(in), reinterpret_cast<fftw_complex*>(out), sign, flags);
}

template<>
inline fftwTypeSpec<double, complexDT>::planT fftw_plan_dft<double, complexDT>( std::vector<int> n, 
                                                                                double * in, 
                                                                                complexDT * out,
                                                                                int UNUSED(sign),
                                                                                unsigned flags )
{
   return ::fftw_plan_dft_r2c( n.size(), n.data(), in, reinterpret_cast<fftw_complex*>(out), flags);
}

template<>
inline fftwTypeSpec<complexDT, double>::planT fftw_plan_dft<complexDT, double>( std::vector<int> n, 
                                                                                complexDT * in, 
                                                                                double * out,
                                                                                int UNUSED(sign),
                                                                                unsigned flags )
{
   return ::fftw_plan_dft_c2r( n.size(), n.data(), reinterpret_cast<fftw_complex*>(in), out, flags);
}



template<>
inline fftwTypeSpec<complexLT, complexLT>::planT fftw_plan_dft<complexLT, complexLT>( std::vector<int> n, 
                                                                                      complexLT * in, 
                                                                                      complexLT * out,
                                                                                      int sign,
                                                                                      unsigned flags )
{
   return ::fftwl_plan_dft( n.size(), n.data(), reinterpret_cast<fftwl_complex*>(in), reinterpret_cast<fftwl_complex*>(out), sign, flags);
}

template<>
inline fftwTypeSpec<long double, complexLT>::planT fftw_plan_dft<long double, complexLT>( std::vector<int> n, 
                                                                                          long double * in, 
                                                                                          complexLT * out,
                                                                                          int UNUSED(sign),
                                                                                          unsigned flags )
{
   return ::fftwl_plan_dft_r2c( n.size(), n.data(), in, reinterpret_cast<fftwl_complex*>(out), flags);
}

template<>
inline fftwTypeSpec<complexLT, long double>::planT fftw_plan_dft<complexLT, long double>( std::vector<int> n, 
                                                                                          complexLT * in, 
                                                                                          long double * out,
                                                                                          int UNUSED(sign),
                                                                                          unsigned flags )
{
   return ::fftwl_plan_dft_c2r( n.size(), n.data(), reinterpret_cast<fftwl_complex*>(in), out, flags);
}

#ifdef HASQUAD
template<>
inline fftwTypeSpec<complexQT, complexQT>::planT fftw_plan_dft<complexQT, complexQT>( std::vector<int> n, 
                                                                                      complexQT * in, 
                                                                                      complexQT * out,
                                                                                      int sign,
                                                                                      unsigned flags )
{
   return ::fftwq_plan_dft( n.size(), n.data(), reinterpret_cast<fftwq_complex*>(in), reinterpret_cast<fftwq_complex*>(out), sign, flags);
}

template<>
inline fftwTypeSpec<__float128, complexQT>::planT fftw_plan_dft<__float128, complexQT>( std::vector<int> n, 
                                                                                        __float128 * in, 
                                                                                        complexQT * out,
                                                                                        int sign,
                                                                                        unsigned flags )
{
   return ::fftwq_plan_dft_r2c( n.size(), n.data(), in, reinterpret_cast<fftwq_complex*>(out), flags);
}

template<>
inline fftwTypeSpec<complexQT, __float128>::planT fftw_plan_dft<complexQT, __float128>( std::vector<int> n, 
                                                                                        complexQT * in, 
                                                                                        __float128 * out,
                                                                                        int sign,
                                                                                        unsigned flags )
{
   return ::fftwq_plan_dft_c2r( n.size(), n.data(), reinterpret_cast<fftwq_complex*>(in), out, flags);
}

#endif

/********* Cleanup *************/

///Cleanup persistent planner data.
template<typename realT>
void fftw_cleanup();

template<>
inline void fftw_cleanup<float>()
{
   ::fftwf_cleanup();
}

template<>
inline void fftw_cleanup<double>()
{
   ::fftw_cleanup();
}

template<>
inline void fftw_cleanup<long double>()
{
   ::fftwl_cleanup();
}

#ifdef HASQUAD
template<>
inline void fftw_cleanup<__float128>()
{
   ::fftwq_cleanup();
}
#endif

/********* Thread Cleanup *************/

///Cleanup persistent planner data and threads data.
template<typename realT>
void fftw_cleanup_threads();

template<>
inline void fftw_cleanup_threads<float>()
{
   ::fftwf_cleanup_threads();
}

template<>
inline void fftw_cleanup_threads<double>()
{
   ::fftw_cleanup_threads();
}

template<>
inline void fftw_cleanup_threads<long double>()
{
   ::fftwl_cleanup_threads();
}

#ifdef HASQUAD
template<>
inline void fftw_cleanup_threads<__float128>()
{
   ::fftwq_cleanup_threads();
}
#endif

///@} fftw_template_plans


/** \defgroup fftw_template_exec Execution 
  * \brief Wrappers for executing with fftw plans.
  *
  * \ingroup fftw_templates.
  * 
  * @{
  */ 

//*********  Plan Execution *************//

/// Execute the given plan on the given arrays
/**
  * \param plan the pre-planned plan
  * \param in the input data array
  * \param out the output data array
  * 
  * \tparam inputDataT the data type of the input array
  * \tparam outputDataT the data type of the output array
  */
template<typename inputDataT, typename outputDataT>
void fftw_execute_dft( typename fftwTypeSpec<inputDataT, outputDataT>::planT plan, 
                       inputDataT * in, 
                       outputDataT * out);

template<>
inline void fftw_execute_dft<complexFT,complexFT>( fftwTypeSpec<complexFT, complexFT>::planT plan, 
                                                   complexFT * in, 
                                                   complexFT * out )
{
   ::fftwf_execute_dft( plan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));
}


template<>
inline void fftw_execute_dft<complexDT,complexDT>( fftwTypeSpec<complexDT, complexDT>::planT plan, 
                                                   complexDT * in, 
                                                   complexDT * out )
{
   ::fftw_execute_dft( plan, reinterpret_cast<fftw_complex*>(in), reinterpret_cast<fftw_complex*>(out));
}


//****** Plan Destruction *********//

/// Destroy the given plan
/**
  * \param plan is the plan to destroy
  *
  * \tparam realT is the real floating point type of the plan
  */ 
template<typename realT>
void fftw_destroy_plan( typename fftwPlanSpec<realT>::planT plan );

template<>
inline void fftw_destroy_plan<float>( fftwPlanSpec<float>::planT plan )
{
   ::fftwf_destroy_plan(plan);
}

template<>
inline void fftw_destroy_plan<double>( fftwPlanSpec<double>::planT plan )
{
   ::fftw_destroy_plan(plan);
}

template<>
inline void fftw_destroy_plan<long double>( fftwPlanSpec<long double>::planT plan )
{
   ::fftwl_destroy_plan(plan);
}

#ifdef HASQUAD
template<>
inline void fftw_destroy_plan<__float128>( fftwPlanSpec<__float128>::planT plan )
{
   ::fftwq_destroy_plan(plan);
}

#endif

///@} fftw_template_exec

}//namespace mx

#endif // fftwTemplates_hpp


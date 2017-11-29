/** \file gaussian.hpp
 * \author Jared R. Males
 * \brief Declarations for utilities related to the Gaussian function.
 * \ingroup gen_math_gaussians
 *
 */

#ifndef gaussian_hpp
#define gaussian_hpp

#include <cmath>


namespace mx
{
namespace math 
{
namespace func 
{
   
///Constant to convert between the Gaussian width parameter and FWHM
/** Used for
  * 
  * \f$ FWHM = 2\sqrt{2\log2}\sigma \f$
  *
  * This was calculated in long double precision.
  * 
  * \ingroup gen_math_gaussians
  */ 
#define twosqrt2log2 2.354820045030949382091484
   
///Convert from FWHM to the Gaussian width parameter
/** Performs the conversion:
  * 
  * \f$ \sigma = 2\sqrt{2\log2}FWHM \f$
  *
  * \ingroup gen_math_gaussians
  */ 
#define fwhm2sigma(fw)   ( (fw) / twosqrt2log2)

///Convert from Gaussian width parameter to FWHM
/** Performs the conversion:
  * 
  * \f$ FWHM = 2\sqrt{2\log2}\sigma \f$
  *
  * \ingroup gen_math_gaussians
  */ 
#define sigma2fwhm(sig)  ( (sig) * twosqrt2log2)
   
///Find value at position (x) of the 1D arbitrarily-centered symmetric Gaussian
/**
  * Computes:
  * \f$ G(x) = G_0 + A\exp[-(0.5/\sigma^2)((x-x_0)^2)]\f$
  * 
  * 
  * \returns the value of the 1D arbitrarily-centered symmetric Gaussian at (x)
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
realT gaussian( const realT & x, ///< [in] is the x-position at which to evaluate the Gaussian
                 const realT G0, ///< [in] is the constant to add to the Gaussian
                 const realT A, ///< [in] is the scaling factor (peak = A)
                 const realT &x0, ///< [in] is the x-coordinate of the center
                 const realT & sigma ///< [in] is the width of the Gaussian.
               )
{ 
   return G0 + A * exp( -( static_cast<realT>(0.5)/(sigma*sigma) * ( ( (x-x0)*(x-x0))) ));
}

///Find value at position (x,y) of the 2D arbitrarily-centered symmetric Gaussian
/**
  * Computes:
  * \f[ 
  * G(x,y) = G_0 + A\exp[-(0.5/\sigma^2)((x-x_0)^2+(y-y_0)^2)]
  * \f]
  * 
  * \returns the value of the 2D arbitrarily-centered symmetric Gaussian at (x,y)
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
realT gaussian2D( const realT  x, ///< [in] the x-position at which to evaluate the Gaussian
                   const realT y, ///< [in] the y-positoin at which to evaluate the Gaussian
                   const realT G0,  ///< [in] the constant to add to the Gaussian
                   const realT A,   ///< [in] the scaling factor (peak height is  A-G0)
                   const realT x0,  ///< [in] the x-coordinate of the center
                   const realT y0,  ///< [in] the y-coordinate of the center
                   const realT sigma  ///< [in] the width of the Gaussian.
                 )
{ 
   return G0 + A*std::exp( -( 0.5/(sigma*sigma) * ( ( (x-x0)*(x-x0)) + ((y-y0)*(y-y0))) ));
}

///Fill in an array with the 2D arbitrarily-centered symmetric Gaussian
/**
  * At each pixel (x,y) of the array this computes:
  * 
  * \f$ (x,y) = G_0 + A\exp[-(0.5/\sigma^2)((x-x_0)^2+(y-y_0)^2)] \f$
  * 
  * \tparam realT is the type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
void gaussian2D( realT * arr, ///< [out] is the allocated array to fill in
                 size_t nx, ///< [in] is the size of the x dimension of the array
                 size_t ny, ///< [in] is the size of the y dimension of the array
                 const realT & G0, ///< [in] is the constant to add to the Gaussian
                 const realT & A, ///< [in] is the scaling factor (peak = A)
                 const realT & x0, ///< [in] is the x-coordinate of the center
                 const realT & y0, ///< [in] is the y-coordinate of the center
                 const realT & sigma  ///< [in] is the third rotation and scaling factor
               )
{
   size_t idx;
   
   for(size_t i=0;i<nx; ++i)
   {
      for(size_t j=0;j<ny; ++j)
      {
         idx = i + j*nx;
         
         arr[idx] = gaussian2D((realT) i, (realT) j,G0,A,x0,y0, sigma);
      }
   }
}

///Find value at position (x,y) of the 2D general elliptical Gaussian
/**
  * Computes:
  * 
  * \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * where, for a counter-clockwise rotation \f$ \theta \f$, we have
  * 
  * \f$ a = \frac{\cos^2 \theta}{\sigma_x^2} + \frac{\sin^2\theta}{\sigma_y^2}\f$
  * 
  * \f$ b = \frac{\sin 2\theta}{2} \left(\frac{1}{\sigma_x^2} - \frac{1}{\sigma_y^2} \right)\f$
  * 
  * \f$ c = \frac{\sin^2 \theta}{\sigma_x^2} + \frac{\cos^2\theta}{\sigma_y^2}\f$
  * 
  * In this version the parameters are specified directly as (a,b,c), in particular avoiding the trig function calls.
  * This should be much more efficient, and so this version should be used inside fitting routines, etc.  However
  * note that the matrix {a b}{b c} must be <a href="http://mathworld.wolfram.com/PositiveDefiniteMatrix.html">positive-definite</a> 
  * otherwise infinities can result from the argument of the exponent being positive.
  * 
  * The functions \ref gaussian2D_gen2rot and \ref gaussian2D_rot2gen provide conversions from
  * (a,b,c) to  (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) and back.  The function \ref gaussian2D_ang is a
  * wrapper for this function, which instead accepts (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) as inputs.
  * 
  * \param x is the x-position at which to evaluate the Gaussian
  * \param y is the y-positoin at which to evaluate the Gaussian
  * \param G0 is the constant to add to the Gaussian
  * \param A is the scaling factor (peak = A)
  * \param x0 is the x-coordinate of the center
  * \param y0 is the y-coordinate of the center
  * \param a is the first rotation and scaling factor
  * \param b is the second rotation and scaling factor
  * \param c is the third rotation and scaling factor
  * 
  * \returns the value of the 2D elliptical Gaussian at (x,y)
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
realT gaussian2D( const realT & x, 
                   const realT & y, 
                   const realT & G0,
                   const realT & A,
                   const realT & x0, 
                   const realT & y0, 
                   const realT & a,
                   const realT & b,
                   const realT & c )
{ 
   realT dx = x-x0;
   realT dy = y-y0;
   
   realT ans;
   ans = G0 + A*std::exp( -0.5* ( a*dx*dx + 2.*b*dx*dy + c*dy*dy));
     
   return ans;
}

///Convert from (a,b,c) to (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) for the elliptical Gaussian. 
/** The general 2D elliptical Gaussian
  *
  *  \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * can be expressed in terms of widths and a rotation angle using: 
  * 
  * \f$ a = \frac{\cos^2 \theta}{\sigma_x^2} + \frac{\sin^2\theta}{\sigma_y^2}\f$
  * 
  * \f$ b = \frac{\sin 2\theta}{2} \left(\frac{1}{\sigma_x^2} - \frac{1}{\sigma_y^2} \right)\f$
  * 
  * \f$ c = \frac{\sin^2 \theta}{\sigma_x^2} + \frac{\cos^2\theta}{\sigma_y^2}\f$
  * 
  * where \f$ \theta \f$ specifies a counter-clockwise rotation.
  * 
  * This function calculates (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) from inputs (a,b,c). It adopts
  * the convention that the long axis of the ellipse is \f$\sigma_x\f$, and \f$\theta\f$ is chosen 
  * appropriately.  Note that \f$-\frac{\pi}{2} < \theta < \frac{\pi}{2}\f$.
  * 
  * \param sigma_x [output]
  * \param sigma_y [output]
  * \param theta [output]
  * \param a [input]
  * \param b [input]
  * \param c [input]
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */
template<typename realT>
void gaussian2D_gen2rot( realT & sigma_x, 
                         realT & sigma_y, 
                         realT & theta,
                         const realT & a,
                         const realT & b,
                         const realT & c )
{
   realT x1, x2, s, s2, theta0, theta1;
   
   
 
   realT arg = a*a - 2*a*c + 4*b*b + c*c;
   if(arg < 0)
   {
      x2 = 0.5*(a+c);
      x1 = x2;
   }
   else
   {
      //There are two roots, one is 1/sigma_x^2 and one is 1/sigma_y^2
      x2 = 0.5*(a + c) + 0.5*sqrt(arg);
      x1 = 0.5*(a + c) - 0.5*sqrt(arg);
   }
   
   //Our convention is that the larger width is sigma_x
   sigma_x = sqrt(1./x1);
   sigma_y = sqrt(1./x2);
   
   //----------------------------------------------------------------------------//
   // Choosing theta:
   // There is an ambiguity in theta and which direction (x,y) is the long axis
   // Here we choose theta so that it specifies the long direction, sigma_x
   //----------------------------------------------------------------------------//

   //s is  (sin(theta))^2
   //s2 = (a-x2)*(a-x2)/(b*b + (a-x2)*(a-x2));
   s = (a-x1)*(a-x1)/(b*b + (a-x1)*(a-x1));
   
   //First check if x1-x2 will be close to zero
   
   if(fabs(x1-x2) < 1e-12)
   {
      theta = 0;
      return;
   }
   
   theta0 = 0.5*asin( 2*b/(x1-x2));   //This always gives the correct sign
   theta1 = asin(sqrt(s)); //This always gives the correct magnitude, and seems to be more accurate
      
   //Compare signs.  If they match, then we use theta1
   if( std::signbit(theta0) == std::signbit(theta1))
   {
      theta = theta1;
   }
   else
   {
      //Otherwise, we switch quadrants
      theta = asin(-sqrt(s));      
   }
         
}
   
///Convert from (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) to (a,b,c) for the elliptical Gaussian.
/** The general 2D elliptical Gaussian
  *
  *  \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * can be expressed in terms of widths and a rotation angle using: 
  * 
  * \f$ a = \frac{\cos^2 \theta}{\sigma_x^2} + \frac{\sin^2\theta}{\sigma_y^2}\f$
  * 
  * \f$ b = \frac{\sin 2\theta}{2} \left(\frac{1}{\sigma_x^2} - \frac{1}{\sigma_y^2} \right)\f$
  * 
  * \f$ c = \frac{\sin^2 \theta}{\sigma_x^2} + \frac{\cos^2\theta}{\sigma_y^2}\f$
  * 
  * where \f$ \theta \f$ specifies a counter-clockwise rotation.
  * 
  * This function calculates (a,b,c) from inputs (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$).
  * 
  * \param a [output]
  * \param b [output]
  * \param c [output]
  * \param sigma_x [input]
  * \param sigma_y [input]
  * \param theta [input]
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */
template<typename realT>
void gaussian2D_rot2gen( realT & a,
                         realT & b,
                         realT & c,
                         const realT & sigma_x, 
                         const realT & sigma_y, 
                         const realT & theta )
{
   realT sn,cs, sx2, sy2;
   
   sn = sin(theta);
   cs = cos(theta);
   sx2 = sigma_x * sigma_x;
   sy2 = sigma_y * sigma_y;
   
   a = cs*cs/sx2 + sn*sn/sy2;
   b = sn*cs*(1./sx2-1./sy2);
   c = sn*sn/sx2 + cs*cs/sy2;
}

///Find value at position (x,y) of the 2D rotated elliptical Gaussian
/**
  * Computes:
  * \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * where, for a counter-clockwise rotation \f$ \theta \f$, we have
  * 
  * \f$ a = \frac{\cos^2 \theta}{\sigma_x^2} + \frac{\sin^2\theta}{\sigma_y^2}\f$
  * 
  * \f$ b = \frac{\sin 2\theta}{2} \left(\frac{1}{\sigma_x^2} - \frac{1}{\sigma_y^2} \right)\f$
  * 
  * \f$ c = \frac{\sin^2 \theta}{\sigma_x^2} + \frac{\cos^2\theta}{\sigma_y^2}\f$
  * 
  * This is a convenience wrapper for the general elliptical Gaussian function \ref Gaussian2D, where here
  * (a,b,c) are first calculated from (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$).  This will in general be
  * slower due to the trig function calls, so the (a,b,c) version should be used most of the time.
  *
  * The functions \ref gaussian2D_gen2rot and \ref gaussian2D_rot2gen provide conversions from
  * (a,b,c) to  (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) and back.  The function \ref gaussian2D_rot is a
  * wrapper for this function, which instead accepts (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) as inputs.
  * 
  * \param x is the x-position at which to evaluate the Gaussian
  * \param y is the y-positoin at which to evaluate the Gaussian
  * \param G0 is the constant to add to the Gaussian
  * \param A is the scaling factor (peak = A)
  * \param x0 is the x-coordinate of the center
  * \param y0 is the y-coordinate of the center
  * \param sigma_x is the width in the rotated x direction
  * \param sigma_y is the width in the rotated y direction
  * \param theta is the counter-clockwise rotation angle
  * 
  * \returns the value of the 2D elliptical Gaussian at (x,y)
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
realT gaussian2D_ang( const realT & x, 
                       const realT & y, 
                       const realT & G0,
                       const realT & A,
                       const realT & x0, 
                       const realT & y0, 
                       const realT & sigma_x,
                       const realT & sigma_y,
                       const realT & theta )
{
   realT a, b, c;
   
   gaussian2D_rot2gen(a,b,c, sigma_x, sigma_y, theta);
   
   return gaussian2D(x,y,G0, A,x0,y0, a,b,c);
}

///Fill in an array with the 2D general elliptical Gaussian
/**
  * At each pixel (x,y) of the array this computes:
  * 
  * \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * where, for a counter-clockwise rotation \f$ \theta \f$, we have
  * 
  * \f$ a = \frac{\cos^2 \theta}{\sigma_x^2} + \frac{\sin^2\theta}{\sigma_y^2}\f$
  * 
  * \f$ b = \frac{\sin 2\theta}{2} \left(\frac{1}{\sigma_x^2} - \frac{1}{\sigma_y^2} \right)\f$
  * 
  * \f$ c = \frac{\sin^2 \theta}{\sigma_x^2} + \frac{\cos^2\theta}{\sigma_y^2}\f$
  * 
  * In this version the parameters are specified directly as (a,b,c), in particular avoiding the trig function calls.
  * This should be much more efficient, and so this version should be used inside fitting routines, etc.
  * 
  * The functions \ref gaussian2D_gen2rot and \ref gaussian2D_rot2gen provide conversions from
  * (a,b,c) to  (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) and back.  The function \ref gaussian2D_ang is a
  * wrapper for this function, which instead accepts (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) as inputs.
  * 
  * \tparam realT is the type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
void gaussian2D( realT * arr,      ///< [out] is the allocated array to fill in
                 size_t nx,        ///< [in] is the size of the x dimension of the array
                 size_t ny,        ///< [in] is the size of the y dimension of the array
                 const realT & G0, ///< [in] is the constant to add to the Gaussian
                 const realT & A,  ///< [in] is the scaling factor (peak = A)
                 const realT & x0, ///< [in] is the x-coordinate (in pixels) of the center
                 const realT & y0, ///< [in] is the y-coordinate (in pixels) of the center
                 const realT & a,  ///< [in] is the first rotation and scaling factor
                 const realT & b,  ///< [in] is the second rotation and scaling factor
                 const realT & c   ///< [in] is the third rotation and scaling factor
               )
{
   size_t idx;
   
   for(size_t i=0;i<nx; ++i)
   {
      for(size_t j=0;j<ny; ++j)
      {
         idx = i + j*nx;
         
         arr[idx] = gaussian2D((realT) i, (realT) j,G0,A, x0, y0, a, b, c);
      }
   }
}


///Fill in an array with the 2D general elliptical Gaussian
/**
  * At each pixel (x,y) of the array this computes:
  * 
  * \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * where, for a counter-clockwise rotation \f$ \theta \f$, we have
  * 
  * \f$ a = \frac{\cos^2 \theta}{\sigma_x^2} + \frac{\sin^2\theta}{\sigma_y^2}\f$
  * 
  * \f$ b = \frac{\sin 2\theta}{2} \left(\frac{1}{\sigma_x^2} - \frac{1}{\sigma_y^2} \right)\f$
  * 
  * \f$ c = \frac{\sin^2 \theta}{\sigma_x^2} + \frac{\cos^2\theta}{\sigma_y^2}\f$
  * 
  * This is a convenience wrapper for the general elliptical Gaussian function \ref Gaussian2D, where here
  * (a,b,c) are first calculated from (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$).  
  *
  * The functions \ref gaussian2D_gen2rot and \ref gaussian2D_rot2gen provide conversions from
  * (a,b,c) to  (\f$\sigma_x\f$,\f$\sigma_y\f$,\f$\theta\f$) and back.  
  * 
  * \param x is the x-position at which to evaluate the Gaussian
  * \param y is the y-positoin at which to evaluate the Gaussian
  * \param G0 is the constant to add to the Gaussian
  * \param A is the scaling factor (peak = A)
  * \param x0 is the x-coordinate of the center
  * \param y0 is the y-coordinate of the center
  * \param sigma_x is the width in the rotated x direction
  * \param sigma_y is the width in the rotated y direction
  * \param theta is the counter-clockwise rotation angle
  * 
  * \returns the value of the 2D elliptical Gaussian at (x,y)
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
void gaussian2D_ang( realT * arr,
                 size_t nx,
                 size_t ny,
                 const realT & G0,
                 const realT & A,
                 const realT & x0, 
                 const realT & y0, 
                 const realT & sigma_x,
                 const realT & sigma_y,
                 const realT & theta )
{
   realT a, b, c;
   size_t idx;
   
   //Get the (a,b,c) parameters so the trig only happens once
   gaussian2D_rot2gen(a,b,c,sigma_x, sigma_y, theta);
   
   for(size_t i=0;i<nx; ++i)
   {
      for(size_t j=0;j<ny; ++j)
      {
         idx = i + j*nx;
         
         arr[idx] = gaussian2D((realT) i, (realT) j,G0,A, x0, y0, a, b, c);
      }
   }
}


///Calculate the Jacobian at position (x,y) for the 2D general elliptical Gaussian
/**
  * Given:
  * 
  * \f$ G(x,y) = G_0 + A\exp [-0.5( a(x-x_0)^2 + b(x-x_0)(y-y_0) + c(y-y_0)^2 ]\f$
  * 
  * we calculate
  * 
  * \f$ \frac{\partial G}{\partial G_0} =  1 \f$
  * 
  * \f$ \frac{\partial G}{\partial A} = (G - G_0)/A \f$
  * 
  * \f$ \frac{\partial G}{\partial x_0} = 0.5(G-G_0)( 2a(x-x_0) + b(y-y_0))  \f$
  *
  * \f$ \frac{\partial G}{\partial y_0} = 0.5(G-G_0) ( b(x-x_0) + 2c(y-y_0)) \f$
  *
  * \f$ \frac{\partial G}{\partial a} = -0.5(G-G_0)  ( (x-x_0)^2 ) \f$
  *
  * \f$ \frac{\partial G}{\partial b} = -0.5(G-G_0) ( (x-x_0)(y-y_0) ) \f$
  *
  * \f$ \frac{\partial G}{\partial c} = -0.5(G-G_0) ( (y-y_0)^2 )\f$ 
  * 
  * \param j is a 7 element vector which is populated with the derivatives
  * \param x is the x-position at which to evaluate the Gaussian
  * \param y is the y-positoin at which to evaluate the Gaussian
  * \param G0 is the constant to add to the Gaussian
  * \param A is the scaling factor (peak = A)
  * \param x0 is the x-coordinate of the center
  * \param y0 is the y-coordinate of the center
  * \param a is the first rotation and scaling factor
  * \param b is the second rotation and scaling factor
  * \param c is the third rotation and scaling factor
  * 
  * 
  * \tparam realT is type to use for arithmetic
  * 
  * \ingroup gen_math_gaussians
  */ 
template<typename realT>
void gaussian2D_jacobian( realT *j,
                          const realT & x, 
                          const realT & y, 
                          const realT & G0,
                          const realT & A,
                          const realT & x0, 
                          const realT & y0, 
                          const realT & a,
                          const realT & b,
                          const realT & c )
{ 
   realT G_G0 = gaussian2D<realT>(x, y, 0, A, x0, y0, a, b ,c);
   
   j[0] = (realT) 1;
   
   j[1] = (G_G0)/A;
   
   j[2] = 0.5*G_G0 * ( 2*a*(x-x0) + b*(y-y0));
   
   j[3] = 0.5*G_G0 * ( b*(x-x0) + 2*c*(y-y0));
   
   j[4] = -0.5*G_G0 * ( (x-x0)*(x-x0) );
   
   j[5] = -0.5*G_G0 * ( (x-x0)*(y-y0) );
   
   j[6] = -0.5*G_G0 * ( (y-y0)*(y-y0) );
   
}

} //namespace func 
}//namespace math
} //namespace mx

#endif //gaussian_hpp




/** \file pupil.hpp
  * \author Jared R. Males (jaredmales@gmail.com)
  * \brief Utilities for specifying pupils.
  * \ingroup mxAO_files
  * 
  */

#ifndef __pupil_hpp__
#define __pupil_hpp__

#include <mx/fitsFile.hpp>
#include <mx/imagingUtils.hpp>
#include <mx/signalWindows.hpp>

#include "aoPaths.hpp"

namespace mx
{
   
namespace AO
{

///Generate a circular pupil and saves it to disk
template<typename realT>
void circularPupil( const std::string & pupilName,
                    realT pupilDiamPixels,
                    realT pupilDiamMeters,
                    realT centralObs = 0 )
{

   

   /*Create pupil*/
   Eigen::Array<realT, -1, -1> pup;
   
   pup.resize(pupilDiamPixels, pupilDiamPixels);
   
   mx::circularPupil( pup, centralObs);
   
   mx::fitsHeader phead;
   phead.append("SCALE", pupilDiamMeters/pupilDiamPixels, "Scale in m/pix");
   phead.append("PUPILD", pupilDiamMeters, "Physical diameter of pupil image [m]");
   phead.append("CENTOBS", centralObs, "Central obscuration ratio");
   
   mx::fitsFile<realT> ff;
   
   std::string fName = mx::AO::path::pupil::pupilFile(pupilName, true);

   ff.write(fName, pup, phead);
   
}


///Generates a circular apodized pupil and saves it to disk
/** Apodization is with a Tukey window.
  */
template<typename realT>
void circularApodizedPupil( const std::string & pupilName,
                            int pupilDiamPixels,
                            realT pupilDiamMeters,
                            realT tukeyAlpha,
                            realT centralObs = 0,
                            realT overScan = 0)
{

   /*Create pupil*/
   Eigen::Array<realT, -1, -1> pup;
   
   pup.resize(pupilDiamPixels, pupilDiamPixels);
   
   
   realT cen = 0.5*(pupilDiamPixels - 1.0);
   
   if(centralObs == 0)
   {
      tukey2d<realT>(pup.data(), pupilDiamPixels, pupilDiamPixels + overScan, tukeyAlpha, cen,cen);
   }
   else
   {
      tukey2dAnnulus<realT>(pup.data(), pupilDiamPixels, pupilDiamPixels + overScan, centralObs, tukeyAlpha, cen,cen);
   }
   
   
   
   mx::fitsHeader phead;
   phead.append("SCALE", pupilDiamMeters/pupilDiamPixels, "Scale in m/pix");
   phead.append("PUPILD", pupilDiamMeters, "Physical diameter of pupil image [m]");
   phead.append("CENTOBS", centralObs, "Central obscuration ratio");
   phead.append("TUKALPHA", tukeyAlpha, "Tukey window alpha parameter");
   phead.append("OVERSCAN", overScan, "Apodization overscan");
    
   mx::fitsFile<realT> ff;
   
   std::string fName = mx::AO::path::pupil::pupilFile(pupilName, true);

   ff.write(fName, pup, phead);
   
}


} //namespace mx
} //namespace AO

   
#endif //__basis_hpp__
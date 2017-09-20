/** \file astrodyn.hpp
 * \author Jared R. Males
 * \brief Declarations of some astrodynamic utilities
 *
 */

#ifndef __astrodyn_hpp__
#define __astrodyn_hpp__

//#include <sofa.h>


#include <cmath>
//#include "astroconstants.h"
#include "geo.hpp"

//#include "../vmop/MMatrix1"

/** \addtogroup astrodynamics
  * @{ 
  */

  

///Breaks down a decimal day into hours, minutes and decimal point seconds.
/** Assumes 86400 seconds per day, ignoring leap seconds.
 * \param Dy [output] is the resultant integer day
 * \param hr [output] is the resultant integer hr
 * \param min [output] is the resultant integer minute
 * \param sec [output] is the resultand integer second
 * \param day [input] is the decimal day to break down.
 * \retval 0 on success
 * \retval -1 on failure.
 */
int get_hrminsec_fm_day(int &Dy, int &hr, int &min, double &sec, double day);

///Returns Greenwich Mean Sidereal Time for a given UTC time.
/** Uses the SOFA 2006 GMST function, and a (for now) hardcoded value of dUT1
 * \param GMST [output] GMST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param Dy [input] integer day (1-31)
 * \param hr [input] integer hour (0-23)
 * \param min [input] integer minute (0-59)
 * \param sec [input] decimal second (0-59)
 * \retval 0 on success
 */
int get_GMST(double &GMST, int Yr, int Mo, int Dy, int hr, int min, double sec);

///Returns Greenwich Mean Sidereal Time for a given UTC time.
/** Uses the SOFA 2006 GMST function, and a (for now) hardcoded value of dUT1
 * \param GMST [output] GMST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param day [input] decimal day (1-31)
 * \retval 0 on sucess
 */
int get_GMST(double &GMST, int Yr, int Mo, double day);


///Returns Greenwich Apparent Sidereal Time for a given UTC time.
/** Uses the SOFA 2006 GAST function, and a (for now) hardcoded value of dUT1
 * \param GAST [output] GAST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param Dy [input] integer day (1-31)
 * \param hr [input] integer hour (0-23)
 * \param min [input] integer minute (0-59)
 * \param sec [input] decimal second (0-59)
 * \retval 0 on success
 */
int get_GAST(double &GAST, int Yr, int Mo, int Dy, int hr, int min, double sec);

///Returns Greenwich Mean Sidereal Time for a given UTC time.
/** Uses the SOFA 2006 GMST function, and a (for now) hardcoded value of dUT1
 * \param GAST [output] GAST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param day [input] decimal day (1-31)
 * \retval 0 on sucess
 */
int get_GAST(double &GAST, int Yr, int Mo, double day);

///Returns Local Mean Sidereal Time for a given UTC time and longitude.
/** Uses the SOFA 2006 GMST function, and a (for now) hardcoded value of dUT1
 * \param LMST [output] LMST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param Dy [input] integer day (1-31)
 * \param hr [input] integer hour (0-23)
 * \param min [input] integer minute (0-59)
 * \param sec [input] decimal second (0-59)
 * \param lon [input] longitude in degrees, E+, W-
 * \retval 0 on success
 */
int get_LMST(double &LMST, int Yr, int Mo, int Dy, int hr, int min, double sec, double lon);

///Returns Local Mean Sidereal Time for a given UTC time.
/** Uses the SOFA 2006 GMST function, and a (for now) hardcoded value of dUT1
 * \param LMST [output] LMST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param day [input] decimal day (1-31)
 * \retval 0 on sucess
 */
int get_LMST(double &LMST, int Yr, int Mo, double day, double lon);

///Returns Local Apparent Sidereal Time for a given UTC time and longitude.
/** Uses the SOFA 2006 GAST function, and a (for now) hardcoded value of dUT1
 * \param LAST [output] LAST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param Dy [input] integer day (1-31)
 * \param hr [input] integer hour (0-23)
 * \param min [input] integer minute (0-59)
 * \param sec [input] decimal second (0-59)
 * \param lon [input] longitude in degrees, E+, W-
 * \retval 0 on success
 */
int get_LAST(double &LAST, int Yr, int Mo, int Dy, int hr, int min, double sec, double lon);

///Returns Local Apparent Sidereal Time for a given UTC time.
/** Uses the SOFA 2006 GMST function, and a (for now) hardcoded value of dUT1
 * \param LAST [output] LAST in hours
 * \param Yr [input] integer year
 * \param Mo [input] integer month (1-12)
 * \param day [input] decimal day (1-31)
 * \retval 0 on sucess
 */
int get_LAST(double &LAST, int Yr, int Mo, double day, double lon);


///Calculates the Azimuth and Elevation for a given Hour-Angle, Declination, and Latitude.
/** References: J. Meeus "Astronomical Algorithms", 1991; V. Pisacane "Fundamentals of Space Systems", 2nd ed., 2005.
 * \param az is filled in with the azimuth
 * \param el is filled in with the elevation
 * \param ha is the hour angle
 * \param dec is the declination
 * \param lat is the latitude*/
void calc_AZ_EL(double *az, double *el, double ha, double dec, double lat);


void azel_to_hadec(double &ha, double &dec, double az, double el, double lat);

//void azel_to_hadec(mx::Vectord &ha, mx::Vectord &dec, const mx::Vectord &az, const mx::Vectord &el, double lat);

///Calculate the Parallactic angle, with angles in degrees
/** \param lat is the observer latitude
 * \param dec is the object declination
 * \param ha is the hour angle, in degrees, positive to the east
 */
double get_ParAng_deg(double lat, double dec, double ha);

///Calculate the Parallactic angle, with angles in radians
/** \param lat is the observer latitude
 * \param dec is the object declination
 * \param ha is the hour angle, in radians, positive to the east
 */
double get_ParAng_rad(double lat, double dec, double ha);


///Convert from latitude/longitude to Earth-centered inertial x-y-z coordinates
/**
  * \param x [output] x coordinate (meters)
  * \param y [output] y coordinate (meters)
  * \param z [output] z coordinate (meters)
  * \param lat [input] the latitude (radians)
  * \param lon [input] the longitude (radians)
  * \param alt [input] the altitude of RAD_EARTH (meters)
  * \param lst [input] the local sidereal time (radians) 
  */
int latlon_to_ECI(double &x, double &y, double &z, double lat, double lon, double alt, double lst);

///Convert from latitude/longitude to Earth-centered inertial x-y-z coordinates
/** vmop version
  * \param x [output] x coordinates, same length as lst (meters)
  * \param y [output] y coordinates, same length as lst  (meters)
  * \param z [output] z coordinate (meters)
  * \param lat [input] the latitude (radians)
  * \param lon [input] the longitude (radians)
  * \param alt [input] the altitude of RAD_EARTH (meters)
  * \param lst [input] the local sidereal time (radians) 
  */
//int latlon_to_ECI(mx::Vectord &x, mx::Vectord &y, double &z, double lat, double lon, double alt, const mx::Vectord &lst);


// int ECI_to_TCH( mx::Vectord & az,
//                 mx::Vectord & el,
//                 mx::Vectord & r,
//                 const double lat,
//                 const mx::Vectord & lst,
//                 const mx::Vectord & obs_x,
//                 const mx::Vectord & obs_y,
//                 const double obs_z,
//                 const mx::Vectord & tgt_x,
//                 const mx::Vectord & tgt_y,
//                 const mx::Vectord & tgt_z ); 


/// @}

#endif //__astrodyn_hpp__
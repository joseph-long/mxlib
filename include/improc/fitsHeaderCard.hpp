/** \file fitsHeaderCard.hpp
  * \brief Declares and defines a class to work with a FITS header card
  * \ingroup fits_processing_files
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  */

#ifndef fitsHeaderCard_hpp
#define fitsHeaderCard_hpp

#include "../ioutils/stringUtils.hpp"

#include "fitsUtils.hpp"

namespace mx
{
namespace improc
{

/** \ingroup fits_processing
  *@{
  */

/// Simple structure to hold the three components of a FITS header card
/** Is a struct so all members are public for easy access.  Several string-to-number conversions are provided.
  */
struct fitsHeaderCard
{
   ///The keyword
   std::string keyword;

   ///The value
   std::string value;

   ///The comment
   std::string comment;

   ///The FITS type
   int type;

   /// \name Constructors
   /**
     */
   //@{

   ///Basic c'tor
   fitsHeaderCard()
   {
      type = getFitsType<fitsUnknownType>();
   }

   ///Construct from the three components, when value's type is unknown
   /**
     */
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   const void * v,        ///< [in] the value, must be castable to char*
                   const std::string & c  ///< [in] the comment
                 );

   ///Construct from the three components, when value's type is unknown
   /**
     */
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   void * v,              ///< [in] the value, must be castable to char*
                   const std::string & c  ///< [in] the comment
                 );

   ///Construct from the three components, when it's really a comment card
   /** This overload is provided to facilitate handling of comments when re-writing the file.
     *
     */
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   fitsCommentType v,     ///< [in] an object of type fitsCommentType
                   const std::string & c  ///< [in] the comment
                 );

   ///Construct from the three components, when it's really a history card
   /** This overload is provided to facilitate handling of history when re-writing the file.
     *
     */
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   fitsHistoryType v,     ///< [in] an object of type fitsHistoryType
                   const std::string & c  ///< [in] the comment
                 );

   ///Construct from the three components for a known type.
   /**
     */
   template<typename typeT>
   fitsHeaderCard( const std::string & k, ///< [in] they keyword
                   const typeT v,         ///< [in] the value
                   const std::string & c  ///< [in] the commend
                 );

   ///Construct from just two components, when value's type is unknown
   /**
     */
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   void* v                ///< [in] the value, must be cast-able to char*
                 );

   ///Construct from just two components, when value's type is unknown
   /**
     */
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   const void* v          ///< [in] the value, must be cast-able to char*
                 );

   ///Construct from just two components when value's type is known.
   /**
     */
   template<typename typeT>
   fitsHeaderCard( const std::string & k, ///< [in] the keyword
                   const typeT v          ///< [in] the value
                 );

   ///Construct from just keyword, when value's type is unknown
   /**
     */
   explicit fitsHeaderCard(const std::string & k  /**< [in] the keyword*/);


   //@}

   ///\name Setting Components
   /**
     */
   //@{

   ///Set the value string, applying the appropriate conversion and setting the FITS type.
   /**
     * \param v the value to convert to string and set
     *
     * \tparam typeT is the type to convert from
     */
   template<typename typeT>
   void setValue(const typeT v);

   //@}

   ///\name Conversions
   /**
     */
   //@{

   ///Get value converted to the specified type
   /** Uses \ref convertFromString.  typeT can be any <a href=\isfundamental>fundamental type</a>,
     * or any type which can be typecast from std::string.  This is specialized for typeT=std::string so
     * that leading and trailing apostrophes are removed.
     */
   template<typename typeT>
   typeT Value() const;

   ///Convert value to an string, stripping apostrophes if necessary.
   /** \returns the result of converting the value string to a string
    */
   std::string String();

   ///Convert value to an integer
   /** \returns the result of converting the value string to an integer
    */
   int Int();

   ///Convert value to an unsigned integer
   /** \returns the result of converting the value string to an unsigned integer
    */
   unsigned int unsignedInt();

   ///Convert value to a long integer
   /** \returns the result of converting the value string to a long integer
    */
   long Long();

   ///Convert value to an unsigned long integer
   /** \returns the result of converting the value string to an unsigned long integer
    */
   unsigned long unsignedLong();

   ///Convert value to a long long integer
   /** \returns the result of converting the value string to a long long integer
    */
   long long longLong();

   ///Convert value to an unsigned long long integer
   /** \returns the result of converting the value string to anunsigned long long  integer
    */
   unsigned long long unsignedLongLong();

   ///Convert value to a float
   /** \returns the result of converting the value string to a float
    */
   float Float();

   ///Convert value to a double
   /** \returns the result of converting the value string to a double
    */
   double Double();

   ///Convert value to a long double
   /** \returns the result of converting the value string to a long double
    */
   long double longDouble();

   //@}




   ///\name Output
   /**
     */
   //@{

   /// Writes this card to a FITS file, using \ref mx::improc::fits_write_key<typename typeT>(fitsfile * fptr, char * keyword, void * value, char * comment).
   /**
     */
   int write(fitsfile * fptr);

   //@}

}; //fitsHeaderCard

///@}



inline
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                void * v,
                                const std::string & c
                              )
{
   keyword = k;
   value = (char *) v;
   type = getFitsType<fitsUnknownType>();
   comment = c;
}

inline
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                const void * v,
                                const std::string & c
                              )
{
   keyword = k;
   value = (char *) v;
   type = getFitsType<fitsUnknownType>();
   comment = c;
}

inline
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                fitsCommentType v,
                                const std::string & c
                              )
{
   keyword = k;
   value = "";
   type = getFitsType<fitsCommentType>();
   comment = c;
}

inline
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                fitsHistoryType v,
                                const std::string & c)
{
   keyword = k;
   value = "";
   type = getFitsType<fitsHistoryType>();
   comment = c;
}

template<typename typeT>
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                const typeT v,
                                const std::string & c
                              )
{
   keyword = k;
   value = ioutils::convertToString<typeT>(v);
   type = getFitsType<typeT>();
   comment = c;
}


inline
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                void * v
                              )
{
   keyword = k;
   value = (char *) v;
   type = getFitsType<fitsUnknownType>();
}


inline
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                const void * v
                              )
{
   keyword = k;
   value = (char *) v;
   type = getFitsType<fitsUnknownType>();
}

inline
fitsHeaderCard::fitsHeaderCard( const std::string & k)
{
   keyword = k;
   value = "";
   type = getFitsType<fitsUnknownType>();
}

template<typename typeT>
fitsHeaderCard::fitsHeaderCard( const std::string & k,
                                const typeT v
                              )
{
   keyword = k;
   value = ioutils::convertToString<typeT>(v);
   type = getFitsType<typeT>();
}

template<typename typeT>
void fitsHeaderCard::setValue(const typeT v)
{
   value = ioutils::convertToString<typeT>(v);
   type = getFitsType<typeT>();
}

template<typename typeT>
typeT fitsHeaderCard::Value() const
{
   return ioutils::convertFromString<typeT>(value);
}

//Specialization for strings, removing the apostrophe.
template<>
std::string fitsHeaderCard::Value<std::string>() const
{
   std::string s = value;
   fitsStripApost(s);
   return s;
}

inline
std::string fitsHeaderCard::String()
{
   std::string s = value;
   fitsStripApost(s);
   return s;
}


inline
int fitsHeaderCard::Int()
{
   return ioutils::convertFromString<int>(value);
}

inline
unsigned int fitsHeaderCard::unsignedInt()
{
   return ioutils::convertFromString<unsigned int>(value);
}

inline
long fitsHeaderCard::Long()
{
   return ioutils::convertFromString<long>(value);
}

inline unsigned long fitsHeaderCard::unsignedLong()
{
   return ioutils::convertFromString<unsigned long>(value);
}

inline long long fitsHeaderCard::longLong()
{
   return ioutils::convertFromString<long long>(value);
}

inline unsigned long long fitsHeaderCard::unsignedLongLong()
{
   return ioutils::convertFromString<unsigned long long>(value);
}

inline float fitsHeaderCard::Float()
{
   return ioutils::convertFromString<float>(value);
}

inline double fitsHeaderCard::Double()
{
   return ioutils::convertFromString<double>(value);
}

inline long double fitsHeaderCard::longDouble()
{
   return ioutils::convertFromString<long double>(value);
}


inline int fitsHeaderCard::write(fitsfile *fptr)
{
//   int fstatus;
   switch(type)
   {
      case TSTRING:
      {
         return fits_write_key<char *>(fptr, (char *) keyword.c_str(), (void *)value.c_str(), (char *) comment.c_str());
      }
      case TBYTE:
      {
         unsigned char b = Int();
         return fits_write_key<unsigned char>(fptr, (char *) keyword.c_str(), &b, (char *) comment.c_str());
      }
      case TSBYTE:
      {
         signed char sc = Int();
         return fits_write_key<signed char>(fptr, (char *) keyword.c_str(), &sc, (char *) comment.c_str());
      }
      case TSHORT:
      {
         short s = Int();
         return fits_write_key<short>(fptr, (char *) keyword.c_str(), &s, (char *) comment.c_str());
      }
      case TUSHORT:
      {
         unsigned short us = Int();
         return fits_write_key<unsigned short>(fptr, (char *) keyword.c_str(), &us, (char *) comment.c_str());
      }
      case TINT:
      {
         int i = Int();
         return fits_write_key<int>(fptr, (char *) keyword.c_str(), &i, (char *) comment.c_str());
      }
      case TLONG:
      {
         long l = Long();
         return fits_write_key<long>(fptr, (char *) keyword.c_str(), &l, (char *) comment.c_str());
      }
      case TLONGLONG:
      {
         long long ll = longLong();
         return fits_write_key<long long>(fptr, (char *) keyword.c_str(), &ll, (char *) comment.c_str());
      }
      case TULONG:
      {
         unsigned long ul = unsignedLong();
         return fits_write_key<unsigned long>(fptr, (char *) keyword.c_str(), &ul, (char *) comment.c_str());
      }
      case TFLOAT:
      {
         float f = Float();
         return fits_write_key<float>(fptr, (char *) keyword.c_str(), &f, (char *) comment.c_str());
      }
      case TDOUBLE:
      {
         double d = Double();
         return fits_write_key<double>(fptr, (char *) keyword.c_str(), &d, (char *) comment.c_str());
      }
      case fitsTCOMMENT:
      {
         return fits_write_comment(fptr, (char *) comment.c_str());
      }
      case fitsTHISTORY:
      {
         return fits_write_history(fptr, (char *) comment.c_str());
      }
      default:
      {
         return fits_write_key<fitsUnknownType>(fptr, (char *) keyword.c_str(), (void *)value.c_str(), (char *) comment.c_str());
      }
   }
}

} //namespace improc
} //namespace mx



#endif //fitsHeaderCard_hpp

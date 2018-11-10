/** \file KLIPreduction.hpp
  * \author Jared R. Males
  * \brief Declarations and definitions for an implementation of the Karhunen-Loeve Image Processing (KLIP) algorithm. 
  * \ingroup hc_imaging_files
  * \ingroup image_processing_files
  *
  */



#ifndef __KLIPreduction_hpp__
#define __KLIPreduction_hpp__


#include <vector>
#include <map>

#include <omp.h>

#include "../ompLoopWatcher.hpp"
#include "../math/geo.hpp"
#include "../math/eigenLapack.hpp"

#include "ADIobservation.hpp"

namespace mx
{
namespace improc
{
   
//double t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, tf;
//double dcut, dscv, dklims, dgemm, dsyevr, dcfs, drot, dcombo, dread;

   



namespace HCI 
{
   ///Image exclusion methods
   /** \ingroup hc_imaging_enums
     */
   enum excludeMethods{ excludeNone, ///< Exclude no images
                        excludePixel, ///< Exclude by pixels of rotation
                        excludeAngle, ///< Exclude by angle of roration
                        excludeImno ///< Exclude by number of images
                      };   
}

/// An implementation of the Karhunen-Loeve Image Processing (KLIP) algorithm.
/** KLIP\cite soummer_2012 is a principle components analysis (PCA) based technique for PSF estimation.
  * 
  * 
  * \tparam _realT  is the floating point type in which to do calculations
  * \tparam _derotFunctObj the ADIobservation derotator class.
  * \tparam _evCalcT the real type in which to do eigen-decomposition.  Should generally be double for stable results.  
  * \ingroup hc_imaging 
  */ 
template<typename _realT, class _derotFunctObj, typename _evCalcT = double>
struct KLIPreduction : public ADIobservation<_realT, _derotFunctObj>
{
   typedef _realT realT;
   
   typedef Array<realT, Eigen::Dynamic, Eigen::Dynamic> eigenImageT;
   
   typedef _evCalcT evCalcT;
   
   int padSize;
   
   /// Specifies the number of modes to include in the PSF.
   /** The output image is a cube with a plane for each entry in Nmodes.
     * Only the number of eigenvalues required for the maximum value of Nmodes
     * are calculated, so this can have an important impact on speed.
     * 
     * Can be initialized as:
     * \code 
     * red.Nmodes={5,10,15,20};
     * \endcode
     * 
     */ 
   std::vector<int> Nmodes;
   
   int maxNmodes;
   
   /// Specify the minimum pixel difference at the inner edge of the search region 
   realT mindpx;
   
   
   /// Controls how reference images are excluded, if at all, from the covariance matrix for each target image.
   /** Can have the following values:
     *  - <b>HCI::excludeNone</b> = no exclusion, all images included
     *  - <b>HCI::excludePixel</b> = exclude based on pixels of rotation at the inner edge of the region
     *  - <b>HCI::excludeAngle</b> = exclude based on degrees of rotation at the inner edge of the region
     *  - <b>HCI::excludeImno</b> = exclude based on number of images
     */
   int excludeMethod;
   
   ///Number of reference images to include in the covariance matrix
   /** If > 0, then at most this many images, determined by highest cross-correlation, are included.
     * This is determined after rotational/image-number exclusion. 
     * If == 0, then all reference images are included. 
     */
   int includeRefNum; 
   
   
   
   KLIPreduction()
   {
      initialize();
   }
   
   KLIPreduction( const std::string & dir, 
                  const std::string & prefix, 
                  const std::string & ext = ".fits") : ADIobservation<_realT, _derotFunctObj>(dir, prefix, ext)
   {
      initialize();
   }
   
   explicit KLIPreduction( const std::string & fileListFile ) : ADIobservation<_realT, _derotFunctObj>(fileListFile)
   {
      initialize();
   }
   
   virtual ~KLIPreduction() {}
   
   void initialize()
   {
      mindpx = 0;
      excludeMethod = HCI::excludeNone;
      includeRefNum = 0;
      padSize = 4;
      
      t_worker_begin =0;
      t_worker_end = 0;
      
      t_eigenv = 0;
      
      t_klim = 0;
      
      t_psf = 0;
      
   }
   
   void meanSubtract(eigenCube<realT> & ims, std::vector<realT> & sds);
   void medianSubtract(eigenCube<realT> & ims, std::vector<realT> & sds);
   void getStdDevs(std::vector<realT> sd, eigenCube<realT> & ims);
   
   /// Run KLIP in a set of geometric search regions.
   /** The arguments are 4 vectors, where each entry defines one component of the  search region.
     *
     * \returns 0 on success
     * \returns -1 on error
     * 
     */
   int regions( std::vector<realT> minr, ///< [in]
                std::vector<realT> maxr, ///< [in]
                std::vector<realT> minq, ///< [in]
                std::vector<realT> maxq  ///< [in]
              );
   
   /// Run KLIP in a geometric search region.
   /** \overload
     *
     * \returns 0 on success
     * \returns -1 on error
     * 
     */
   int regions( realT minr, ///< [in]
                realT maxr, ///< [in]
                realT minq, ///< [in]
                realT maxq  ///< [in]
              )
   {
      std::vector<realT> vminr(1, minr);
      std::vector<realT> vmaxr(1, maxr);
      std::vector<realT> vminq(1, minq);
      std::vector<realT> vmaxq(1, maxq);
      
      return regions(vminr, vmaxr, vminq, vmaxq);
   }
   
   void worker(eigenCube<realT> & rims, std::vector<size_t> & idx, realT dang);
   //void worker(eigenCube<realT> rims, vector<size_t> idx, realT dang);
   
   ///Calculate the KL images for a given covariance matrix
/*   template<typename eigenT, typename eigenT1>
   void calcKLIms( eigenT & klims, 
                   eigenT & cv, 
                   const eigenT1 & Rims, 
                   int n_modes = 0,
                   math::syevrMem<evCalcT> * mem = 0);   
*/

   /*template<typename eigenT, typename eigenTv>
   void collapseCovar( eigenT & cutCV, 
                       const eigenT & CV,
                       const std::vector<realT> & sds,
                       eigenT & rimsCut,
                       const eigenTv & rims,
                       int imno,
                       double dang );
     */                 
   
   double t_worker_begin;
   double t_worker_end;
   
   double t_eigenv;
   double t_klim;
   double t_psf;
   
   void dump_times()
   {
      printf("KLIP reduction times: \n");
      printf("  Total time: %f sec\n", this->t_end-this->t_begin);
      printf("    Loading: %f sec\n", this->t_load_end-this->t_load_begin);
      printf("    Fake Injection: %f sec\n", this->t_fake_end-this->t_fake_begin);
      printf("    Coadding: %f sec\n", this->t_coadd_end-this->t_coadd_begin);
      printf("    Preprocessing: %f sec\n", this->t_preproc_end - this->t_preproc_begin);
      printf("      Az USM: %f sec\n", this->t_azusm_end - this->t_azusm_begin);
      printf("      Gauss USM: %f sec\n", this->t_gaussusm_end - this->t_gaussusm_begin);
      printf("    KLIP algorithm: %f sec\n", this->t_worker_end - this->t_worker_begin);
      printf("      EigenDecomposition %f sec\n", this->t_eigenv);
      printf("      KL image calc %f sec\n", this->t_klim);
      printf("      PSF calc/sub %f sec \n", this->t_psf);
      printf("    Derotation: %f sec\n", this->t_derotate_end-this->t_derotate_begin);
      printf("    Combination: %f sec\n", this->t_combo_end-this->t_combo_begin);
   }

   int processPSFSub( const std::string & dir,
                      const std::string & prefix,
                      const std::string & ext
                    );

};

template<typename _realT, class _derotFunctObj, typename _evCalcT>
inline
void KLIPreduction<_realT, _derotFunctObj, _evCalcT>::meanSubtract(eigenCube<realT> & ims, std::vector<_realT> & norms)
{

   norms.resize(ims.planes());

   for(int n=0;n<ims.planes(); ++n)
   {
      ims.image(n) -= ims.image(n).mean();
      norms[n] = ims.image(n).matrix().norm();
   }
}
 
template<typename _realT, class _derotFunctObj, typename _evCalcT>
inline
void KLIPreduction<_realT, _derotFunctObj, _evCalcT>::medianSubtract(eigenCube<realT> & ims, std::vector<_realT> & sds)
{
         
   sds.resize(ims.planes());
   //#pragma omp parallel for schedule(static, 1)
   for(int i=0;i<ims.planes(); ++i)
   {
      _realT med = eigenMedian(ims.image(i));
      ims.image(i) -= med;
      sds[i] = ims.image(i).matrix().norm();//This is the standard deviation relative to the median.
   }
} 

template<typename _realT, class _derotFunctObj, typename _evCalcT>
inline
int KLIPreduction<_realT, _derotFunctObj, _evCalcT>::regions( std::vector<_realT> minr, 
                                                              std::vector<_realT> maxr, 
                                                              std::vector<_realT> minq, 
                                                              std::vector<_realT> maxq)
{   
   this->t_begin = get_curr_time();
   
   maxNmodes = Nmodes[0];
   for(size_t i = 1; i<Nmodes.size(); ++i)
   {
      if(Nmodes[i] > maxNmodes) maxNmodes = Nmodes[i];
   }
   
   std::cerr << "Beginning\n";
      
   if(this->imSize == 0)
   {
      this->imSize = 2*(*std::max_element(maxr.begin(),maxr.end()) + padSize);
   }
   
   if(!this->filesRead) 
   {         
      if( this->readFiles() < 0) return -1;
   }
   
   if(this->preProcess_only && !this->skipPreProcess)
   {
      std::cerr << "Pre-processing complete, stopping.\n";
      return 0;
   }

   this->psfsub.resize(Nmodes.size());
   for(size_t n=0;n<Nmodes.size(); ++n)
   {
      this->psfsub[n].resize(this->Nrows, this->Ncols, this->Nims);
      this->psfsub[n].cube().setZero();
   }
   
   //Make radius and angle images
   eigenImageT rIm(this->Nrows,this->Ncols);
   eigenImageT qIm(this->Nrows,this->Ncols);
   
   radAngImage(rIm, qIm, .5*(this->Nrows-1), .5*(this->Ncols-1));

   std::cerr << "starting regions " << minr.size() << "\n";
   //******** For each region do this:
   for(size_t regno = 0; regno < minr.size(); ++regno)
   {
      eigenImageT * maskPtr = 0;
      if( this->mask.rows() == this->Nrows && this->mask.cols() == this->Ncols) maskPtr = &this->mask;
      
      std::vector<size_t> idx = annulusIndices(rIm, qIm, .5*(this->Nrows-1), .5*(this->Ncols-1), 
                                                    minr[regno], maxr[regno], minq[regno], maxq[regno], maskPtr);
   
      //Create storage for the R-ims and psf-subbed Ims
      eigenCube<realT> rims(idx.size(), 1, this->Nims);
   
      //#pragma omp parallel for schedule(static, 1)
      for(int i=0;i< this->Nims; ++i)
      {
         auto rim = rims.image(i);
         cutImageRegion(rim, this->imc.image(i), idx, false);
      }

      realT dang = 0;
      
      if(mindpx < 0) excludeMethod = HCI::excludeNone;
      
      if(excludeMethod == HCI::excludePixel)
      {
         dang = fabs(atan(mindpx/minr[regno]));
      }
      else if(excludeMethod == HCI::excludeAngle)
      {
         dang = math::dtor(mindpx);
      }
      else if(excludeMethod == HCI::excludeImno)
      {
         dang = mindpx;
      }
      
      //*** Dispatch the work
      worker(rims, idx, dang);
      std::cerr << "worker done\n";
      
   }
   
   if(this->doDerotate)
   {
      std::cerr << "derotating\n";
      this->derotate();
   }
   
   
   if(this->combineMethod > 0)
   {
      std::cerr << "combining\n";
      this->combineFinim();
      
   }
   
   if(this->doWriteFinim == true || this->doOutputPSFSub == true)
   {
      std::cerr << "writing\n";
      
      fitsHeader head;
      
      this->ADIobservation<_realT, _derotFunctObj>::fitsHeader(&head);
      
      head.append("", fitsCommentType(), "----------------------------------------");
      head.append("", fitsCommentType(), "mx::KLIPreduction parameters:");
      head.append("", fitsCommentType(), "----------------------------------------");
   
      std::stringstream str;
      
      if(Nmodes.size() > 0)
      {
         for(size_t nm=0;nm < Nmodes.size()-1; ++nm) str << Nmodes[nm] << ",";
         str << Nmodes[Nmodes.size()-1];      
         head.append<char *>("NMODES", (char *)str.str().c_str(), "number of modes");
      }
      
      if(minr.size() > 0)
      {
         str.str("");
         for(size_t nm=0;nm < minr.size()-1; ++nm) str << minr[nm] << ",";
         str << minr[minr.size()-1];      
         head.append<char *>("REGMINR", (char *)str.str().c_str(), "region inner edge(s)");
      }
      
      if(maxr.size() > 0)
      {
         str.str("");
         for(size_t nm=0;nm < maxr.size()-1; ++nm) str << maxr[nm] << ",";
         str << maxr[maxr.size()-1];      
         head.append<char *>("REGMAXR", (char *)str.str().c_str(), "region outer edge(s)");
      }
      
      if(minq.size() > 0)
      {
         str.str("");
         for(size_t nm=0;nm < minq.size()-1; ++nm) str << minq[nm] << ",";
         str << minq[minq.size()-1];      
         head.append<char *>("REGMINQ", (char *)str.str().c_str(), "region minimum angle(s)");
      }
      
      if(maxq.size() > 0)
      {
         str.str("");
         for(size_t nm=0;nm < maxq.size()-1; ++nm) str << maxq[nm] << ",";
         str << maxq[maxq.size()-1];      
         head.append<char *>("REGMAXQ", (char *)str.str().c_str(), "region maximum angle(s)");
      }
      
      head.append<int>("EXCLMTHD", excludeMethod, "value of excludeMethod");
      head.append<realT>("MINDPX", mindpx, "minimum pixel delta");
      head.append<int>("INCLREFN", includeRefNum, "value of includeRefNum");

      if(this->doWriteFinim == true && this->combineMethod > 0)
      {
         this->writeFinim(&head);
      }
      
      if(this->doOutputPSFSub)
      {
         this->outputPSFSub(&head);
      }
   }
   
   this->t_end = get_curr_time();
 
   dump_times();
   
   return 0;
}

struct cvEntry
{
   int index;
   double cvVal;
};

//This sorts with greater-than
bool cvEntryComp( const cvEntry & cvE1, const cvEntry & cvE2)
{
   return ( fabs(cvE1.cvVal) > fabs(cvE2.cvVal) );
}

//This sorts with less-than
bool cvEntryCompIndex( const cvEntry & cvE1, const cvEntry & cvE2)
{
   return ( cvE1.index < cvE2.index );
}

template<typename eigenT, typename eigenTin>
void extractRowsAndCols(eigenT & out, const eigenTin & in, const std::vector<cvEntry> & idx)
{
   
   out.resize(idx.size(), idx.size());
   
   for(size_t i=0; i< idx.size(); ++i)
   {
      for(size_t j=0; j < idx.size(); ++j)
      {
         out(i,j) = in(idx[i].index, idx[j].index);
      }
   }
   
}

template<typename eigenT, typename eigenTin>
void extractCols(eigenT & out, const eigenTin & in, const std::vector<cvEntry> & idx)
{
   
   out.resize(in.rows(), idx.size()); 
   
   for(size_t i=0; i< idx.size(); ++i)
   {
      out.col(i) = in.col(idx[i].index); //it1->index);
   }
   
}

//template<typename realT, class derotFunctObj>
//KLIPreduction<realT, derotFunctObj>::
template<typename realT, typename eigenT, typename eigenTv, class derotFunctObj>
void collapseCovar( eigenT & cutCV, 
                                                          const eigenT & CV,
                                                          const std::vector<realT> & sds,
                                                          eigenT & rimsCut,
                                                          const eigenTv & rims,
                                                          int imno,
                                                          double dang,
                                                          int Nims,
                                                          int excludeMethod,
                                                          int includeRefNum,
                                                          const derotFunctObj & derotF
                                                        )
{   
   std::vector<cvEntry> allidx(Nims);
   
   
   //Initialize the vector cvEntries
   for(int i=0; i < Nims; ++i)
   {
      allidx[i].index = i;
      
      //CV is lower-triangular
      if(i <= imno)
      {
         allidx[i].cvVal = CV(imno,i)/(sds[i]*sds[imno]);
      }
      else
      {
         allidx[i].cvVal = CV(i,imno)/(sds[i]*sds[imno]);
      }
   }
   
   int rotoff0 = 0;
   int rotoff1 = 0;
   
   if(excludeMethod == HCI::excludePixel || excludeMethod == HCI::excludeAngle )
   {
      rotoff1 = Nims;
      
      //Find first rotoff within dang
      int j;
      for(j=0; j< Nims; ++j)
      {
         if( fabs(math::angleDiff<1>( derotF.derotAngle(j), derotF.derotAngle(imno))) <= dang )
         {
            rotoff0 = j;
            ++j;
            break;
         }
      }
      //Find first rotoff outside dang --> this is the first image that will be included again
      for(; j< Nims; ++j)
      {
         if( fabs(math::angleDiff<1>( derotF.derotAngle(j), derotF.derotAngle(imno))) > dang )
         {
            rotoff1 = j;
            break;
         }
      }
   }
   else if(excludeMethod == HCI::excludeImno)
   {
      rotoff0 = imno-dang;
      if(rotoff0 < 0) rotoff0= 0;
      
      rotoff1 = imno+dang+1;
      if(rotoff1 > Nims-1) rotoff1 = Nims;      
   }
      
   if(rotoff1-rotoff0 > 0)
   {
      
      //Note: erase(first, end()+n) does not erase the last element properly
      //      have to handle this case as a possible error 
      
      std::vector<cvEntry>::iterator last;
         
      if(rotoff1 < Nims) last = allidx.begin() + rotoff1;
      else last = allidx.begin() + (Nims-1); //Make sure we don't try to erase end() or more
      
      allidx.erase(allidx.begin()+rotoff0, last);
      
      if(rotoff1 > Nims-1) allidx.pop_back(); //Erase the last element if needed   
   }
   
   if( includeRefNum > 0 && (size_t) includeRefNum < allidx.size())
   {
      //First partially sort the correlation values
      std::nth_element(allidx.begin(), allidx.begin()+ includeRefNum, allidx.end(), cvEntryComp);
      std::cerr << "    Minimum correlation: " << allidx[includeRefNum].cvVal << "\n";
      
      //Now delete the lower correlation values
      allidx.erase(allidx.begin()+includeRefNum, allidx.end());
      
      //Now we have to re-sort the remaining indices so that the CV matrix will still be U.T.
      std::sort(allidx.begin(), allidx.end(), cvEntryCompIndex);
   }
    
   std::cerr << "  Keeping " << allidx.size() << " reference images out of " << Nims << " (" << rotoff1-rotoff0 << " rejected)\n";
   

   extractRowsAndCols(cutCV, CV, allidx);
   extractCols(rimsCut, rims, allidx);
   
}




template<typename _realT, class _derotFunctObj, typename _evCalcT>
void KLIPreduction<_realT, _derotFunctObj, _evCalcT>::worker(eigenCube<_realT> & rims, std::vector<size_t> & idx, realT dang)
{
   std::cerr << "beginning worker\n";

   t_worker_begin = get_curr_time();
   
   std::vector<realT> sds;

   //*** First mean subtract ***//   
   meanSubtract(rims, sds);  

   //*** Form lower-triangle covariance matrix      
   eigenImageT cv;
 
   math::eigenSYRK(cv, rims.cube());
      
   ompLoopWatcher<> status( this->Nims, std::cerr);
   
   //int nTh = 0;
   #pragma omp parallel //num_threads(20) 
   {
//       #ifdef _OPENMP
//          #pragma omp critical
//          std::cerr << "This is thread " << nTh++ << "\n";
//       #endif
      
      //We need local copies for each thread.  Only way this works, for whatever reason.

      eigenImageT cfs; //The coefficients
      eigenImageT psf;
      eigenImageT rims_cut;
      eigenImageT cv_cut;
      eigenImageT klims;
      
      math::syevrMem<evCalcT> mem;

      if( excludeMethod == HCI::excludeNone )
      {
         /**** Now calculate the K-L Images ****/
      
         //calcKLIms(klims, cv, rims.cube(), maxNmodes);
         math::calcKLModes(klims, cv, rims.cube(), maxNmodes);
      }
   
      #pragma omp for 
      for(int imno = 0; imno < this->Nims; ++imno)
      {
         //if(imno != 847) continue;
         
         status.incrementAndOutputStatus();
         
         if( excludeMethod != HCI::excludeNone )
         {         
            /// \bug 2017-07-18 this omp critical is necessary on jetstream install.  Investigation needed. 
            //Symptom is segfaults after running for a while.  
            //Note that this may have been fixed by switching to satlas from tatlas, but probably not 
            //Possible problem: this uses rims which is not thread private.
            //#pragma omp critical
            //{
               collapseCovar<realT>( cv_cut,  cv, sds, rims_cut, rims.asVectors(), imno, dang, this->Nims, this->excludeMethod, this->includeRefNum, this->derotF);
            //} 
            
            /**** Now calculate the K-L Images ****/
            //calcKLIms(klims, cv_cut, rims_cut, maxNmodes, &mem);
            math::calcKLModes(klims, cv_cut, rims_cut, maxNmodes, &mem);
         }
         cfs.resize(1, klims.rows());
   
  
         double t0 = get_curr_time();
         
         for(int j=0; j<cfs.size(); ++j)
         {
            cfs(j) = klims.row(j).matrix().dot(rims.cube().col(imno).matrix());    
         }

         for(size_t mode_i =0; mode_i < Nmodes.size(); ++mode_i)
         {
            psf = cfs(cfs.size()-1)*klims.row(cfs.size()-1);

            //Count down, since eigenvalues are returned in increasing order
            //  handle case where cfs.size() < Nmodes[mode_i], i.e. when more modes than images.
            for(int j=cfs.size()-2; j>=cfs.size()-Nmodes[mode_i] && j >= 0; --j)
            {
               psf += cfs(j)*klims.row(j);
            }  
            
            //#pragma omp critical
            insertImageRegion( this->psfsub[mode_i].cube().col(imno), rims.cube().col(imno) - psf.transpose(), idx);
         }
         

         t_psf = (get_curr_time() - t0) ;/// omp_get_num_threads();
         
         
      } //for imno
   }//openmp parrallel  
   
   t_worker_end = get_curr_time();
}


template<typename _realT, class _derotFunctObj, typename _evCalcT>
inline
int KLIPreduction<_realT, _derotFunctObj, _evCalcT>::processPSFSub( const std::string & dir,
                                                                    const std::string & prefix,
                                                                    const std::string & ext
                                                                  )

{   
  
   
   std::cerr << "Beginning\n";
      

   this->readPSFSub(dir, prefix, ext, Nmodes.size());
   
   
   //This is generic to both regions and this from here on . . .
   
   if(this->doDerotate)
   {
      std::cerr << "derotating\n";
      this->derotate();
   }
   
   
   if(this->combineMethod > 0)
   {
      std::cerr << "combining\n";
      this->combineFinim();
      
   }
   
   if(this->doWriteFinim == true || this->doOutputPSFSub == true)
   {
      std::cerr << "writing\n";
      
      fitsHeader head;
      
      this->ADIobservation<_realT, _derotFunctObj>::fitsHeader(&head);
      
      head.append("", fitsCommentType(), "----------------------------------------");
      head.append("", fitsCommentType(), "mx::KLIPreduction parameters:");
      head.append("", fitsCommentType(), "----------------------------------------");
   
      std::stringstream str;
      
      if(Nmodes.size() > 0)
      {
         for(size_t nm=0;nm < Nmodes.size()-1; ++nm) str << Nmodes[nm] << ",";
         str << Nmodes[Nmodes.size()-1];      
         head.append<char *>("NMODES", (char *)str.str().c_str(), "number of modes");
      }
      
//       if(minr.size() > 0)
//       {
//          str.str("");
//          for(size_t nm=0;nm < minr.size()-1; ++nm) str << minr[nm] << ",";
//          str << minr[minr.size()-1];      
//          head.append<char *>("REGMINR", (char *)str.str().c_str(), "region inner edge(s)");
//       }
//       
//       if(maxr.size() > 0)
//       {
//          str.str("");
//          for(size_t nm=0;nm < maxr.size()-1; ++nm) str << maxr[nm] << ",";
//          str << maxr[maxr.size()-1];      
//          head.append<char *>("REGMAXR", (char *)str.str().c_str(), "region outer edge(s)");
//       }
//       
//       if(minq.size() > 0)
//       {
//          str.str("");
//          for(size_t nm=0;nm < minq.size()-1; ++nm) str << minq[nm] << ",";
//          str << minq[minq.size()-1];      
//          head.append<char *>("REGMINQ", (char *)str.str().c_str(), "region minimum angle(s)");
//       }
//       
//       if(maxq.size() > 0)
//       {
//          str.str("");
//          for(size_t nm=0;nm < maxq.size()-1; ++nm) str << maxq[nm] << ",";
//          str << maxq[maxq.size()-1];      
//          head.append<char *>("REGMAXQ", (char *)str.str().c_str(), "region maximum angle(s)");
//       }
      
      head.append<int>("EXCLMTHD", excludeMethod, "value of excludeMethod");
      head.append<realT>("MINDPX", mindpx, "minimum pixel delta");
      head.append<int>("INCLREFN", includeRefNum, "value of includeRefNum");

      if(this->doWriteFinim == true && this->combineMethod > 0)
      {
         this->writeFinim(&head);
      }
      
      if(this->doOutputPSFSub)
      {
         this->outputPSFSub(&head);
      }
   }
   
   
   return 0;
}

///@}

} //namespace improc
} //namespace mx

#endif //__KLIPreduction_hpp__

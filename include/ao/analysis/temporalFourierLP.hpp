
#ifndef temporalFourierLP_hpp
#define temporalFourierLP_hpp


#include <vector>

#include "../../math/geo.hpp"
#include "../../sigproc/psdUtils.hpp"
#include "../../sigproc/autocorrelation.hpp"
#include "../../sigproc/linearPredictor.hpp"

#include "clGainOpt.hpp"

namespace mx
{
namespace AO
{
namespace analysis 
{
   
template<typename realT>
struct temporalFourierLP
{
   std::vector<realT> PSDtn;
   
   std::vector<realT> psd2s;
   
   std::vector<realT> ac;
   
   sigproc::autocorrelationFromPSD<realT> acpsd;

   sigproc::linearPredictor<realT> lp;
   
   temporalFourierLP()
   {
   }
   
   ///Calculate the LP coefficients for a turbulence PSD and a noise PSD.
   /** This combines the two PSDs, and augments to two-sided, and calls the linearPredictor.calcCoefficients method.
    */
   int calcCoefficients( std::vector<realT> & PSDt, 
                         std::vector<realT> & PSDn, 
                         std::vector<realT> & PSDreg,
                         int Nc 
                       )
   {
      PSDtn.resize(PSDt.size());

      for(int i=0; i< PSDt.size(); ++i)
      {
         PSDtn[i] = PSDt[i] +  PSDn[i] + PSDreg[i];
      }
      
      sigproc::augment1SidedPSD( psd2s, PSDtn,1);

      ac.resize(psd2s.size());
   
      //#pragma omp critical
      acpsd(ac, psd2s);

      return lp.calcCoefficients(ac, Nc);
   }
   
   template< bool printout=false>
   int regularizeCoefficients( clGainOpt<realT> & go_lp,
                               realT & gmax_lp,
                               realT & gopt_lp,
                               realT & var_lp,
                               std::vector<realT> & PSDt, 
                               std::vector<realT> & PSDn, 
                               int Nc
                             )
   {
      realT min_var = std::numeric_limits<realT>::max();
      realT min_sc;
      realT max_sc;
   
      std::vector<realT> rpsd;
      
      realT last_gmax = 0;
      for(double sc = 10.0; sc < 101.0; sc += 10.0)
      {
         rpsd.clear();
         rpsd.resize(go_lp.f_size(), PSDt[0]*pow(10, -sc/10));
   
         if( calcCoefficients(PSDt, PSDn, rpsd, Nc) < 0) return -1;
   
         go_lp.a(lp._c);
         go_lp.b(lp._c);

         realT ll = 0, ul = 0;
         gmax_lp = go_lp.maxStableGain(ll,ul);
         gopt_lp = go_lp.optGainOpenLoop(PSDt, PSDn, gmax_lp);
         var_lp = go_lp.clVariance(PSDt, PSDn, gopt_lp);
      
         if(printout)
         {
            std::cout << -sc/10 << " " << gmax_lp << " " << gopt_lp << " " << var_lp << "\n";
         }
         
         if( var_lp < min_var )
         {
            min_var = var_lp;
            min_sc = sc;
         }
   
         max_sc = sc;
         
         
         if( last_gmax/gmax_lp > 100 ) break;
         
         last_gmax = gmax_lp;
      }

      last_gmax = 0;
      if( min_sc != max_sc && min_sc != 10.0)
      {
         realT sc0 = min_sc*2;
         for(double sc = sc0-10; sc < sc0+11; ++sc)
         {
            rpsd.clear();
            rpsd.resize(go_lp.f_size(), PSDt[0]*pow(10, -sc/20));
   
            if( calcCoefficients(PSDt, PSDn, rpsd, Nc) < 0) return -1;
   
            go_lp.a(lp._c);
            go_lp.b(lp._c);

            realT ll = 0, ul = 0;
            gmax_lp = go_lp.maxStableGain(ll,ul);
            gopt_lp = go_lp.optGainOpenLoop(PSDt, PSDn, gmax_lp);
      
            var_lp = go_lp.clVariance(PSDt, PSDn, gopt_lp);
      
            if(printout)
            {
               std::cout << -sc/20 << " " << gmax_lp << " " << gopt_lp << " " << var_lp << "\n";
            }
            
            if( var_lp < min_var )
            {
               min_var = var_lp;
               min_sc = sc/2.0;
            }
            
            if( last_gmax/gmax_lp > 100 ) break;
         
            last_gmax = gmax_lp;
         
         }
      }

      //Now record final values
      rpsd.clear();
      rpsd.resize(go_lp.f_size(), PSDt[0]*pow(10, -min_sc/10));
   
      if(calcCoefficients(PSDt, PSDn, rpsd, Nc) < 0) return -1;
   
      go_lp.a(lp._c);
      go_lp.b(lp._c);
      
      realT ll = 0, ul = 0;
      gmax_lp = go_lp.maxStableGain(ll,ul);
      gopt_lp = go_lp.optGainOpenLoop(PSDt, PSDn, gmax_lp);
      
      var_lp = go_lp.clVariance(PSDt, PSDn, gopt_lp);
      
      return 0;
   }
   
};

}//namespace analysis
}//namespace ao
}//namespace mx

#endif // temporalFourierLP_hpp

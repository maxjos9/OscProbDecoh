////////////////////////////////////////////////////////////////////////
//
// Implementation of oscillations of neutrinos in matter in a
// three-neutrino framework with NSI.
//
// This  class inherits from the PMNS_Fast class
//
// coelho@lal.in2p3.fr
////////////////////////////////////////////////////////////////////////

#include "PMNS_NSI.h"
#include "MatrixDecomp/zheevh3.h"

#include <iostream>
#include <cassert>
#include <stdlib.h>

using namespace OscProb;

using namespace std;


//......................................................................
///
/// Constructor. \sa PMNS_Base::PMNS_Base
///
/// This class is restricted to 3 neutrino flavours.
///
PMNS_NSI::PMNS_NSI() : PMNS_Fast(), fEps()
{
  SetStdPath();
  SetNSI(0.,0.,0.,0.,0.,0.,0.,0.,0.);
}

//......................................................................
///
/// Nothing to clean.
///
PMNS_NSI::~PMNS_NSI(){}

//......................................................................
///
/// Set all NSI parameters at once.
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param eps_ee      - The real parameter eps_ee
/// @param eps_emu     - The absolute value of the complex parameter eps_emu
/// @param eps_etau    - The absolute value of the complex parameter eps_etau
/// @param eps_mumu    - The real parameter eps_mumu
/// @param eps_mutau   - The absolute value of the complex parameter eps_mutau
/// @param eps_tautau  - The real parameter eps_tautau
/// @param delta_emu   - The phase of the complex parameter eps_emu in radians
/// @param delta_etau  - The phase of the complex parameter eps_etau in radians
/// @param delta_mutau - The phase of the complex parameter eps_mutau in radians
///
void PMNS_NSI::SetNSI(double eps_ee,    double eps_emu,    double eps_etau,
                       double eps_mumu,  double eps_mutau,  double eps_tautau,
                       double delta_emu, double delta_etau, double delta_mutau) 
{

  SetEps(0,0, eps_ee,     0);
  SetEps(1,1, eps_mumu,   0);
  SetEps(2,2, eps_tautau, 0);

  SetEps(0,1, eps_emu,   delta_emu);
  SetEps(0,2, eps_etau,  delta_etau);
  SetEps(1,2, eps_mutau, delta_mutau);

}

//......................................................................
///
/// Set any given NSI parameter.
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// Flavours are:\n
/// - 0 = nue, 1 = numu, 2 = nutau
///
/// Requires that flvi < flvj. Will notify you if input is wrong.
/// If flvi > flvj, will assume reverse order and swap flvi and flvj.
///
/// @param flvi  - The first flavour index 
/// @param flvj  - The second flavour index
/// @param val   - The absolute value of the parameter
/// @param phase - The complex phase of the parameter in radians
///
void PMNS_NSI::SetEps(int flvi, int flvj, double val, double phase){

  if(flvi > flvj){
    cout << "First argument should be smaller or equal to second argument" << endl;
    cout << "Setting reverse order (Eps_" << flvj << flvi << "). " << endl;
    int temp = flvi;
    flvi = flvj;
    flvj = temp;
  }
  if(flvi<0 || flvi>2 || flvj < flvi || flvj > 2){
    cout << "Eps_" << flvi << flvj << " not valid for " << fNumNus;
    cout << " neutrinos. Doing nothing." << endl;
    return;
  }

  complex h = val;  

  if(flvi != flvj) h *= complex(cos(phase), sin(phase)); 

  else if(flvi == 0) h += 1;

  fGotES *= (fEps[flvi][flvj] == h);
  
  fEps[flvi][flvj] = h;
  
}

//......................................................................
///
/// Get any given NSI parameter.
///
/// Flavours are:\n
/// - 0 = nue, 1 = numu, 2 = nutau
///
/// Requires that flvi < flvj. Will notify you if input is wrong.
/// If flvi > flvj, will assume reverse order and swap flvi and flvj.
///
/// @param flvi  - The first flavour index
/// @param flvj  - The second flavour index
///
complex<double> PMNS_NSI::GetEps(int flvi, int flvj){

  if(flvi > flvj){
    cout << "First argument should be smaller or equal to second argument" << endl;
    cout << "Setting reverse order (Eps_" << flvj << flvi << "). " << endl;
    int temp = flvi;
    flvi = flvj;
    flvj = temp;
  }
  if(flvi<0 || flvi>2 || flvj < flvi || flvj > 2){
    cout << "Eps_" << flvi << flvj << " not valid for " << fNumNus;
    cout << " neutrinos. Returning 0." << endl;
    return zero;
  }

  return fEps[flvi][flvj];

}

//......................................................................
///
/// Set eps_ee parameter
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param a - The value of the real parameter eps_ee
///
void PMNS_NSI::SetEps_ee(double a){ SetEps(0,0, a, 0); }

//......................................................................
///
/// Set eps_mumu parameter
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param a - The value of the real parameter eps_mumu
///
void PMNS_NSI::SetEps_mumu(double a){ SetEps(1,1, a, 0); }

//......................................................................
///
/// Set eps_tautau parameter
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param a - The value of the real parameter eps_tautau
///
void PMNS_NSI::SetEps_tautau(double a){ SetEps(2,2, a, 0); }

//......................................................................
///
/// Set eps_emu parameter
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param a   - The absolute value of the parameter eps_emu
/// @param phi - The phase of the complex parameter eps_emu in radians
///
void PMNS_NSI::SetEps_emu  (double a, double phi){ SetEps(0,1, a, phi); }

//......................................................................
///
/// Set eps_etau parameter
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param a   - The absolute value of the parameter eps_etau
/// @param phi - The phase of the complex parameter eps_etau in radians
///
void PMNS_NSI::SetEps_etau  (double a, double phi){ SetEps(0,2, a, phi); }

//......................................................................
///
/// Set eps_mutau parameter
///
/// This will check if value is changing to keep track of whether
/// the eigensystem needs to be recomputed.
///
/// @param a   - The absolute value of the parameter eps_mutau
/// @param phi - The phase of the complex parameter eps_mutau in radians
///
void PMNS_NSI::SetEps_mutau  (double a, double phi){ SetEps(1,2, a, phi); }

//......................................................................
///
/// Build the full Hamiltonian in matter
///
/// Here we divide the mass squared matrix Hms by the 2E
/// to obtain the vacuum Hamiltonian in eV. Then, the matter
/// potential is added to each flavour pair.
///
void PMNS_NSI::UpdateHam()
{

  double lv = 2 * kGeV2eV*fEnergy;           // 2*E in eV 

  double kr2GNe = kK2*M_SQRT2*kGf;
  kr2GNe *= fPath.density * fPath.zoa; // Matter potential in eV


  // Finish build Hamiltonian in matter with dimension of eV
  for(int i=0;i<fNumNus;i++){
    for(int j=i;j<fNumNus;j++){
      if(!fIsNuBar) fHam[i][j] = fHms[i][j]/lv + kr2GNe*fEps[i][j];
      else          fHam[i][j] = conj(fHms[i][j]/lv - kr2GNe*fEps[i][j]);
    }
  }

}

////////////////////////////////////////////////////////////////////////

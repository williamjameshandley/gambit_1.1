// ====================================================================
// This file is part of FlexibleSUSY.
//
// FlexibleSUSY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// FlexibleSUSY is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with FlexibleSUSY.  If not, see
// <http://www.gnu.org/licenses/>.
// ====================================================================

/**
 * @file SingletDM_two_scale_model_slha.hpp
 * @brief contains wrapper class for model class in SLHA convention
 */

// File generated at Sat 20 Feb 2016 16:21:37

#ifndef SingletDM_TWO_SCALE_SLHA_H
#define SingletDM_TWO_SCALE_SLHA_H

#include "SingletDM_two_scale_model.hpp"
#include "SingletDM_physical.hpp"
#include "SingletDM_model_slha.hpp"
#include "wrappers.hpp"

#define PHYSICAL_SLHA(p) physical_slha.p
#define PHYSICAL_SLHA_REAL(p) Re(physical_slha.p)

namespace flexiblesusy {

class Two_scale;

/**
 * @class SingletDM_slha<Two_scale>
 * @brief model class wrapper in SLHA convention
 */

template<>
class SingletDM_slha<Two_scale> : public SingletDM<Two_scale> {
public:
   explicit SingletDM_slha(const SingletDM_input_parameters& input_ = SingletDM_input_parameters());
   explicit SingletDM_slha(const SingletDM<Two_scale>&);
   virtual ~SingletDM_slha();

   virtual void clear();
   void convert_to_slha(); ///< converts pole masses and couplings to SLHA convention
   const Eigen::Matrix<std::complex<double>,3,3>& get_ckm_matrix() const { return ckm; }
   const Eigen::Matrix<std::complex<double>,3,3>& get_pmns_matrix() const { return pmns; }
   const SingletDM_physical& get_physical_slha() const; ///< returns pole masses to SLHA convention
   SingletDM_physical& get_physical_slha(); ///< returns pole masses to SLHA convention

   // interface functions
   virtual void calculate_spectrum();
   virtual void print(std::ostream&) const;

   double get_MHp_pole_slha() const { return PHYSICAL_SLHA(MHp); }
   double get_Mss_pole_slha() const { return PHYSICAL_SLHA(Mss); }
   const Eigen::Array<double,3,1>& get_MFv_pole_slha() const { return PHYSICAL_SLHA(MFv); }
   double get_MFv_pole_slha(int i) const { return PHYSICAL_SLHA(MFv(i)); }
   double get_MAh_pole_slha() const { return PHYSICAL_SLHA(MAh); }
   double get_Mhh_pole_slha() const { return PHYSICAL_SLHA(Mhh); }
   double get_MVZ_pole_slha() const { return PHYSICAL_SLHA(MVZ); }
   const Eigen::Array<double,3,1>& get_MFd_pole_slha() const { return PHYSICAL_SLHA(MFd); }
   double get_MFd_pole_slha(int i) const { return PHYSICAL_SLHA(MFd(i)); }
   const Eigen::Array<double,3,1>& get_MFu_pole_slha() const { return PHYSICAL_SLHA(MFu); }
   double get_MFu_pole_slha(int i) const { return PHYSICAL_SLHA(MFu(i)); }
   const Eigen::Array<double,3,1>& get_MFe_pole_slha() const { return PHYSICAL_SLHA(MFe); }
   double get_MFe_pole_slha(int i) const { return PHYSICAL_SLHA(MFe(i)); }
   double get_MVG_pole_slha() const { return PHYSICAL_SLHA(MVG); }
   double get_MVP_pole_slha() const { return PHYSICAL_SLHA(MVP); }
   double get_MVWp_pole_slha() const { return PHYSICAL_SLHA(MVWp); }

   const Eigen::Matrix<std::complex<double>,3,3>& get_Vd_pole_slha() const { return PHYSICAL_SLHA(Vd); }
   const std::complex<double>& get_Vd_pole_slha(int i, int k) const { return PHYSICAL_SLHA(Vd(i,k)); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Ud_pole_slha() const { return PHYSICAL_SLHA(Ud); }
   const std::complex<double>& get_Ud_pole_slha(int i, int k) const { return PHYSICAL_SLHA(Ud(i,k)); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Vu_pole_slha() const { return PHYSICAL_SLHA(Vu); }
   const std::complex<double>& get_Vu_pole_slha(int i, int k) const { return PHYSICAL_SLHA(Vu(i,k)); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Uu_pole_slha() const { return PHYSICAL_SLHA(Uu); }
   const std::complex<double>& get_Uu_pole_slha(int i, int k) const { return PHYSICAL_SLHA(Uu(i,k)); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Ve_pole_slha() const { return PHYSICAL_SLHA(Ve); }
   const std::complex<double>& get_Ve_pole_slha(int i, int k) const { return PHYSICAL_SLHA(Ve(i,k)); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Ue_pole_slha() const { return PHYSICAL_SLHA(Ue); }
   const std::complex<double>& get_Ue_pole_slha(int i, int k) const { return PHYSICAL_SLHA(Ue(i,k)); }

   const Eigen::Array<double,3,1>& get_Yu_slha() const { return Yu_slha; }
   double get_Yu_slha(int i) const { return Yu_slha(i); }
   const Eigen::Array<double,3,1>& get_Yd_slha() const { return Yd_slha; }
   double get_Yd_slha(int i) const { return Yd_slha(i); }
   const Eigen::Array<double,3,1>& get_Ye_slha() const { return Ye_slha; }
   double get_Ye_slha(int i) const { return Ye_slha(i); }



   const Eigen::Matrix<std::complex<double>,3,3>& get_Vd_slha() const { return Vd_slha; }
   const std::complex<double>& get_Vd_slha(int i, int k) const { return Vd_slha(i,k); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Vu_slha() const { return Vu_slha; }
   const std::complex<double>& get_Vu_slha(int i, int k) const { return Vu_slha(i,k); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Ud_slha() const { return Ud_slha; }
   const std::complex<double>& get_Ud_slha(int i, int k) const { return Ud_slha(i,k); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Uu_slha() const { return Uu_slha; }
   const std::complex<double>& get_Uu_slha(int i, int k) const { return Uu_slha(i,k); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Ve_slha() const { return Ve_slha; }
   const std::complex<double>& get_Ve_slha(int i, int k) const { return Ve_slha(i,k); }
   const Eigen::Matrix<std::complex<double>,3,3>& get_Ue_slha() const { return Ue_slha; }
   const std::complex<double>& get_Ue_slha(int i, int k) const { return Ue_slha(i,k); }


private:
   SingletDM_physical physical_slha; ///< contains the pole masses and mixings in slha convention
   Eigen::Matrix<std::complex<double>,3,3> ckm;
   Eigen::Matrix<std::complex<double>,3,3> pmns;
   Eigen::Array<double,3,1> Yu_slha;
   Eigen::Array<double,3,1> Yd_slha;
   Eigen::Array<double,3,1> Ye_slha;

   Eigen::Matrix<std::complex<double>,3,3> Vd_slha;
   Eigen::Matrix<std::complex<double>,3,3> Vu_slha;
   Eigen::Matrix<std::complex<double>,3,3> Ud_slha;
   Eigen::Matrix<std::complex<double>,3,3> Uu_slha;
   Eigen::Matrix<std::complex<double>,3,3> Ve_slha;
   Eigen::Matrix<std::complex<double>,3,3> Ue_slha;




   void calculate_ckm_matrix();
   void calculate_pmns_matrix();
   void convert_yukawa_couplings_to_slha();
   void convert_trilinear_couplings_to_slha();
   void convert_soft_squared_masses_to_slha();
};

} // namespace flexiblesusy

#undef PHYSICAL_SLHA
#undef PHYSICAL_SLHA_REAL

#endif

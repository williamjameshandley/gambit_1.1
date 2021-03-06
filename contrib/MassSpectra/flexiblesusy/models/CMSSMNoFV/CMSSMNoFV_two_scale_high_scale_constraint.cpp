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

// File generated at Sat 27 Aug 2016 12:48:18

#include "CMSSMNoFV_two_scale_high_scale_constraint.hpp"
#include "CMSSMNoFV_two_scale_model.hpp"
#include "wrappers.hpp"
#include "logger.hpp"
#include "ew_input.hpp"
#include "gsl_utils.hpp"
#include "minimizer.hpp"
#include "root_finder.hpp"
#include "threshold_loop_functions.hpp"
#include "numerics2.hpp"

#include <cassert>
#include <cmath>
#include <cerrno>
#include <cstring>

namespace flexiblesusy {

#define DERIVEDPARAMETER(p) model->p()
#define INPUTPARAMETER(p) model->get_input().p
#define MODELPARAMETER(p) model->get_##p()
#define PHASE(p) model->get_##p()
#define BETAPARAMETER(p) beta_functions.get_##p()
#define BETA(p) beta_##p
#define LowEnergyConstant(p) Electroweak_constants::p
#define MZPole Electroweak_constants::MZ
#define STANDARDDEVIATION(p) Electroweak_constants::Error_##p
#define Pole(p) model->get_physical().p
#define SCALE model->get_scale()
#define THRESHOLD static_cast<int>(model->get_thresholds())
#define MODEL model
#define MODELCLASSNAME CMSSMNoFV<Two_scale>

CMSSMNoFV_high_scale_constraint<Two_scale>::CMSSMNoFV_high_scale_constraint()
   : Constraint<Two_scale>()
   , scale(0.)
   , initial_scale_guess(0.)
   , model(0)
{
}

CMSSMNoFV_high_scale_constraint<Two_scale>::CMSSMNoFV_high_scale_constraint(
   CMSSMNoFV<Two_scale>* model_)
   : Constraint<Two_scale>()
   , model(model_)
{
   initialize();
}

CMSSMNoFV_high_scale_constraint<Two_scale>::~CMSSMNoFV_high_scale_constraint()
{
}

void CMSSMNoFV_high_scale_constraint<Two_scale>::apply()
{
   assert(model && "Error: CMSSMNoFV_high_scale_constraint::apply():"
          " model pointer must not be zero");



   update_scale();

   const auto Azero = INPUTPARAMETER(Azero);
   const auto m0 = INPUTPARAMETER(m0);
   const auto m12 = INPUTPARAMETER(m12);
   const auto Ye = MODELPARAMETER(Ye);
   const auto Yd = MODELPARAMETER(Yd);
   const auto Yu = MODELPARAMETER(Yu);

   MODEL->set_TYe((Azero*Ye).real());
   MODEL->set_TYd((Azero*Yd).real());
   MODEL->set_TYu((Azero*Yu).real());
   MODEL->set_mHd2(Re(Sqr(m0)));
   MODEL->set_mHu2(Re(Sqr(m0)));
   MODEL->set_mq2((Sqr(m0)*UNITMATRIX(3)).real());
   MODEL->set_ml2((Sqr(m0)*UNITMATRIX(3)).real());
   MODEL->set_md2((Sqr(m0)*UNITMATRIX(3)).real());
   MODEL->set_mu2((Sqr(m0)*UNITMATRIX(3)).real());
   MODEL->set_me2((Sqr(m0)*UNITMATRIX(3)).real());
   MODEL->set_MassB(Re(m12));
   MODEL->set_MassWB(Re(m12));
   MODEL->set_MassG(Re(m12));


   check_non_perturbative();


}

bool CMSSMNoFV_high_scale_constraint<Two_scale>::check_non_perturbative()
{
   bool problem = false;

   const auto g1 = MODELPARAMETER(g1);
   const auto g2 = MODELPARAMETER(g2);
   const auto g3 = MODELPARAMETER(g3);
   const auto Yd = MODELPARAMETER(Yd);
   const auto Ye = MODELPARAMETER(Ye);
   const auto Yu = MODELPARAMETER(Yu);

   if (MaxAbsValue(g1) > 3.5449077018110318) {
      problem = true;
      model->get_problems().flag_non_perturbative_parameter("g1", MaxAbsValue(g1), model->get_scale(), 3.5449077018110318);
   } else {
      model->get_problems().unflag_non_perturbative_parameter("g1");
   }
   if (MaxAbsValue(g2) > 3.5449077018110318) {
      problem = true;
      model->get_problems().flag_non_perturbative_parameter("g2", MaxAbsValue(g2), model->get_scale(), 3.5449077018110318);
   } else {
      model->get_problems().unflag_non_perturbative_parameter("g2");
   }
   if (MaxAbsValue(g3) > 3.5449077018110318) {
      problem = true;
      model->get_problems().flag_non_perturbative_parameter("g3", MaxAbsValue(g3), model->get_scale(), 3.5449077018110318);
   } else {
      model->get_problems().unflag_non_perturbative_parameter("g3");
   }
   if (MaxAbsValue(Yd) > 3.5449077018110318) {
      problem = true;
      model->get_problems().flag_non_perturbative_parameter("Yd", MaxAbsValue(Yd), model->get_scale(), 3.5449077018110318);
   } else {
      model->get_problems().unflag_non_perturbative_parameter("Yd");
   }
   if (MaxAbsValue(Ye) > 3.5449077018110318) {
      problem = true;
      model->get_problems().flag_non_perturbative_parameter("Ye", MaxAbsValue(Ye), model->get_scale(), 3.5449077018110318);
   } else {
      model->get_problems().unflag_non_perturbative_parameter("Ye");
   }
   if (MaxAbsValue(Yu) > 3.5449077018110318) {
      problem = true;
      model->get_problems().flag_non_perturbative_parameter("Yu", MaxAbsValue(Yu), model->get_scale(), 3.5449077018110318);
   } else {
      model->get_problems().unflag_non_perturbative_parameter("Yu");
   }


   return problem;
}

double CMSSMNoFV_high_scale_constraint<Two_scale>::get_scale() const
{
   return scale;
}

double CMSSMNoFV_high_scale_constraint<Two_scale>::get_initial_scale_guess() const
{
   return initial_scale_guess;
}

const CMSSMNoFV_input_parameters& CMSSMNoFV_high_scale_constraint<Two_scale>::get_input_parameters() const
{
   return model->get_input();
}

CMSSMNoFV<Two_scale>* CMSSMNoFV_high_scale_constraint<Two_scale>::get_model() const
{
   return model;
}

void CMSSMNoFV_high_scale_constraint<Two_scale>::set_model(Two_scale_model* model_)
{
   model = cast_model<CMSSMNoFV<Two_scale>*>(model_);
}

void CMSSMNoFV_high_scale_constraint<Two_scale>::set_scale(double s)
{
   scale = s;
}

void CMSSMNoFV_high_scale_constraint<Two_scale>::clear()
{
   scale = 0.;
   initial_scale_guess = 0.;
   model = NULL;
}

void CMSSMNoFV_high_scale_constraint<Two_scale>::initialize()
{
   assert(model && "CMSSMNoFV_high_scale_constraint<Two_scale>::"
          "initialize(): model pointer is zero.");

   initial_scale_guess = 2.e16;

   scale = initial_scale_guess;
}

void CMSSMNoFV_high_scale_constraint<Two_scale>::update_scale()
{
   assert(model && "CMSSMNoFV_high_scale_constraint<Two_scale>::"
          "update_scale(): model pointer is zero.");

   const double currentScale = model->get_scale();
   const CMSSMNoFV_soft_parameters beta_functions(model->calc_beta());

   const auto g1 = MODELPARAMETER(g1);
   const auto g2 = MODELPARAMETER(g2);
   const auto beta_g1 = BETAPARAMETER(g1);
   const auto beta_g2 = BETAPARAMETER(g2);

   scale = currentScale*Exp((-g1 + g2)/(BETA(g1) - BETA(g2)));


   if (errno == ERANGE) {
#ifdef ENABLE_VERBOSE
      ERROR("CMSSMNoFV_high_scale_constraint<Two_scale>: Overflow error"
            " during calculation of high scale: " << strerror(errno) << '\n'
            << "   current scale = " << currentScale << '\n'
            << "   new scale = " << scale << '\n'
            << "   resetting scale to " << get_initial_scale_guess());
#endif
      scale = get_initial_scale_guess();
      errno = 0;
   }


}

} // namespace flexiblesusy

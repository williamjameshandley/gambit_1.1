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

// File generated at Sat 20 Feb 2016 16:21:19

#ifndef SingletDM_INPUT_PARAMETERS_H
#define SingletDM_INPUT_PARAMETERS_H

#include <complex>
#include <Eigen/Core>

namespace flexiblesusy {

struct SingletDM_input_parameters {
   double HiggsIN;
   double LamSHInput;
   double LamSInput;
   double mS2Input;
   double QEWSB;
   double Qin;

   SingletDM_input_parameters()
      : HiggsIN(0), LamSHInput(0), LamSInput(0), mS2Input(0), QEWSB(0), Qin(0)

   {}
};

std::ostream& operator<<(std::ostream&, const SingletDM_input_parameters&);

} // namespace flexiblesusy

#endif

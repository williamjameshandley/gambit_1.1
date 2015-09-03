//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Core-only functions for triggering 
///  initialisation code.
///
///  *********************************************
///
///  Authors
///  =======
///
///  (add name and date if you modify)
///
///  \author Pat Scott 
///          (p.scott@imperial.ac.uk)
///  \date 2015 Feb
///
///  *********************************************

#include "gambit/Elements/ini_functions.hpp"
#include "gambit/Core/ini_functions.hpp"
#include "gambit/Core/core_singleton.hpp"

namespace Gambit
{
    
  /// Register a module with the Core.
  int register_module(str module)
  {
    try
    {
      Core().registerModule(module);
    }
    catch (std::exception& e) { ini_catch(e); }
    return 0;
  }

  /// Register a module functor with the Core.
  int register_module_functor_core(module_functor_common& f)
  {
    try
    {
      Core().registerModuleFunctor(f);
    }
    catch (std::exception& e) { ini_catch(e); }
    return 0;    
  }

  /// Register a model functor with the Core.
  int register_model_functor_core(primary_model_functor& primary_parameters)
  {
    try
    {
      Core().registerPrimaryModelFunctor(primary_parameters);
    }
    catch (std::exception& e) { ini_catch(e); }
    return 0;    
  }

  /// Register a backend with the Core
  int register_backend(str be, str ver)
  {
    try
    {
      Core().registerBackend(be, ver);
    }
    catch (std::exception& e) { ini_catch(e); }
    return 0;    
  }
  
  /// Register a backend functor with the Core
  int register_backend_functor(functor& f)
  {
    try
    {
      Core().registerBackendFunctor(f);
    }
    catch (std::exception& e) { ini_catch(e); }
    return 0;    
  }
  
    /// Register a loop management requirement with the Core
  int register_management_req(module_functor_common& f)
  {
    try
    {
      Core().registerNestedModuleFunctor(f);
    }
    catch (std::exception& e) { ini_catch(e); }
    return 0;    
  }

}
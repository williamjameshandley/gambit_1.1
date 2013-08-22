
#ifndef GAMBIT_MODULE_INTERFACE_HPP
#define GAMBIT_MODULE_INTERFACE_HPP

#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <functors.hpp>
#include <graphs.hpp>
#include <dlfcn.h>

namespace GAMBIT
{
        namespace Module
        {
                const std::string sblank = std::string("       \033[7D");
                const std::string blank = std::string("               \033[15D");
                const std::string bblank = std::string("                              \033[30D");          
                
                inline bool vector_elem_check(std::vector<std::string> &vec, std::string elem)
                {
                        for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
                        {
                                if ((*it) == elem)
                                {
                                        return true;
                                }
                        }
                        
                        return false;
                }
                
                inline void clearInputBuffer()
                {
                        char c;
                        do 
                        {
                                c = getchar();
                        } 
                        while (c != '\n' && c != EOF);
                }
                
                template <typename T>
                class Module_Interface
                {
                private:
                        bool open;
                        void *plugin;
                        std::string errors;
                        std::string name;
                        std::vector<std::string> mod_names;
                        typedef void (*inputFuncType)(std::string, std::string);
                        typedef void (*initFuncType)(std::vector<void *> *, std::vector<std::string> &, void *);                              
                        typedef bool (*defFuncType)(std::string);
                        typedef void * (*getFuncType)(std::string);
                        typedef void (*rmFuncType)(std::string);
                        inputFuncType inputFunc;
                        initFuncType initFunc;
                        defFuncType defFunc;
                        getFuncType getFunc;
                        rmFuncType rmFunc;
                        
                public:
                        T* main;
                        
                        Module_Interface(std::string file, std::string name_in, const IniParser::IniFile *boundIniFile = NULL, std::vector<void*> *input = NULL, void *factory = NULL) : errors(""), open(true), name(name_in)
                        {
                                plugin = dlopen (file.c_str(), RTLD_NOW);
                                char *tempFile = tempnam(NULL, NULL);
                                system((std::string("nm ") + file + std::string(" | grep \"__gambit_module_moduleInit_\" >& ") + std::string(tempFile)).c_str());
                                std::ifstream in(tempFile);
                                std::string str;
                                while(getline(in, str))
                                {
                                        int pos = str.find("__gambit_module_moduleInit_");
                                        int posLast = str.rfind("__");
                                        mod_names.push_back(str.substr(pos + 27, posLast - pos - 27));
                                }
                                in.close();
                                
                                if (bool(plugin))
                                {
                                        if (mod_names.size() > 0)
                                        {
                                                if (!vector_elem_check(mod_names, name))
                                                {
                                                        
                                                        if (name !=  "")
                                                                std::cout << "\n\e[01;33mWARNING:\e[00m  Module \"" << name << "\" is not in file \"" << file << "\".\n\n";
                                                        else
                                                                std::cout << "\n\e[01;33mWARNING:\e[00m  Module was not specified in ini-file.\n\n";
                                                        
                                                        std::cout << "Modules in \"" << file << "\" are:\n";
                                                        for (int i = 0; i < mod_names.size(); i++)
                                                        {
                                                                std::cout << "   Module " << i << ":  " << mod_names[i] << "\n";
                                                        }
                                                        std::cout << "\nPlease choose a module number:  " << std::flush;
                                                        int iin;
                                                        std::getline(std::cin, str);
                                                        std::istringstream ss(str);
                                                        bool is_not_int = !(ss >> iin);
                                                        while(iin < 0 || iin >= mod_names.size() || is_not_int)
                                                        {
                                                                std::cout << "\033[1APlease choose a module number:  " << bblank << std::flush;
                                                                std::getline(std::cin, str);
                                                                std::stringstream ss(str);
                                                                is_not_int = !(ss >> iin);
                                                        }
                                                        name = mod_names[iin];
                                                }
                                        
                                                inputFunc = (inputFuncType)dlsym(plugin, (std::string("__gambit_module_setValue_") + name + std::string("__")).c_str());
                                                initFunc = (initFuncType)dlsym(plugin, (std::string("__gambit_module_moduleInit_") + name + std::string("__")).c_str());
                                                defFunc = (defFuncType)dlsym(plugin, (std::string("__gambit_module_setDefault_") + name + std::string("__")).c_str());
                                                getFunc = (getFuncType)dlsym(plugin, (std::string("__gambit_module_getMember_") + name + std::string("__")).c_str());
                                                rmFunc = (rmFuncType)dlsym(plugin, (std::string("__gambit_module_rmMember_") + name + std::string("__")).c_str());
                                                
                                                init(input, factory, boundIniFile);
                                        }
                                        else
                                        {
                                                std::stringstream ss;
                                                ss << "\e[00;31mERROR:\e[00m  Could not any modules in file \"" << file << "\".";
                                                errors = ss.str();
                                        }
                                }
                                else
                                {
                                        std::stringstream ss;
                                        ss << "\e[00;31mERROR:\e[00m  Cannot load " << file << ":  " << dlerror();
                                        errors = ss.str();
                                        open = false;
                                }
                        }
                        
                        void init(std::vector<void*> *input, void *factory, const IniParser::IniFile *boundIniFile)
                        {
                                bool good = true;
                                std::vector<std::string> missingParams;
                                std::vector<std::string> iniKeys;
                                initFunc(input, iniKeys, factory);
                                
                                if(boundIniFile != NULL)
                                {
                                        for (std::vector<std::string>::iterator it = iniKeys.begin(); it != iniKeys.end(); it++)
                                        {
                                                if (boundIniFile->hasKey(name, *it)) 
                                                {
                                                        std::string value = boundIniFile->getValue<std::string>(name, *it);
                                                        inputFunc(value, *it);
                                                }
                                                else if (!defFunc(*it))
                                                {
                                                        missingParams.push_back(*it);
                                                        good = false;
                                                }
                                        }
                                }
                                
                                if (good)
                                {
                                        main = (T *)getFunc(name);
                                        
                                        if (main == 0)
                                        {
                                                std::stringstream ss;
                                                ss << "\e[00;31mERROR:\e[00m  Could not find main function in module \"" << name << "\".";
                                                errors = ss.str();
                                        }
                                }
                                else
                                {
                                        std::stringstream ss;
                                        ss << "\e[00;31mERROR:\e[00m  Missing entries needed by scanner module \"" << name << "\":  " << missingParams;
                                        errors = ss.str();
                                }
                        }
                        
                        void *getMember(std::string in){return getFunc(in);}
                        void deleteMember(void *ptr, std::string in){rmFunc(ptr, in);}
                        const std::string printErrors() const {return errors;}
                        
                        ~Module_Interface(){if (open) dlclose(plugin);}
                };
        };
};

#endif
//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  HDF5 printer retriever class declaration
///  This is a class accompanying the HDF5Printer
///  which takes care of *reading* from output
///  created by the  HDF5Printer.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Ben Farmer
///          (benjamin.farmer@monash.edu.au)
///  \date 2017 Jan
///
///  *********************************************

#include "gambit/Printers/baseprinter.hpp"
#include "gambit/Printers/printer_id_tools.hpp"
#include "gambit/Printers/printers/hdf5printer/hdf5tools.hpp"
#include "gambit/Printers/printers/hdf5printer/DataSetInterfaceScalar.hpp"
#include "gambit/Utils/cats.hpp"

#ifndef __hdf5_reader_hpp__
#define __hdf5_reader_hpp__

namespace Gambit {
  namespace Printers {

    /// Length of dataset chunks read into memory during certain search operations.
    /// For maximum efficiency this should probably match the chunking length used
    /// to write the files in the first place.
    static const std::size_t CHUNKLENGTH = 100; 

    template<class T>
    struct BuffPair
    {
       DataSetInterfaceScalar<T,   CHUNKLENGTH> data;
       DataSetInterfaceScalar<int, CHUNKLENGTH> isvalid;
       BuffPair(DataSetInterfaceScalar<T, CHUNKLENGTH>& d,
                DataSetInterfaceScalar<int, CHUNKLENGTH>& v)
         : data(d)
         , isvalid(v)
       {}
       // Handy shortcut constructor
       BuffPair(hid_t location_id, const std::string& name)
         : data   (location_id,name,true,'r')
         , isvalid(location_id,name+"_isvalid",true,'r')
       {}
       // Default constructor, data uninitialised!
       BuffPair() {}
    };

    /// Keeps track of vertex buffers local to a retrieve function 
    /// Similar to the buffer manager for HDF5Printer. I considered
    /// trying to re-use that, but it is too integrated with the
    /// printer.
    template<class T>
    class H5P_LocalReadBufferManager
    {
      private:
        // Buffers local to a print function. Access whichever ones match the IDcode.
        std::map<VBIDpair, BuffPair<T>> local_buffers;

      public:
        /// Constructor
        H5P_LocalReadBufferManager()
        {
        } 

        /// Destructor. Close all datasets
        ~H5P_LocalReadBufferManager()
        {
          for(typename std::map<VBIDpair, BuffPair<T>>::iterator it=local_buffers.begin();
              it!=local_buffers.end(); ++it)
          {
            it->second.data.closeDataSet();
            it->second.isvalid.closeDataSet();
          }
        }

        /// Retrieve a buffer for an IDcode/auxilliary-index pair
        /// location_id used to access dataset if it has not yet been opened.
        BuffPair<T>& get_buffer(const int vID, const unsigned int i, const std::string& label, hid_t location_id);  
    };

    class HDF5Reader : public BaseReader
    {
      public:
        HDF5Reader(const Options& options);
        ~HDF5Reader();

        /// @{ Base class virtual interface functions
        virtual void reset(); // Reset 'read head' position to first entry
        virtual ulong get_dataset_length(); // Get length of input dataset
        virtual PPIDpair get_next_point(); // Get next rank/ptID pair in data file
        virtual PPIDpair get_current_point(); // Get current rank/ptID pair in data file
        virtual bool eoi(); // Check if 'current point' is past the end of the data file (and thus invalid!)
        /// Get type information for a data entry, i.e. defines the C++ type which this should be
        /// retrieved as, not what it is necessarily literally stored as in the output.
        virtual std::size_t get_type(const std::string& label);
        virtual std::set<std::string> get_all_labels(); // Get all dataset labels

        /// Templatable retrieve functions
        bool _retrieve(int      &, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(uint     &, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(long     &, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(ulong    &, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(longlong &, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(ulonglong&, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(float    &, const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(double   &, const std::string& label, const uint rank, const ulong pointID);
        /// 'Custom' retrieve functions
        bool _retrieve(bool&,               const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(std::vector<double>&,const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(map_str_dbl&,        const std::string& label, const uint rank, const ulong pointID);
        bool _retrieve(ModelParameters&,    const std::string& label, const uint rank, const ulong pointID);
        /// @}

      private:
        // Location of HDF5 datasets to be read
        const std::string file;
        const std::string group;
        const hid_t file_id; 
        const hid_t location_id; 

        // Names of all datasets at the target location
        const std::vector<std::string> all_datasets;

        // MPIrank and pointID dataset wrappers
        DataSetInterfaceScalar<unsigned long, CHUNKLENGTH> pointIDs;
        DataSetInterfaceScalar<int, CHUNKLENGTH> pointIDs_isvalid;
        DataSetInterfaceScalar<int, CHUNKLENGTH> mpiranks;
        DataSetInterfaceScalar<int, CHUNKLENGTH> mpiranks_isvalid;

        ulong current_dataset_index; // index in input dataset of the current read-head position
        PPIDpair current_point;      // PPID of the point at the current read-head position

        // PPIDpair and dataset index of the last retrieved data.
        // Just to speed up "random access" retrieval of a lot of data from the same point
        ulong    mem_index;
        PPIDpair mem_point;

        // Search for the PPID supplied in the input data and return the index of the first match
        ulong get_index_from_PPID(const PPIDpair);

        template<class T>
        H5P_LocalReadBufferManager<T>& get_mybuffermanager();

        /// Buffer manager objects
        //  Need one for every directly retrievable type, and a specialisation
        //  of 'get_mybuffermanager' to access it. But the latter have to be
        //  defined outside the class declaration, so they can be found below.
        //  Could create all these with a macro, but I am sick of macros so
        //  will just do it the "old-fashioned" way.
        H5P_LocalReadBufferManager<int      > hdf5_localbufferman_int;
        H5P_LocalReadBufferManager<uint     > hdf5_localbufferman_uint;
        H5P_LocalReadBufferManager<long     > hdf5_localbufferman_long;
        H5P_LocalReadBufferManager<ulong    > hdf5_localbufferman_ulong;
        H5P_LocalReadBufferManager<longlong > hdf5_localbufferman_longlong;
        H5P_LocalReadBufferManager<ulonglong> hdf5_localbufferman_ulonglong;
        H5P_LocalReadBufferManager<float    > hdf5_localbufferman_float;
        H5P_LocalReadBufferManager<double   > hdf5_localbufferman_double;

        /// "Master" templated retrieve function.
        template<class T>
        bool _retrieve_template(T& out, const std::string& label, int aux_id, const uint rank, const ulong pointID)
        {
           // Retrieve the buffer manager for buffers with this type
           auto& buffer_manager = get_mybuffermanager<T>();
 
           // Buffers are labelled by an IDcode, which in the printer case is a graph vertex.
           // In the reader case I think we can safely re-use this system to assign IDs:
           int IDcode = get_param_id(label);

           // Extract a buffer pair from the manager corresponding to this type + label
           auto& selected_buffer = buffer_manager.get_buffer(IDcode, aux_id, label, location_id); 
           
           // Determine the dataset index from which to extrat the input PPIDpair
           ulong dset_index = get_index_from_PPID(PPIDpair(pointID,rank));

           // Extract data value
           out = selected_buffer.data.get_entry(dset_index);

           // Extract data validity flag
           return selected_buffer.isvalid.get_entry(dset_index);
        }
  
    };

    /// Define the buffermanager getter specialisations
    #define DEFINE_BUFFMAN_GETTER(TYPE) \
      template<> \
      inline H5P_LocalReadBufferManager<TYPE>& \
        HDF5Reader::get_mybuffermanager() \
      { \
         return CAT(hdf5_localbufferman_,TYPE); \
      }
    DEFINE_BUFFMAN_GETTER(int      ) 
    DEFINE_BUFFMAN_GETTER(uint     )
    DEFINE_BUFFMAN_GETTER(long     )
    DEFINE_BUFFMAN_GETTER(ulong    )
    DEFINE_BUFFMAN_GETTER(longlong )
    DEFINE_BUFFMAN_GETTER(ulonglong)
    DEFINE_BUFFMAN_GETTER(float    )
    DEFINE_BUFFMAN_GETTER(double   )
    #undef DEFINE_BUFFMAN_GETTER

    // Register reader so it can be constructed via inifile instructions
    // First argument is string label for inifile access, second is class from which to construct printer
    LOAD_READER(hdf5, HDF5Reader)

  }
}

#endif

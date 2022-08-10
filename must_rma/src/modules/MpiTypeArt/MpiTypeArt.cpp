/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MpiTypeArt.cpp
 * 	@see MpiTypeArt.
 *
 * @author Joachim Protze (RWTH Aachen), Alexander Hueck (TU Darmstadt)
 */

#include "GtiMacros.h"
#include "MpiTypeArt.h"
#include "MustEnums.h"
#include "MustDefines.h"

#include <sstream>

// TypeArt runtime interface
#include <RuntimeInterface.h>

using namespace must;

mGET_INSTANCE_FUNCTION(MpiTypeArt)
mFREE_INSTANCE_FUNCTION(MpiTypeArt)
mPNMPI_REGISTRATIONPOINT_FUNCTION(MpiTypeArt)


//=============================
// Constructor
//=============================
MpiTypeArt::MpiTypeArt (const char* instanceName)
    : gti::ModuleBase<MpiTypeArt, I_MpiTypeArt> (instanceName)
{
    // create sub modules
    std::vector<I_Module*> subModInstances;
    subModInstances = createSubModuleInstances ();
    
    // handle sub modules
#define NUM_SUBMODULES 5
    if (subModInstances.size() < NUM_SUBMODULES)
    {
	std::cerr << "Module does not have enough sub modules, check its analysis specification! (" << __FILE__ << "@" << __LINE__ << ")" << std::endl;
	assert (0);
    }
    if (subModInstances.size() > NUM_SUBMODULES)
    {
	for (std::vector<I_Module*>::size_type i = NUM_SUBMODULES; i < subModInstances.size(); ++i)
	{
	    destroySubModuleInstance (subModInstances[i]);
	}
    }
    
    myPIdMod = (I_ParallelIdAnalysis*) subModInstances[0];
    myLIdMod = (I_LocationAnalysis*) subModInstances[1];
    myLogger = (I_CreateMessage*) subModInstances[2];
    myArgMod = (I_ArgumentAnalysis*) subModInstances[3];
    myDatMod = (I_DatatypeTrack*) subModInstances[4];
}

//=============================
// Destructor
//=============================
MpiTypeArt::~MpiTypeArt (void)
{
    if (myPIdMod)
	destroySubModuleInstance ((I_Module*) myPIdMod);
    myPIdMod = NULL;

    if (myLIdMod)
	destroySubModuleInstance ((I_Module*) myLIdMod);
    myLIdMod = NULL;
    
    if (myLogger)
	destroySubModuleInstance ((I_Module*) myLogger);
    myLogger = NULL;
    
    if (myArgMod)
	destroySubModuleInstance ((I_Module*) myArgMod);
    myArgMod = NULL;
    
    if (myDatMod)
	destroySubModuleInstance ((I_Module*) myDatMod);
    myDatMod = NULL;
    
}

int isCompIntSize(MustMpiDatatypePredefined mpi_type, size_t size)
{
    switch (size) {
        case 1:
            return mpi_type == MUST_MPI_INT8_T || mpi_type == MUST_MPI_UINT8_T;
        case 2:
            return mpi_type == MUST_MPI_INT16_T || mpi_type == MUST_MPI_UINT16_T;
        case 4:
            return mpi_type == MUST_MPI_INT32_T || mpi_type == MUST_MPI_UINT32_T;
        case 8:
            return mpi_type == MUST_MPI_INT64_T || mpi_type == MUST_MPI_UINT64_T;
        default: break;
    }
    return 0;
}

int isCompatible(MustMpiDatatypePredefined mpi_type, int typeart_type_id)
{
    if (mpi_type == MUST_MPI_BYTE)
        return 1;
    switch (typeart_type_id) {
        case TYPEART_INT8:
            return mpi_type == MUST_MPI_CHAR || mpi_type == MUST_MPI_UNSIGNED_CHAR || isCompIntSize(mpi_type, 1);
        case TYPEART_INT16:
            return mpi_type == MUST_MPI_SHORT || mpi_type == MUST_MPI_UNSIGNED_SHORT || isCompIntSize(mpi_type, 2);
        case TYPEART_INT32:
            return mpi_type == MUST_MPI_INT || mpi_type == MUST_MPI_UNSIGNED || isCompIntSize(mpi_type, 4);
        case TYPEART_INT64:
            return mpi_type == MUST_MPI_LONG || mpi_type == MUST_MPI_UNSIGNED_LONG || mpi_type == MUST_MPI_LONG_LONG || mpi_type == MUST_MPI_LONG_LONG_INT || mpi_type == MUST_MPI_UNSIGNED_LONG_LONG || isCompIntSize(mpi_type, 8);
/*        case TYPEART_HALF:
            return mpi_type == MUST_MPI_FLOAT;*/
        case TYPEART_FLOAT:
            return mpi_type == MUST_MPI_FLOAT;
        case TYPEART_DOUBLE:
            return mpi_type == MUST_MPI_DOUBLE;
        case TYPEART_FP128:
            return mpi_type == MUST_MPI_LONG_DOUBLE;
        default:
            break;
    }
    return 0;
}

I_Datatype * getBasicType (I_Datatype * info)
{
    while (info->getDatatypeClass() != MUST_TYPE_BASE && info->getDatatypeClass() != MUST_TYPE_STRUCT)
        info = info->getReferencedTypes().front();
    return info;
}

void MpiTypeArt::createMessage(
    MustParallelId pId, 
    MustLocationId lId, 
    const std::string& message,
    MustMessageIdNames type)
{
    if (type < MUST_LAST_ERROR)
        myLogger->createMessage(
            type,
            pId,
            lId,
            MustErrorMessage,
            message
            );
    else if (type < MUST_LAST_WARNING)
        myLogger->createMessage(
            type,
            pId,
            lId,
            MustWarningMessage,
            message
            );
    else 
        myLogger->createMessage(
            type,
            pId,
            lId,
            MustInformationMessage,
            message
            );
}

inline std::string combiner_name_for(const I_Datatype * const must_type_info)
{
  switch (must_type_info->getDatatypeClass()) {
  case MUST_TYPE_BASE:
    return "MUST_TYPE_BASE";
  case MUST_TYPE_CONTIGUOUS:
    return "MUST_type_contiguous";
  case MUST_TYPE_VECTOR:
    return "MUST_type_vector";
  case MUST_TYPE_HVECTOR:
    return "MUST_type_hvector";
  case MUST_TYPE_INDEXED:
    return "MUST_type_indexed";
  case MUST_TYPE_HINDEXED:
    return "MUST_type_hindexed";
  case MUST_TYPE_INDEXED_BLOCK:
    return "MUST_type_indexed_bloc";
  case MUST_TYPE_STRUCT:
    return "MUST_type_struct";
  case MUST_TYPE_RESIZED:
    return "MUST_type_resized";
  case MUST_TYPE_SUBARRAY:
    return "MUST_type_subarray";
  case MUST_TYPE_DARRAY:
    return "MUST_type_darray";
  case MUST_TYPE_UNKNOWN:
    return "MUST_type_unknown";
  }
  return "Unknown_combiner";
}

inline std::string get_mpi_name_for(I_Datatype * const must_type_info)
{
  if (must_type_info->isPredefined()) {
    return must_type_info->getPredefinedName();
  }
  return combiner_name_for(must_type_info);
}

//=============================
// checkSendOrRecv
//=============================
GTI_ANALYSIS_RETURN MpiTypeArt::checkSendOrRecv(MustParallelId pId,
                                                MustLocationId lId,
                                                MustAddressType buffer,
                                                MustDatatypeType datatype,
                                                int count) {
  // Sth. like std::optional:
  using AnalysisResult = struct {
    struct {
      MustMessageIdNames message_id;
      std::string message;
    } analysis;
    bool has_error;
  };

  I_Datatype * const must_type_info = myDatMod->getDatatype(pId, datatype);
  const auto mpi_combiner = must_type_info->getDatatypeClass();
  void *startAddr =
      (void *)((MUST_BOTTOM == buffer) ? must_type_info->getLb()
                                       : (buffer + must_type_info->getLb()));

  if (startAddr == nullptr || count == 0) {
    return GTI_ANALYSIS_SUCCESS;
  }

  const bool simple_array = must_type_info->isSimpleArray();
  if(!simple_array) {
    createMessage(pId, lId,
                  "Not yet implemented: MPI datatype is not an array-like "
                  "type of structs or base-types.",
                  MUST_INFO_UNIMPLEMENTED_FEATURE);
    return GTI_ANALYSIS_SUCCESS;
  }

  switch (mpi_combiner) {
  case MUST_TYPE_SUBARRAY:
  case MUST_TYPE_HINDEXED:
  case MUST_TYPE_HVECTOR:
  case MUST_TYPE_DARRAY:
  case MUST_TYPE_INDEXED:
  case MUST_TYPE_INDEXED_BLOCK:
  {
    std::stringstream stream;
    stream << "Not yet implemented: Type checking combiner "
           << get_mpi_name_for(must_type_info);
    createMessage(pId, lId,
                  stream.str(),
                  MUST_INFO_UNIMPLEMENTED_FEATURE);
    return GTI_ANALYSIS_SUCCESS;
  }
  default: break;
  };

  int typeart_type_info;
  size_t typeart_buffer_count;
  size_t offset = 0;
  const void *base_address;

  typeart_status status = typeart_get_type(startAddr, &typeart_type_info, &typeart_buffer_count);

  if (status != TYPEART_OK) {
    if (status == TYPEART_BAD_ALIGNMENT) {
      createMessage(pId, lId,
                    "Buffer address does not align with the underlying type.",
                    MUST_ERROR_TYPEMATCH_ALIGNMENT);
    } else if (status == TYPEART_UNKNOWN_ADDRESS) {
      createMessage(pId, lId, "No buffer allocated at given address.",
                    MUST_INFO_UNIMPLEMENTED_FEATURE);
    } else if (status == TYPEART_INVALID_ID){
      createMessage(pId, lId, "Buffer has invalid TypeART type id at given address.",
                    MUST_INFO_UNIMPLEMENTED_FEATURE);
    }

    return GTI_ANALYSIS_SUCCESS;
  }

  I_Datatype *baseType = getBasicType(must_type_info);

  const auto check_type_length_error = [&](int typeart_type_info,
                                            size_t mpi_byte_transfer_size,
                                            size_t typeart_byte_buf_count) -> AnalysisResult
  {
    if(mpi_byte_transfer_size > typeart_byte_buf_count) {
      std::stringstream stream;
      const char *typeart_recorded_name =
          typeart_get_type_name(typeart_type_info);
      stream << "Buffer too small: Transfer of type [" << count << "x\""
             << get_mpi_name_for(must_type_info) << "\"] with byte count of "
             << mpi_byte_transfer_size
             << " longer than buffer argument of type [" << typeart_buffer_count
             << "x\"" << typeart_recorded_name << "\"] with byte count of "
             << typeart_byte_buf_count << ".";
      return AnalysisResult{{MUST_ERROR_TYPEMATCH_LENGTH, stream.str()}, true};
    }
    return AnalysisResult{{}, false};
  };

  const auto print_if_typelength_error = [&](int typeart_type_info,
                                                                size_t mpi_byte_transfer_size,
                                                                size_t typeart_byte_buf_count) -> bool
  {
    const auto result = check_type_length_error(typeart_type_info,
                                                mpi_byte_transfer_size,
                                                typeart_byte_buf_count);
    if(result.has_error) {
      createMessage(pId, lId,
                    result.analysis.message,
                    result.analysis.message_id);
      return true;
    }
    return false;
  };

  const auto get_mpi_size_multiplier = [&] ()
  {
    const auto mpi_combiner = must_type_info->getDatatypeClass();
    switch(mpi_combiner){
    case MUST_TYPE_CONTIGUOUS:
      return must_type_info->getSize();
    case MUST_TYPE_VECTOR:
    case MUST_TYPE_SUBARRAY:
      return must_type_info->getTrueExtent();
    default:
      return baseType->getSize();
    }
  };

  if (baseType->isPredefined()) {
    if (baseType->getPredefinedInfo() == MUST_MPI_BYTE) {
      // the only thing we can do is comparing the size
      const auto mpi_byte_transfer_size = static_cast<size_t>(count * get_mpi_size_multiplier());
      const auto typeart_byte_size = typeart_get_type_size(typeart_type_info);
      const auto typeart_byte_buf_count = typeart_buffer_count * typeart_byte_size;

      print_if_typelength_error(typeart_type_info, mpi_byte_transfer_size, typeart_byte_buf_count);

      return GTI_ANALYSIS_SUCCESS;
    }

    // Determine multiplier for LLVM vecN types, introduced by LLVM optimizer:
    const auto typeart_vec_size_multiplier = [&]() -> size_t {
      const bool is_vec_type = typeart_is_vector_type(typeart_type_info);
      if (is_vec_type) {
        typeart_struct_layout struct_layout;
        const auto status =
            typeart_resolve_type_id(typeart_type_info, &struct_layout);
        if (status == TYPEART_OK) {
          return struct_layout.count[0];
        }
      }
      return 1UL;
    }();

    // If the address corresponds to a struct, recurse and fetch the type of the first
    // built-in type member:
    while (typeart_is_struct_type(typeart_type_info)) {
      typeart_struct_layout struct_layout;
      typeart_resolve_type_id(typeart_type_info, &struct_layout);
      typeart_type_info = struct_layout.member_types[0];
    }

    if (isCompatible(baseType->getPredefinedInfo(), typeart_type_info)) {
      const auto mpi_byte_transfer_size = static_cast<size_t>(count * get_mpi_size_multiplier());
      const auto typeart_byte_size =
          typeart_get_type_size(typeart_type_info) * typeart_vec_size_multiplier;
      const auto typeart_byte_buf_count = typeart_buffer_count * typeart_byte_size;

      print_if_typelength_error(typeart_type_info, mpi_byte_transfer_size, typeart_byte_buf_count);
    } else {
      const char *typeart_recorded_name = typeart_get_type_name(typeart_type_info);
      std::stringstream stream;
      stream << "Incompatible buffer of type " << typeart_type_info
             << " ("
             << typeart_recorded_name << ") - expected "
             << must_type_info->getPredefinedName() << " instead";
      createMessage(pId, lId, stream.str(), MUST_ERROR_TYPEMATCH_MISMATCH);
    }

    return GTI_ANALYSIS_SUCCESS;
  }

  if (!typeart_is_struct_type(typeart_type_info)) {
    typeart_get_containing_type(startAddr, &typeart_type_info, &typeart_buffer_count,
                                &base_address, &offset);
  }

  if (!typeart_is_struct_type(typeart_type_info)) {
    createMessage(pId, lId, "Distributed struct not yet implemented.",
                  MUST_INFO_UNIMPLEMENTED_FEATURE);
    return GTI_ANALYSIS_SUCCESS;
  }

  // Starting here, we have a struct to check:
  typeart_struct_layout struct_layout;
  typeart_resolve_type_id(typeart_type_info, &struct_layout);

  const auto is_typemap_compatible = [&]() -> AnalysisResult
  {
    for (const auto& typemap_entry : baseType->getTypemap()) {
      int typemap_type_id = TYPEART_INVALID_ID;
//      const auto status_typemap = typeart_get_type_id(
//          (void *)(buffer + typemap_entry.second), &typemap_type_id);
      const void* base_addr{nullptr};
      size_t subtype_byte_offset{0};
      size_t count_check{0};
      const auto status_typemap = typeart_get_subtype((void*)startAddr,
                                                      typemap_entry.second, &struct_layout,
                                                      &typemap_type_id, &base_addr,
                                                      &subtype_byte_offset, &count_check);

      if (status_typemap == TYPEART_OK && subtype_byte_offset != 0) {
        const char *recorded_name =
            typeart_get_type_name(typeart_type_info);
        std::stringstream stream;
        stream << "Not yet implemented: Resolving nested struct typemap. Detected for "
               << recorded_name << ".";
        return AnalysisResult{
            {MUST_INFO_UNIMPLEMENTED_FEATURE, stream.str()},
            true};
      }

      if (status_typemap != TYPEART_OK) {
        std::stringstream stream;

        switch (status_typemap) {
        case TYPEART_BAD_OFFSET:
          stream << "Bad typemap offset for struct element ";
          break;
        case TYPEART_BAD_ALIGNMENT:
          stream << "Bad offset, points to illegal position for struct element ";
          break;
        default:
          stream << "Incompatible struct element ";
          break;
        };

        const char *typeart_recorded_name =
            typeart_get_type_name(typeart_type_info);
        stream << myDatMod->getPredefinedName(typemap_entry.first)
               << " at offset " << typemap_entry.second
               << " of base type " << get_mpi_name_for(must_type_info) << ". "
               << "Checked against " << typeart_recorded_name << ".";

        return AnalysisResult{
            {MUST_ERROR_TYPEMATCH_ALIGNMENT, stream.str()},
            true};
      }

      const bool is_builtin_type = typeart_is_builtin_type(typemap_type_id);

      if (is_builtin_type
          && !isCompatible(typemap_entry.first, typemap_type_id)) {
        const char *recorded_name =
            typeart_get_type_name(typeart_type_info);
        const char *built_in_type_name =
            typeart_get_type_name(typemap_type_id);

        std::stringstream stream;
        stream << "Incompatible buffer element of type " << typemap_type_id
               << " (" << built_in_type_name << ") of struct "
               << recorded_name << " - expected "
               << myDatMod->getPredefinedName(typemap_entry.first) << " instead";

        return AnalysisResult{
            {MUST_ERROR_TYPEMATCH_MISMATCH,stream.str()},
            true};
      }
    }
    return AnalysisResult{{}, false};
  };

  const auto mpi_byte_transfer_size = static_cast<size_t>(count * get_mpi_size_multiplier());
  const auto typeart_byte_buf_count = typeart_buffer_count * struct_layout.extent;
  const auto type_length_check_result = check_type_length_error(typeart_type_info, mpi_byte_transfer_size,
                                              typeart_byte_buf_count);
  if(type_length_check_result.has_error) {
    std::stringstream stream;
    stream << type_length_check_result.analysis.message;

    const auto typemap_analysis_result = is_typemap_compatible();
    if(typemap_analysis_result.has_error) {
      stream << " Dependent error: "
             << typemap_analysis_result.analysis.message;
    }

    createMessage(pId, lId, stream.str(),
                  type_length_check_result.analysis.message_id);

    return GTI_ANALYSIS_SUCCESS;
  }

  // For now the MPI type and the user struct type need to match exactly
  // w.r.t. extent:
  if (static_cast<size_t>(baseType->getExtent()) != struct_layout.extent) {
    const auto analysis_result = is_typemap_compatible();
    if (!analysis_result.has_error) {
      createMessage(
          pId, lId,
          "Not yet implemented: Extent of struct differs from MPI extent.",
          MUST_INFO_UNIMPLEMENTED_FEATURE);
      return GTI_ANALYSIS_SUCCESS;
    }

    std::stringstream stream;
    stream << "Extent of struct differs from MPI extent. Dependent error: "
           << analysis_result.analysis.message;
    createMessage(pId, lId, stream.str(),
                  analysis_result.analysis.message_id);

    return GTI_ANALYSIS_SUCCESS;
  }

  const auto analysis_result = is_typemap_compatible();
  if (analysis_result.has_error) {
    createMessage(pId, lId, analysis_result.analysis.message,
                  analysis_result.analysis.message_id);
  }

  return GTI_ANALYSIS_SUCCESS;
}

//=============================
// checkSendOrRecvCounts
//=============================
GTI_ANALYSIS_RETURN MpiTypeArt::checkSendOrRecvCounts (
    MustParallelId pId,
    MustLocationId lId,
    MustAddressType buffer,
    const int displs[],
    const int counts[],
    MustDatatypeType datatype,
    int commsize)
{
    GTI_ANALYSIS_RETURN ret;
    for(int i=0; i<commsize; i++)
    {
        ret = checkSendOrRecv(pId, lId, buffer+displs[i], datatype, counts[i]);
        if ( GTI_ANALYSIS_SUCCESS != ret )
            return ret;
    }
    return GTI_ANALYSIS_SUCCESS;
}

//=============================
// checkSendOrRecvTypes
//=============================
GTI_ANALYSIS_RETURN MpiTypeArt::checkSendOrRecvTypes (
    MustParallelId pId,
    MustLocationId lId,
    MustAddressType buffer,
    const int displs[],
    const int counts[],
    const MustDatatypeType datatypes[],
    int commsize)
{
    GTI_ANALYSIS_RETURN ret;
    for(int i=0; i<commsize; i++)
    {
        ret = checkSendOrRecv(pId, lId, buffer+displs[i], datatypes[i], counts[i]);
        if ( GTI_ANALYSIS_SUCCESS != ret )
            return ret;
    }
    return GTI_ANALYSIS_SUCCESS;
}

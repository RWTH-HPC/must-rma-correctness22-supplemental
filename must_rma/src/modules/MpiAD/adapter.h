/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MUST_MpiAD_adapter_H
#define MUST_MpiAD_adapter_H

#include "I_InitLocationId.h"
#include "I_InitParallelId.h"
#include "I_MpiADadapter.h"

#include "MpiADT.h"

namespace must {
/**
 * MpiAD tools interface adapter.
 *
 * This class provides the MUST internal logic for relaying OMPT callbacks into
 * the MUST stack.
 */
class MpiADadapter : public gti::ModuleBase<MpiADadapter, I_MpiADadapter> {
public:
  /**
   * Constructor.
   *
   *
   * @param instanceName name of this module instance
   */
  MpiADadapter(const char *instanceName);

  /**
   * @copydoc I_MpiADIadapter::finish
   */
  void finish();

  /**
   * Get the parallel ID for a specific call.
   *
   *
   * @return The parallel ID for the thread executing the call.
   */
  MustParallelId getParallelId();

  /**
   * Get the location ID for a specific call.
   *
   *
   * @param codeptr_ra The source code location pointer.
   *
   * @return The location ID indicating the origin of the call.
   */
  MustLocationId getLocationId(const void *codeptr_ra);

  /**
   * MUST version of `MPIADT_Allgather_t`.
   */
  typedef int(*callback_Allgather_t)(MustParallelId pId, MustLocationId lId,
                                      const void *sendbuf, int sendcount,
                                      MPI_Datatype sendtype, void *recvbuf,
                                      int recvcount, MPI_Datatype recvtype,
                                      MPI_Comm comm);
  callback_Allgather_t callback_Allgather_pre;
  callback_Allgather_t callback_Allgather_post;
  /**
   * MUST version of `MPIADT_Allreduce_t`.
   */
  typedef int(*callback_Allreduce_t)(MustParallelId pId, MustLocationId lId,
                                      const void *sendbuf, void *recvbuf,
                                      int count, MPI_Datatype datatype,
                                      MPI_Op op, MPI_Comm comm);
  callback_Allreduce_t callback_Allreduce_pre;
  callback_Allreduce_t callback_Allreduce_post;
  /**
   * MUST version of `MPIADT_Alltoall_t`.
   */
  typedef int(*callback_Alltoall_t)(MustParallelId pId, MustLocationId lId,
                                     const void *sendbuf, int sendcount,
                                     MPI_Datatype sendtype, void *recvbuf,
                                     int recvcount, MPI_Datatype recvtype,
                                     MPI_Comm comm);
  callback_Alltoall_t callback_Alltoall_pre;
  callback_Alltoall_t callback_Alltoall_post;
  /**
   * MUST version of `MPIADT_Gather_t`.
   */
  typedef int(*callback_Gather_t)(MustParallelId pId, MustLocationId lId,
                                   const void *sendbuf, int sendcnt,
                                   MPI_Datatype sendtype, void *recvbuf,
                                   int recvcnt, MPI_Datatype recvtype, int root,
                                   MPI_Comm comm);
  callback_Gather_t callback_Gather_pre;
  callback_Gather_t callback_Gather_post;
  /**
   * MUST version of `MPIADT_Irecv_t`.
   */
  typedef int(*callback_Irecv_t)(MustParallelId pId, MustLocationId lId,
                                  void *buf, int count, MPI_Datatype datatype,
                                  int source, int tag, MPI_Comm comm,
                                  MPI_Request *request);
  callback_Irecv_t callback_Irecv_pre;
  callback_Irecv_t callback_Irecv_post;
  /**
   * MUST version of `MPIADT_Isend_t`.
   */
  typedef int(*callback_Isend_t)(MustParallelId pId, MustLocationId lId,
                                  const void *buf, int count,
                                  MPI_Datatype datatype, int dest, int tag,
                                  MPI_Comm comm, MPI_Request *request);
  callback_Isend_t callback_Isend_pre;
  callback_Isend_t callback_Isend_post;
  /**
   * MUST version of `MPIADT_Wait_t`.
   */
  typedef int(*callback_Wait_t)(MustParallelId pId, MustLocationId lId,
                                 MPI_Request *request, MPI_Status *status);
  callback_Wait_t callback_Wait_pre;
  callback_Wait_t callback_Wait_post;
  /**
   * MUST version of `MPIADT_Waitall_t`.
   */
  typedef int(*callback_Waitall_t)(MustParallelId pId, MustLocationId lId,
                                    int count, MPI_Request array_of_requests[],
                                    MPI_Status array_of_statuses[]);
  callback_Waitall_t callback_Waitall_pre;
  callback_Waitall_t callback_Waitall_post;

protected:
  /**
   * I_InitParallelId interface to generated parallel id's when necessary
   */
  I_InitParallelId *parallelInit;

  /**
   * I_InitLocationId interface to generated location id's when necessary
   */
  I_InitLocationId *locationInit;
};
} // namespace must

#endif

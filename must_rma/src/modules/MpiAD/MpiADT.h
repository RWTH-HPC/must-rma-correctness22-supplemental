typedef enum MPIADT_endpoint_t {
  MPIADT_endpoint_begin = 1,
  MPIADT_endpoint_end
} MPIADT_endpoint_t;

typedef enum MPIADT_register_result_t {
  MPIADT_set_error = 0,
  MPIADT_set_never = 1,
  MPIADT_set_impossible = 2,
  MPIADT_set_sometimes = 3,
  MPIADT_set_sometimes_paired = 4,
  MPIADT_set_always = 5
} MPIADT_register_result_t;




typedef void(*MPIADT_Allgather_t)(const void *sendbuf, int sendcount,
                                  MPI_Datatype sendtype, void *recvbuf,
                                  int recvcount, MPI_Datatype recvtype,
                                  MPI_Comm comm, MPIADT_endpoint_t endpoint,
                                  void **tool_data, const void *codeptr);
typedef void(*MPIADT_Allreduce_t)(const void *sendbuf, void *recvbuf, int count,
                                  MPI_Datatype datatype, MPI_Op op,
                                  MPI_Comm comm, MPIADT_endpoint_t endpoint,
                                  void **tool_data, const void *codeptr);
typedef void(*MPIADT_Alltoall_t)(const void *sendbuf, int sendcount,
                                 MPI_Datatype sendtype, void *recvbuf,
                                 int recvcount, MPI_Datatype recvtype,
                                 MPI_Comm comm, MPIADT_endpoint_t endpoint,
                                 void **tool_data, const void *codeptr);
typedef void(*MPIADT_Gather_t)(const void *sendbuf, int sendcnt,
                               MPI_Datatype sendtype, void *recvbuf,
                               int recvcnt, MPI_Datatype recvtype, int root,
                               MPI_Comm comm, MPIADT_endpoint_t endpoint,
                               void **tool_data, const void *codeptr);
typedef void(*MPIADT_Irecv_t)(void *buf, int count, MPI_Datatype datatype,
                              int source, int tag, MPI_Comm comm,
                              MPI_Request *request, MPIADT_endpoint_t endpoint,
                              void **tool_data, const void *codeptr);
typedef void(*MPIADT_Isend_t)(const void *buf, int count, MPI_Datatype datatype,
                              int dest, int tag, MPI_Comm comm,
                              MPI_Request *request, MPIADT_endpoint_t endpoint,
                              void **tool_data, void *codeptr);
typedef void(*MPIADT_Wait_t)(MPI_Request *request, MPI_Status *status,
                             MPIADT_endpoint_t endpoint, void **tool_data,
                             const void *codeptr);
typedef void(*MPIADT_Waitall_t)(int count, MPI_Request array_of_requests[],
                                MPI_Status array_of_statuses[],
                                MPIADT_endpoint_t endpoint, void **tool_data,
                                const void *codeptr);

typedef enum MPIADT_callbacks_t {
  MPIADT_Allgather = 1,
  MPIADT_Allreduce,
  MPIADT_Alltoall,
  MPIADT_Gather,
  MPIADT_Irecv,
  MPIADT_Isend,
  MPIADT_Wait,
  MPIADT_Waitall
} MPIADT_callbacks_t;

typedef void (*MPIADT_callback_t)(void);
typedef int(*MPIADT_register_callback_t)(MPIADT_callbacks_t callbacks,
                                          MPIADT_callback_t callback);

// Function to be implemented publicly by the tool
#ifdef __cplusplus
extern "C" 
#endif
int MPIADT_start_tool(MPIADT_register_callback_t register_fct);

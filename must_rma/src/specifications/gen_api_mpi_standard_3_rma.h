/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 *  @file
 *
 *  MPI-calls of MPI-3 RMA one-sided communication.

 *  @author Tobias Hilbrich, Simon Schwitanski
 *
 * \note For Fortran we need additional information for the arguments.
 * 		 That is necessary for the handle conversion for MPI-Implementations like
 * 		 OpenMPI.
 * 		 We need for all MPI-Handles of pointer type information how the argument
 * 		 is used. That means we have to know whether it is:
 * 			* an out single value
 * 			* an in-out single value
 * 			* an array for input purpoeses
 * 			* an out array for output purposses
 * 			* an in-out array
 * 		For the arrays we additionally need to know their size !
 * 		We have to denote all this in the argument name, so we will append at the
 * 		end of these argument names:
 *          * IGNORE not added to trace records
 * 			* SINGLE_IN for an in single value (this is often used for requests)
 * 			* SINGLE_OUT for an out single value
 * 			* SINGLE_IO for an in-out single value
 * 			* ARRAY_IN_sizeargument an array for input purpoeses
 *          * ARRAY_IN_SIZE_OF_commargument use comm size for size of array, commargument is the comm
 * 			* ARRAY_OUT_sizeargument an out array for output purposses
 * 			* ARRAY_IO_sizeargument an in-out array
 * 		Where "sizeargument" is the fixed numeric size or the argument speci-
 * 		fing the array size.
 * 		Otherwise we would have to create the wrapper manually.
 *
 */

/* Window calls */
int MPI_Win_allocate(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void* baseptr {SINGLE_IN}, MPI_Win* win {SINGLE_OUT});
int MPI_Win_allocate_shared(MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, void* baseptr, MPI_Win* win {SINGLE_OUT});
int MPI_Win_attach(MPI_Win win, void* base {SINGLE_IN}, MPI_Aint size);
MPI_Fint MPI_Win_c2f(MPI_Win win);
int MPI_Win_call_errhandler(MPI_Win win, int errorcode);
int MPI_Win_complete(MPI_Win win);
int MPI_Win_create(void* base {SINGLE_IN}, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win* win {SINGLE_OUT});
int MPI_Win_create_dynamic(MPI_Info info, MPI_Comm comm, MPI_Win* win {SINGLE_OUT});
int MPI_Win_create_errhandler(MPI_Win_errhandler_function* function {SINGLE_IN}, MPI_Errhandler* errhandler {SINGLE_OUT});
int MPI_Win_create_keyval(MPI_Win_copy_attr_function* win_copy_attr_fn {SINGLE_IN}, MPI_Win_delete_attr_function* win_delete_attr_fn {SINGLE_IN}, int* win_keyval {SINGLE_OUT}, void* extra_state {SINGLE_IN});
int MPI_Win_delete_attr(MPI_Win win, int win_keyval);
int MPI_Win_detach(MPI_Win win, const void* base {SINGLE_IN});
MPI_Win MPI_Win_f2c(MPI_Fint win);
int MPI_Win_fence(int assert, MPI_Win win);
int MPI_Win_flush(int rank, MPI_Win win);
int MPI_Win_flush_all(MPI_Win win);
int MPI_Win_flush_local(int rank, MPI_Win win);
int MPI_Win_flush_local_all(MPI_Win win);
int MPI_Win_free(MPI_Win* win {SINGLE_IN});
int MPI_Win_free_keyval(int* win_keyval {SINGLE_IN});
int MPI_Win_get_attr(MPI_Win win, int win_keyval, void* attribute_val {SINGLE_OUT}, int* flag {SINGLE_OUT});
int MPI_Win_get_errhandler(MPI_Win win, MPI_Errhandler* errhandler {SINGLE_OUT});
int MPI_Win_get_group(MPI_Win win, MPI_Group* group {SINGLE_OUT});
int MPI_Win_get_info(MPI_Win win, MPI_Info* info_used {SINGLE_OUT});
int MPI_Win_get_name(MPI_Win win, char* win_name {SINGLE_OUT}, int* resultlen {SINGLE_OUT});
int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
int MPI_Win_lock_all(int assert, MPI_Win win);
int MPI_Win_post(MPI_Group group, int assert, MPI_Win win);
int MPI_Win_set_attr(MPI_Win win, int win_keyval, void* attribute_val {SINGLE_IN});
int MPI_Win_set_errhandler(MPI_Win win, MPI_Errhandler errhandler);
int MPI_Win_set_info(MPI_Win win, MPI_Info info);
int MPI_Win_set_name(MPI_Win win, const char* win_name {SINGLE_IN});
int MPI_Win_shared_query(MPI_Win win, int rank, MPI_Aint* size {SINGLE_OUT}, int* disp_unit {SINGLE_OUT}, void* baseptr {SINGLE_OUT});
int MPI_Win_start(MPI_Group group, int assert, MPI_Win win);
int MPI_Win_sync(MPI_Win win);
int MPI_Win_test(MPI_Win win, int* flag {SINGLE_OUT});
int MPI_Win_unlock(int rank, MPI_Win win);
int MPI_Win_unlock_all(MPI_Win win);
int MPI_Win_wait(MPI_Win win);

/* Communication calls */
int MPI_Get(void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int MPI_Get_accumulate(const void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, void* result_addr {SINGLE_IN}, int result_count, MPI_Datatype result_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win);
int MPI_Rget(void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,MPI_Win win, MPI_Request* request {SINGLE_OUT});
int MPI_Rget_accumulate(const void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, void* result_addr {SINGLE_IN}, int result_count, MPI_Datatype result_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request* request {SINGLE_OUT});
int MPI_Put(const void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win);
int MPI_Rput(const void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win, MPI_Request* request {SINGLE_OUT});
int MPI_Accumulate(const void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win);
int MPI_Raccumulate(const void* origin_addr {SINGLE_IN}, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request* request {SINGLE_OUT});
int MPI_Fetch_and_op(const void* origin_addr {SINGLE_IN}, void* result_addr {SINGLE_IN}, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Op op, MPI_Win win);
int MPI_Compare_and_swap(const void* origin_addr {SINGLE_IN}, const void* compare_addr {SINGLE_IN}, void* result_addr {SINGLE_IN}, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Win win);

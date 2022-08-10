! Part of the MUST Project, under BSD-3-Clause License
! See https://hpc.rwth-aachen.de/must/LICENSE for license information.
! SPDX-License-Identifier: BSD-3-Clause

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C  @file cfortran_F.f          
C  This is a a test for the analysis of must.
C
C
C  Description:
C  A subroutine that performs a MPI_Recv. This Routine is used by cfortran.cpp
C
C  @date 16.08.2011
C  @author Mathias Korepkat
C
C                                                              
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      subroutine MYRECV_ (recv_buf)
          INTEGER recv_buf
          CALL MYRECV (recv_buf)
      end
      subroutine MYRECV__ (recv_buf)
          INTEGER recv_buf
          CALL MYRECV (recv_buf)
      end
      subroutine MYRECV___ (recv_buf)
          INTEGER recv_buf
          CALL MYRECV (recv_buf)
      end
      subroutine MYRECV (recv_buf)
      INCLUDE "mpif.h"

      INTEGER recv_buf
      INTEGER ierror
      INTEGER status(MPI_STATUS_SIZE)

      CALL MPI_RECV(recv_buf, 1, MPI_INTEGER, 0, 42,
     &                    MPI_COMM_WORLD, status, ierror)
      RETURN
      end

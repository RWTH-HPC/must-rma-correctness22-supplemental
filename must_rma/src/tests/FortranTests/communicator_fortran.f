! Part of the MUST Project, under BSD-3-Clause License
! See https://hpc.rwth-aachen.de/must/LICENSE for license information.
! SPDX-License-Identifier: BSD-3-Clause

      PROGRAM communicator
! RUN: %must-run %mpiexec-numproc-flag 2 %must-bin-dir/communicator_fortran \
! RUN: 2>&1 \
! RUN: | %filecheck --implicit-check-not \
! RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

! CHECK: Hello, I am{{ *}}0{{ *}}of{{ *}}2
! CHECK: Signing off, rank {{ *}}0


CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C  @file communicator_fortran.f          
C  This is a a test for the analysis of must.
C
C
C  Description:
C  Creates a cartesian communicator and perform a send and recv call.
C  This test will cause no error.
C
C  @date 08.08.2011
C  @author Mathias Korepkat
C
C                                                              
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      IMPLICIT NONE

      INCLUDE "mpif.h"

      INTEGER ierror, my_rank, size
      INTEGER dims(2),periods(2)
      INTEGER commCart
      INTEGER status(MPI_STATUS_SIZE)
      INTEGER send_buf, recv_buf

      CALL MPI_INIT(ierror)

      CALL MPI_COMM_RANK(MPI_COMM_WORLD, my_rank, ierror)
      CALL MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierror)

C     Say hello
      WRITE(*,*) "Hello, I am ", my_rank, " of ", size
    
C     Check if there are enough tasks
      IF (size .LT. 2) THEN
         WRITE(*,*) "This test needs at least 2 processes!"
      ELSE

C       Create a cartesian communicator
        dims(1) = 1
        dims(2) = 2
        periods(1) = 0
        periods(2) = 1
        CALL MPI_CART_CREATE(MPI_COMM_WORLD, 2, dims,
     &                       periods, 1, commCart,ierror);

C       Send a message to rank 1
        send_buf = 1
        IF (my_rank .EQ. 0) THEN 
            CALL MPI_SEND(send_buf, 1, MPI_INTEGER, 1, 42,
     &                 commCart, status, ierror)
        END IF

C       Recive a message from rank 0
        IF (my_rank .EQ. 1) THEN 
            CALL MPI_RECV(recv_buf, 1, MPI_INTEGER, 0, 42,
     &                 commCart, status, ierror)
        END IF

        CALL MPI_COMM_FREE(commCart,ierror)
      END IF

C     Say bye bye
      WRITE(*,*) "Signing off, rank ", my_rank
      CALL MPI_FINALIZE(ierror)
      END

! Part of the MUST Project, under BSD-3-Clause License
! See https://hpc.rwth-aachen.de/must/LICENSE for license information.
! SPDX-License-Identifier: BSD-3-Clause

      PROGRAM fortranc
! RUN: %must-run %mpiexec-numproc-flag 2 --must:layout \
! RUN: %builddir/tests/FortranTests/fortran_calls_cLayout.xml \
! RUN: %must-bin-dir/fortran_calls_c 2>&1 \
! RUN: | %filecheck --implicit-check-not \
! RUN: '[MUST-REPORT]{{.*(Error|ERROR|Warning|WARNING)}}' %s

! CHECK: Hello, I am{{ *}}0{{ *}}of{{ *}}2
! CHECK: Signing off, rank {{ *}}0


CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C  @file fortranc.f          
C  This is a a test for the analysis of must.
C
C
C  Description:
C  Performs a MPI_Send and calls recv, what is a C function that calls a MPI_Recv.
C
C
C  @date 16.08.2011
C  @author Mathias Korepkat
C
C                                                              
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      IMPLICIT NONE

      INCLUDE "mpif.h"

      INTEGER ierror, my_rank, size
      INTEGER send_buf, recv_buf
      INTEGER status(MPI_STATUS_SIZE)

      CALL MPI_INIT(ierror)

      CALL MPI_COMM_RANK(MPI_COMM_WORLD, my_rank, ierror)
      CALL MPI_COMM_SIZE(MPI_COMM_WORLD, size, ierror)

C     Say hello
      WRITE(*,*) "Hello, I am ", my_rank, " of ", size
    
C     Check if there are enough tasks
      IF (size .LT. 2) THEN
         WRITE(*,*) "This test needs at least 2 processes!"
      ELSE

C     Send a message to rank 1
      send_buf = 1
      IF (my_rank .EQ. 0) THEN 
          CALL MPI_SEND(send_buf, 1, MPI_INTEGER, 1, 42,
     &                  MPI_COMM_WORLD, ierror)
          WRITE(*,*) "Rank 0 sends: ",send_buf
      END IF

C     Recive a message from rank 0
        IF (my_rank .EQ. 1) THEN 
            recv_buf = -1
            CALL myrecv(recv_buf)
            WRITE(*,*) "Rank 1 recives: ",recv_buf
        END IF
      END IF


C     Say bye bye
      WRITE(*,*) "Signing off, rank ", my_rank
      CALL MPI_FINALIZE(ierror)
      END

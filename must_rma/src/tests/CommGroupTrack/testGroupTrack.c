/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

// UNSUPPORTED: fast-tests
// XFAIL: *
// RUN: %must-run %mpiexec-numproc-flag 4 --must:layout \
// RUN: %builddir/tests/CommGroupTrack/GroupLayout.xml --must:analyses \
// RUN: %builddir/tests/CommGroupTrack/analysis_spec.xml \
// RUN: %must-bin-dir/testGroupTrack 2>&1 \
// RUN: | %filecheck %s

// CHECK: [MUST-REPORT]{{.*MPI_GROUP_NULL.*MPI_GROUP_EMPTY size=0.*size=1 table: 0->0.*MPI_Comm_group.*size=4 table: 0->0| 1->1| 2->2| 3->3.*MPI_Comm_group.*size=4 table: 0->3| 1->2| 2->1| 3->0.*MPI_Group_incl.*size=4 table: 0->0| 1->1| 2->2| 3->3.*MPI_Group_incl.*MPI_GROUP_EMPTY size=0.*size=2 table: 0->3| 1->0.*MPI_Group_excl.*size=2 table: 0->0| 1->2.*MPI_Group_range_incl.*size=2 table: 0->3| 1->1.*MPI_Group_range_excl.*size=3 table: 0->3| 1->0| 2->2.*MPI_Group_union.*size=1 table: 0->3.*MPI_Group_intersection.*size=2 table: 0->2| 1->0.*MPI_Group_difference}}

/**
 * @file hello.c
 * A must test for the communicator and group tracking modules.
 * Contains no errors.
 *
 * @author Tobias Hilbrich
 * @data 6.3.2011
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "mustFeaturetested.h"
#include "MustTypes.h"

#define TEST_COMM(C) fComm = MUST_Comm_m2i(C); MPI_Initialized ((int*)&fComm)
#define TEST_GROUP(G) fGroup = MUST_Group_m2i(G); MPI_Initialized ((int*)&fGroup)

/**
 * This test needs exactly 4 processes (to easily validate the results of the performed actions)
 * It uses all group constructors and group operations to check whether they do as
 * they should. Details on the expected mappings are given for each step.
 * Finally, all groups are freed.
 */
int main (int argc, char** argv)
{
	int rank,size, inverseRank;
	MPI_Group 	groupSelf,
						groupWorld,
						inverseGroup,
						inverse2Group,
						emptyGroup,
						exclGroup,
						rangeIncl,
						rangeExcl,
						unionGroup,
						interGroup,
						diffGroup;
	MPI_Comm inverseWorld;
	int *ranksIncl;
	int i;

	MustGroupType fGroup;
	MustCommType fComm;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	printf ("Ready: %d of %d tasks.\n", rank, size);

	//Enough tasks ?
	if (size != 4)
	{
		printf ("This test needs exactly 4 tasks.\n");
		MPI_Finalize ();
		exit (1);
	}

	//==0) Test predefined groups
	// -> NULL
	// -> ()
	TEST_GROUP (MPI_GROUP_NULL);
	TEST_GROUP (MPI_GROUP_EMPTY);

	//==1) Get group of MPI_COMM_SELF
	// -> (0->rank)
	MPI_Comm_group (MPI_COMM_SELF, &groupSelf);
	TEST_GROUP (groupSelf);

	//==2) Get group of MPI_COMM_WORLD
	// -> (0->0, 1->1, 2->2, 3->3)
	MPI_Comm_group (MPI_COMM_WORLD, &groupWorld);
	TEST_GROUP (groupWorld);

	//==3) Reorder the ranks in the group inversly with MPI_Group_incl
	// -> (0->3, 1->2, 2->1, 3->0)
	ranksIncl = (int*) malloc (sizeof(int) * size);
	for (i = 0; i < size; i++)
	{
		ranksIncl[i] = size - i - 1;
	}
	MPI_Group_incl (groupWorld, size, ranksIncl, &inverseGroup);
	TEST_GROUP (inverseGroup);

	//==4) Again do an MPI_Group_incl to inverse the comms in inverseGroup => MPI_COMM_WORLD
	// -> (0->0, 1->1, 2->2, 3->3)
	MPI_Group_incl (inverseGroup, size, ranksIncl, &inverse2Group);
	TEST_GROUP (inverse2Group);

	//==5) Create an empty group with MPI_Group_incl
	// -> ()
	MPI_Group_incl (groupWorld, 0, NULL, &emptyGroup);
	TEST_GROUP (emptyGroup);

	//==6) Use MPI_Group_excl to remove ranks 1 and 2 from the inverse world group
	// -> (0->3, 1->0)
	int exclRanks[2] = {1,2};
	MPI_Group_excl (inverseGroup, 2, exclRanks, &exclGroup);
	TEST_GROUP (exclGroup);

	//==7) Use MPI_Group_range_incl to remove ranks 0 and 2 from the inverse world group while reordering such that rank 3 comes first and rank 1 after wards
	// -> (0->0, 1->2)
	int rangesI[1][3] = {{3,1,-2}};
	MPI_Group_range_incl (inverseGroup, 1, rangesI, &rangeIncl);
	TEST_GROUP (rangeIncl);

	//==8) Use MPI_Group_range_ecl to remove ranks 1 and 3 from the inverse world group
	// -> (0->3, 1->1)
	int rangesE[1][3] = {{1,4,2}};
	MPI_Group_range_excl (inverseGroup, 1, rangesE, &rangeExcl);
	TEST_GROUP (rangeExcl);

	//==9) Create a union of exclGroup (6) and rangeIncl (7)
	// -> (0->3, 1->0) + (0->0, 1->2)
	// -> (0->3, 1->0, 2->2)
	MPI_Group_union (exclGroup, rangeIncl, &unionGroup);
	TEST_GROUP (unionGroup);

	//==10) Create an intersection of unionGroup (9) and rangeExcl (8)
	// -> (0->3, 1->0, 2->2) & (0->3, 1->1)
	// -> (0->3)
	MPI_Group_intersection (unionGroup, rangeExcl, &interGroup);
	TEST_GROUP (interGroup);

	//==11) Create a difference of inverseGroup (3) minus rangeExcl (8)
	// -> (0->3, 1->2, 2->1, 3->0) - (0->3, 1->1)
	// -> (0->2, 1->0)
	MPI_Group_difference (inverseGroup, rangeExcl, &diffGroup);
	TEST_GROUP (diffGroup);

	/*//USED TO DEBUG what the result of a group operation should be
	int outRank;
	MPI_Group_size (diffGroup, &outRank);
	if (rank < outRank)
	{
		MPI_Group_translate_ranks (diffGroup, 1, &rank, groupWorld, &outRank);
		printf ("%d->%d\n", rank, outRank);
	}
	*/

	//==12) Free groups
	MPI_Group_free(&groupSelf);
	MPI_Group_free(&groupWorld);
	MPI_Group_free(&inverseGroup);
	MPI_Group_free(&inverse2Group);
	MPI_Group_free(&exclGroup);
	MPI_Group_free(&rangeIncl);
	MPI_Group_free(&rangeExcl);
	MPI_Group_free(&unionGroup);
	MPI_Group_free(&interGroup);
	MPI_Group_free(&diffGroup);

	printf ("Signing off: %d of %d tasks.\n", rank, size);
	MPI_Finalize ();
	return 0;
}

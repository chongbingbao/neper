/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2016, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#include"ut.h"
#include"neut.h"

extern void ReadNodes (FILE *, struct NODES *, int *);
extern int ReadNodesHead (FILE *);
extern void ReadNodesProp (FILE *, struct NODES *, int *);
extern void ReadNodesFoot (FILE *);
extern void ReadElts (struct MESH *, struct MESH *, struct MESH *,
		      struct MESH *, FILE *);
extern int ReadMeshOfDim (FILE *, struct MESH *, int *, int, int);
extern void ReadEltsFoot (FILE *);
extern void ReadEltsProp (FILE *, struct MESH *, int **, int);
extern void SetElsets (struct MESH *, int *, int **);
extern void SetMeshToNull (struct MESH *);
extern void neut_mesh_fscanf_gmshHead (FILE *, int *);

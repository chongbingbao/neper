/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2016, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include"neut_mesh_fprintf_gmsh_.h"

void
neut_mesh_fprintf_gmsh (FILE * file, char *dim, struct NODES Nodes,
			struct MESH Mesh0D, struct MESH Mesh1D,
			struct MESH Mesh2D, struct MESH Mesh3D,
			struct PART Part, struct MESH MeshCo,
			char *numbering)
{
  neut_meshheader_fprintf_gmsh (file, Mesh0D, Mesh1D, Mesh2D, Mesh3D);

  neut_nodes_fprintf_gmsh (file, Nodes);

  neut_elts_fprintf_gmsh (file, Mesh0D, Mesh1D, Mesh2D, Mesh3D, Part,
			  MeshCo, dim, numbering);

  return;
}

void
neut_meshheader_fprintf_gmsh (FILE * file, struct MESH Mesh0D, struct
			      MESH Mesh1D, struct MESH Mesh2D,
			      struct MESH Mesh3D)
{
  fprintf (file, "$MeshFormat\n");
  fprintf (file, "2 0 8\n");
  if (!Mesh0D.ElsetId && !Mesh1D.ElsetId
      && !Mesh2D.ElsetId && !Mesh3D.ElsetId)
  {
    fprintf (file, "$Comments\n");
    fprintf (file, "contiguous\n");
    fprintf (file, "$EndComments\n");
  }
  fprintf (file, "$EndMeshFormat\n");

  return;
}


void
neut_elts_fprintf_gmsh (FILE * file, struct MESH Mesh0D,
			struct MESH Mesh1D, struct MESH Mesh2D,
			struct MESH Mesh3D,
			struct PART Part, struct MESH MeshCo, char *dim, char *numbering)

{
  int i, elt_type0D, elt_type1D, elt_type2D, elt_type3D, elt_typeC, elset;
  int eltnodeqty1D, eltnodeqty2D, eltnodeqty3D, eltnodeqtyCo;
  int *shift = ut_alloc_1d_int (5);
  int shiftCo = 0;

  eltnodeqty1D =
    neut_elt_nodeqty (Mesh1D.EltType, Mesh1D.Dimension, Mesh1D.EltOrder);
  eltnodeqty2D =
    neut_elt_nodeqty (Mesh2D.EltType, Mesh2D.Dimension, Mesh2D.EltOrder);
  eltnodeqty3D =
    neut_elt_nodeqty (Mesh3D.EltType, Mesh3D.Dimension, Mesh3D.EltOrder);
  eltnodeqtyCo =
    neut_elt_nodeqty (MeshCo.EltType, MeshCo.Dimension, MeshCo.EltOrder);

  if (Mesh0D.EltQty != 0
      && neut_gmsh_elt_nb (Mesh0D.EltType, Mesh0D.Dimension, Mesh0D.EltOrder,
			   &elt_type0D) != 0)
    ut_error_reportbug ();

  if (Mesh1D.EltQty != 0
      && neut_gmsh_elt_nb (Mesh1D.EltType, Mesh1D.Dimension, Mesh1D.EltOrder,
			   &elt_type1D) != 0)
    ut_error_reportbug ();

  if (Mesh2D.EltQty != 0
      && neut_gmsh_elt_nb (Mesh2D.EltType, Mesh2D.Dimension, Mesh2D.EltOrder,
			   &elt_type2D) != 0)
    ut_error_reportbug ();

  if (Mesh3D.EltQty != 0
      && neut_gmsh_elt_nb (Mesh3D.EltType, Mesh3D.Dimension, Mesh3D.EltOrder,
			   &elt_type3D) != 0)
    ut_error_reportbug ();

  if (MeshCo.EltQty != 0
      && neut_gmsh_elt_nb (MeshCo.EltType, MeshCo.Dimension, MeshCo.EltOrder,
			   &elt_typeC) != 0)
    ut_error_reportbug ();

  fprintf (file, "$Elements\n");
  int eltqty = 0;
  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "0"))
  {
    shift[0] = 0;
    eltqty += Mesh0D.EltQty;
  }

  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "1"))
  {
    if (numbering == NULL || !strcmp (numbering, "contiguous"))
      shift[1] = eltqty;
    else if (!strcmp (numbering, "from1"))
      shift[1] = 0;
    else
      ut_error_reportbug ();

    eltqty += Mesh1D.EltQty;
  }

  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "2"))
  {
    if (numbering == NULL || !strcmp (numbering, "contiguous"))
      shift[2] = eltqty;
    else if (!strcmp (numbering, "from1"))
      shift[2] = 0;
    else
      ut_error_reportbug ();

    eltqty += Mesh2D.EltQty;
  }

  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "3"))
  {
    if (numbering == NULL || !strcmp (numbering, "contiguous"))
      shift[3] = eltqty;
    else if (!strcmp (numbering, "from1"))
      shift[3] = 0;
    else
      ut_error_reportbug ();

    eltqty += Mesh3D.EltQty;
  }

  if (MeshCo.EltQty > 0)
  {
    if (numbering == NULL || !strcmp (numbering, "contiguous"))
      shift[4] = eltqty;
    else if (!strcmp (numbering, "from1"))
      shift[4] = 0;
    else
      ut_error_reportbug ();

    eltqty += MeshCo.EltQty;
  }

  fprintf (file, "%d\n", eltqty);

  // 0D mesh
  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "0"))
  {
    if (Mesh0D.EltElset == NULL)
      neut_mesh_init_eltelset (&Mesh0D, NULL);
    for (i = 1; i <= Mesh0D.EltQty; i++)
    {
      elset = (!Mesh0D.ElsetId) ? Mesh0D.EltElset[i]
	: Mesh0D.ElsetId[Mesh0D.EltElset[i]];

      fprintf (file, "%d %d 3 0 %d 0 ", i, elt_type0D, elset);
      ut_array_1d_int_fprintf (file, Mesh0D.EltNodes[i], 1, "%d");
    }
  }

  // 1D mesh
  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "1"))
  {
    if (Mesh1D.EltElset == NULL)
      neut_mesh_init_eltelset (&Mesh1D, NULL);

    for (i = 1; i <= Mesh1D.EltQty; i++)
    {
      elset = (!Mesh1D.ElsetId) ? Mesh1D.EltElset[i]
	: Mesh1D.ElsetId[Mesh1D.EltElset[i]];

      fprintf (file, "%d %d 3 0 %d 0 ", i + shift[1], elt_type1D, elset);
      ut_array_1d_int_fprintf (file, Mesh1D.EltNodes[i], eltnodeqty1D, "%d");
    }
  }

  // 2D mesh
  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "2"))
  {
    if (Mesh2D.EltElset == NULL)
      neut_mesh_init_eltelset (&Mesh2D, NULL);

    for (i = 1; i <= Mesh2D.EltQty; i++)
    {
      elset = (!Mesh2D.ElsetId) ? Mesh2D.EltElset[i]
	: Mesh2D.ElsetId[Mesh2D.EltElset[i]];

      fprintf (file, "%d %d 3 0 %d 0 ", i + shift[2], elt_type2D, elset);
      ut_array_1d_int_fprintf (file, Mesh2D.EltNodes[i], eltnodeqty2D, "%d");
    }
  }

  // 3D mesh
  if (ut_string_inlist (dim, NEUT_SEP_NODEP, "3"))
  {

    if (Part.qty > 0)
      for (i = 1; i <= Mesh3D.EltQty; i++)
      {
	elset = (!Mesh3D.ElsetId) ? Mesh3D.EltElset[i]
	  : Mesh3D.ElsetId[Mesh3D.EltElset[i]];

	fprintf (file, "%d %d 3 0 %d %d ", i + shift[3], elt_type3D,
		 elset, Part.elt_parts[i] + 1);
	ut_array_1d_int_fprintf (file, Mesh3D.EltNodes[i], eltnodeqty3D,
				 "%d");
      }
    else
      for (i = 1; i <= Mesh3D.EltQty; i++)
      {
	elset = (!Mesh3D.ElsetId) ? Mesh3D.EltElset[i]
	  : Mesh3D.ElsetId[Mesh3D.EltElset[i]];

	fprintf (file, "%d %d 3 0 %d 0 ", i + shift[3], elt_type3D, elset);
	ut_array_1d_int_fprintf (file, Mesh3D.EltNodes[i], eltnodeqty3D,
				 "%d");
      }
  }

  // cohesive element mesh
  if (MeshCo.EltQty > 0)
  {
    if (MeshCo.Dimension == 2)
      shiftCo = Mesh2D.ElsetQty;
    else if (MeshCo.Dimension == 3)
      shiftCo = Mesh3D.ElsetQty;

    for (i = 1; i <= MeshCo.EltQty; i++)
    {
      elset = (!MeshCo.ElsetId) ? MeshCo.EltElset[i]
	: MeshCo.ElsetId[MeshCo.EltElset[i]];
      elset += shiftCo;

      fprintf (file, "%d %d 3 0 %d 0 ", i + shift[4], elt_typeC, elset);
      ut_array_1d_int_fprintf (file, MeshCo.EltNodes[i], eltnodeqtyCo,
			       "%d");
    }
  }

  fprintf (file, "$EndElements\n");

  ut_free_1d_int (shift);

  return;
}

void
neut_nodes_fprintf_gmsh (FILE * file, struct NODES Nodes)
{
  int i, j;

  fprintf (file, "$Nodes\n");
  fprintf (file, "%d\n", Nodes.NodeQty);
  for (i = 1; i <= Nodes.NodeQty; i++)
  {
    fprintf (file, "%d", i);
    for (j = 0; j < 3; j++)
      fprintf (file, " %.12f",
	       (fabs (Nodes.NodeCoo[i][j]) <
		1e-12) ? 0 : Nodes.NodeCoo[i][j]);
    fprintf (file, "\n");
  }
  fprintf (file, "$EndNodes\n");

  return;
}

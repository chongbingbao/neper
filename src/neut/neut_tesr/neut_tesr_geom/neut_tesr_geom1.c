/* This file is part of the Neper software package. */
/* Cgeomyright (C) 2003-2016, Romain Quey. */
/* See the COPYING file in the tgeom-level directory. */

#include "neut_tesr_geom_.h"

int
neut_tesr_point_pos (struct TESR Tesr, double *coo, int *rptpos)
{
  int i;

  for (i = 0; i < Tesr.Dim; i++)
  {
    if (coo[i] < 0 || coo[i] > Tesr.size[i] * Tesr.vsize[i])
      return -1;

    rptpos[i] = ceil (coo[i] / Tesr.vsize[i] + 1e-6);
    rptpos[i] = ut_num_min_int (rptpos[i], Tesr.size[i]);
  }
  for (i = Tesr.Dim; i < 3; i++)
    rptpos[i] = 1;

  return 0;
}

int
neut_tesr_coo (struct TESR Tesr, int *rptpos, double *coo)
{
  int i;

  for (i = 0; i < Tesr.Dim; i++)
  {
    if (rptpos[i] < 1 || rptpos[i] > Tesr.size[i])
      return -1;

    coo[i] = (rptpos[i] - 0.5) * Tesr.vsize[i];
  }

  return 0;
}

int
neut_tesr_cell_voxqty (struct TESR Tesr, int cell)
{
  int i, j, k, vqty;

  vqty = 0;
  for (k = Tesr.CellBBox[cell][2][0]; k <= Tesr.CellBBox[cell][2][1]; k++)
    for (j = Tesr.CellBBox[cell][1][0]; j <= Tesr.CellBBox[cell][1][1]; j++)
      for (i = Tesr.CellBBox[cell][0][0]; i <= Tesr.CellBBox[cell][0][1]; i++)
	if (Tesr.VoxCell[i][j][k] == cell)
	  vqty++;

  return vqty;
}

int
neut_tesr_voxqty (struct TESR Tesr)
{
  int i, j, k, vqty;

  vqty = 0;
  for (k = 1; k <= Tesr.size[2]; k++)
    for (j = 1; j <= Tesr.size[1]; j++)
      for (i = 1; i <= Tesr.size[0]; i++)
	if (Tesr.VoxCell[i][j][k] != 0)
	  vqty++;

  return vqty;
}

int
neut_tesr_cell_volume (struct TESR Tesr, int cell, double *pvol)
{
  int vqty;
  double voxvol;

  if (Tesr.CellVol)
    (*pvol) = Tesr.CellVol[cell];

  else
  {
    vqty = neut_tesr_cell_voxqty (Tesr, cell);
    neut_tesr_voxsize (Tesr, &voxvol);
    (*pvol) = voxvol * vqty;
  }

  return (*pvol > 0) ? 0 : -1;
}

int
neut_tesr_cell_area (struct TESR Tesr, int cell, double *parea)
{
  int i, j;

  if (Tesr.Dim != 2)
    abort ();

  if (Tesr.CellVol)
    (*parea) = Tesr.CellVol[cell];
  else
  {
    (*parea) = 0;
    for (j = 1; j <= Tesr.size[1]; j++)
      for (i = 1; i <= Tesr.size[0]; i++)
	if (Tesr.VoxCell[i][j][1] == cell)
	  (*parea)++;

    (*parea) *= (Tesr.vsize[0] * Tesr.vsize[1]);
  }

  return 0;
}

int
neut_tesr_cell_size (struct TESR Tesr, int cell, double *psize)
{
  if (Tesr.Dim == 2)
    neut_tesr_cell_area (Tesr, cell, psize);
  else if (Tesr.Dim == 3)
    neut_tesr_cell_volume (Tesr, cell, psize);
  else
    abort ();

  return 0;
}

int
neut_tesr_cell_centre (struct TESR Tesr, int cell, double *coo)
{
  int i, j, k, qty;
  int *rptpos = NULL;
  double *rptcoo = NULL;

  if (Tesr.CellCoo)
  {
    ut_array_1d_memcpy (coo, 3, Tesr.CellCoo[cell]);
    return 0;
  }

  rptpos = ut_alloc_1d_int (3);
  rptcoo = ut_alloc_1d (3);

  qty = 0;
  ut_array_1d_zero (coo, 3);
  for (k = Tesr.CellBBox[cell][2][0]; k <= Tesr.CellBBox[cell][2][1]; k++)
    for (j = Tesr.CellBBox[cell][1][0]; j <= Tesr.CellBBox[cell][1][1]; j++)
      for (i = Tesr.CellBBox[cell][0][0]; i <= Tesr.CellBBox[cell][0][1]; i++)
	if (Tesr.VoxCell[i][j][k] == cell)
	{
	  ut_array_1d_int_set_3 (rptpos, i, j, k);
	  neut_tesr_coo (Tesr, rptpos, rptcoo);
	  ut_array_1d_add (coo, rptcoo, 3, coo);
	  qty++;
	}

  if (qty > 0)
    ut_array_1d_scale (coo, 3, 1. / qty);

  ut_free_1d_int (rptpos);
  ut_free_1d (rptcoo);

  return (qty > 0) ? 0 : -1;
}

void
neut_tesr_centre (struct TESR Tesr, double *coo)
{
  int i;

  for (i = 0; i < 3; i++)
    coo[i] = 0.5 * Tesr.vsize[i] * Tesr.size[i];

  return;
}

void
neut_tesr_cell_diameq (struct TESR Tesr, int cell, double *pval)
{
  if (Tesr.Dim == 3)
  {
    if (Tesr.CellVol)
      (*pval) = Tesr.CellVol[cell];
    else
      neut_tesr_cell_volume (Tesr, cell, pval);
    (*pval) = pow ((6 / M_PI) * (*pval), 0.3333333333333333333333333);
  }
  else if (Tesr.Dim == 2)
  {
    if (Tesr.CellVol)
      (*pval) = Tesr.CellVol[cell];
    else
      neut_tesr_cell_area (Tesr, cell, pval);
    (*pval) = sqrt ((4 / M_PI) * (*pval));
  }

  else
    abort ();

  return;
}

void
neut_tesr_cell_radeq (struct TESR Tesr, int cell, double *pval)
{
  neut_tesr_cell_diameq (Tesr, cell, pval);
  (*pval) *= 0.5;

  return;
}

void
neut_tesr_cell_points (struct TESR Tesr, int cell, int ***ppts, int *pptqty)
{
  int i, j, k;

  if (!Tesr.CellBBox)
    ut_error_reportbug ();

  (*pptqty) = 0;
  for (k = Tesr.CellBBox[cell][2][0]; k <= Tesr.CellBBox[cell][2][1]; k++)
    for (j = Tesr.CellBBox[cell][1][0]; j <= Tesr.CellBBox[cell][1][1]; j++)
      for (i = Tesr.CellBBox[cell][0][0]; i <= Tesr.CellBBox[cell][0][1]; i++)
	if (Tesr.VoxCell[i][j][k] == cell)
	{
	  (*pptqty)++;
	  (*ppts) = ut_realloc_2d_int_addline (*ppts, *pptqty, 3);
	  ut_array_1d_int_set_3 ((*ppts)[(*pptqty) - 1], i, j, k);
	}

  return;
}

void
neut_tesr_cell_boundpoints (struct TESR Tesr, int cell, int ***ppts,
			    int *pptqty, int connec)
{
  int i, j, k;

  if (!Tesr.CellBBox)
    ut_error_reportbug ();

  (*pptqty) = 0;
  for (k = Tesr.CellBBox[cell][2][0]; k <= Tesr.CellBBox[cell][2][1]; k++)
    for (j = Tesr.CellBBox[cell][1][0]; j <= Tesr.CellBBox[cell][1][1]; j++)
      for (i = Tesr.CellBBox[cell][0][0]; i <= Tesr.CellBBox[cell][0][1]; i++)
	if (Tesr.VoxCell[i][j][k] == cell)
	{
	  if (neut_tesr_cell_boundpoints_test(Tesr, cell, i, j, k, connec))
    {
  	  (*pptqty)++;
  	  (*ppts) = ut_realloc_2d_int_addline (*ppts, *pptqty, 3);
  	  ut_array_1d_int_set_3 ((*ppts)[(*pptqty) - 1], i, j, k);
    }
	}

  return;
}

int
neut_tesr_cell_boundpoints_test(struct TESR Tesr, int cell, int i, int j, int k, int connec)
{
  int res = -1;

  if (connec < 0 || connec > 2)
    ut_print_message(2, 0, "connectivity = %d!\n", connec);

  if (     Tesr.VoxCell[i - 1][j][k] == cell
        && Tesr.VoxCell[i + 1][j][k] == cell
        && Tesr.VoxCell[i][j - 1][k] == cell
        && Tesr.VoxCell[i][j + 1][k] == cell
        && (Tesr.Dim == 2 ||
           (Tesr.VoxCell[i][j][k - 1] == cell
        &&  Tesr.VoxCell[i][j][k + 1] == cell)))
    res = 0;

  if (res == 0 && connec > 0)
  {
    if (     Tesr.VoxCell[i + 1][j - 1][k] == cell
          && Tesr.VoxCell[i + 1][j + 1][k] == cell
          && Tesr.VoxCell[i - 1][j - 1][k] == cell
          && Tesr.VoxCell[i - 1][j + 1][k] == cell
          && (Tesr.Dim == 2 ||
             (Tesr.VoxCell[i + 1][j][k - 1] == cell
          &&  Tesr.VoxCell[i + 1][j][k + 1] == cell
          &&  Tesr.VoxCell[i - 1][j][k - 1] == cell
          &&  Tesr.VoxCell[i - 1][j][k + 1] == cell
          &&  Tesr.VoxCell[i][j + 1][k - 1] == cell
          &&  Tesr.VoxCell[i][j + 1][k + 1] == cell
          &&  Tesr.VoxCell[i][j - 1][k - 1] == cell
          &&  Tesr.VoxCell[i][j - 1][k + 1] == cell)))
      res = 1;
  }

  if (res == 1 && connec > 1)
  {
    if  (Tesr.Dim == 2 ||
           (Tesr.VoxCell[i-1][j-1][k - 1] == cell
        &&  Tesr.VoxCell[i-1][j-1][k + 1] == cell
        &&  Tesr.VoxCell[i-1][j+1][k - 1] == cell
        &&  Tesr.VoxCell[i-1][j+1][k + 1] == cell
        &&  Tesr.VoxCell[i+1][j-1][k - 1] == cell
        &&  Tesr.VoxCell[i+1][j-1][k + 1] == cell
        &&  Tesr.VoxCell[i+1][j+1][k - 1] == cell
        &&  Tesr.VoxCell[i+1][j+1][k + 1] == cell))
        res = 2;
  }

  return res != connec;
}

void
neut_tesr_cell_cornerpoints (struct TESR Tesr, int cell, int ***ppts,
			     int *pptqty)
{
  int i, j, k;

  if (!Tesr.CellBBox)
    ut_error_reportbug ();

  (*pptqty) = 0;
  for (k = Tesr.CellBBox[cell][2][0]; k <= Tesr.CellBBox[cell][2][1]; k++)
    for (j = Tesr.CellBBox[cell][1][0]; j <= Tesr.CellBBox[cell][1][1]; j++)
      for (i = Tesr.CellBBox[cell][0][0]; i <= Tesr.CellBBox[cell][0][1]; i++)
	if (Tesr.VoxCell[i][j][k] == cell)
	{
	  // neighbours along x
	  if (Tesr.VoxCell[i - 1][j][k] == cell
	      && Tesr.VoxCell[i + 1][j][k] == cell)
	    continue;
	  // neighbours along y
	  if (Tesr.VoxCell[i][j - 1][k] == cell
	      && Tesr.VoxCell[i][j + 1][k] == cell)
	    continue;
	  // neighbours along z
	  if (Tesr.VoxCell[i][j][k - 1] == cell
	      && Tesr.VoxCell[i][j][k + 1] == cell)
	    continue;
	  // neighbours along x y
	  if (Tesr.VoxCell[i - 1][j - 1][k] == cell
	      && Tesr.VoxCell[i + 1][j + 1][k] == cell)
	    continue;
	  // neighbours along x -y
	  if (Tesr.VoxCell[i - 1][j + 1][k] == cell
	      && Tesr.VoxCell[i + 1][j - 1][k] == cell)
	    continue;
	  // neighbours along y z
	  if (Tesr.VoxCell[i][j - 1][k - 1] == cell
	      && Tesr.VoxCell[i][j + 1][k + 1] == cell)
	    continue;
	  // neighbours along y -z
	  if (Tesr.VoxCell[i][j - 1][k + 1] == cell
	      && Tesr.VoxCell[i][j + 1][k - 1] == cell)
	    continue;
	  // neighbours along z x
	  if (Tesr.VoxCell[i - 1][j][k - 1] == cell
	      && Tesr.VoxCell[i + 1][j][k + 1] == cell)
	    continue;
	  // neighbours along z -x
	  if (Tesr.VoxCell[i + 1][j][k - 1] == cell
	      && Tesr.VoxCell[i - 1][j][k + 1] == cell)
	    continue;

	  (*pptqty)++;
	  (*ppts) = ut_realloc_2d_int_addline (*ppts, *pptqty, 3);
	  ut_array_1d_int_set_3 ((*ppts)[(*pptqty) - 1], i, j, k);
	}

  return;
}

void
neut_tesr_cell_convexity (struct TESR Tesr, int cell, double *pval)
{
  int i, j, k, qty, **pts = NULL;
  double voxvol, dist, vol, vol2, **coos = NULL;
  struct NODES N;
  struct MESH M;
  int *pt = NULL;
  double *coo = NULL;
  double eps = 1e-4 * ut_array_1d_min (Tesr.vsize, Tesr.Dim);
  double eps3 = 1e-2 * ut_array_1d_gmean (Tesr.vsize, Tesr.Dim);
  srand48 (1);

  if (Tesr.CellConvexity)
  {
    (*pval) = Tesr.CellConvexity[cell];
    return;
  }

  pt = ut_alloc_1d_int (3);
  coo = ut_alloc_1d (3);

  neut_nodes_set_zero (&N);
  neut_mesh_set_zero (&M);

  neut_tesr_cell_cornerpoints (Tesr, cell, &pts, &qty);

  coos = ut_alloc_2d (qty, 3);
  for (i = 0; i < qty; i++)
  {
    neut_tesr_coo (Tesr, pts[i], coos[i]);
    for (j = 0; j < Tesr.Dim; j++)
      coos[i][j] += eps * drand48 ();
  }

  net_pts_convexhull (coos, qty, 3, &N, &M);
  vol = 0;
  for (k = Tesr.CellBBox[cell][2][0]; k <= Tesr.CellBBox[cell][2][1]; k++)
    for (j = Tesr.CellBBox[cell][1][0]; j <= Tesr.CellBBox[cell][1][1]; j++)
      for (i = Tesr.CellBBox[cell][0][0]; i <= Tesr.CellBBox[cell][0][1]; i++)
      {
	if (Tesr.VoxCell[i][j][k] == cell)
	  vol++;
	else
	{
	  ut_array_1d_int_set_3 (pt, i, j, k);
	  neut_tesr_coo (Tesr, pt, coo);
	  ut_space_polypts_point_dist (coos, qty, coo, &dist);
	  if (dist < eps3)
	    vol++;
	}
      }
  neut_tesr_voxsize (Tesr, &voxvol);
  vol *= voxvol;
  neut_tesr_cell_volume (Tesr, cell, &vol2);

  (*pval) = vol2 / vol;
  if (*pval > 1 + 1e-9 || *pval < 0)
    ut_error_reportbug ();

  ut_free_2d_int (pts, qty);
  ut_free_2d (coos, qty);
  ut_free_1d (coo);
  ut_free_1d_int (pt);

  neut_nodes_free (&N);
  neut_mesh_free (&M);

  return;
}

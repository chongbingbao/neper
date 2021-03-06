/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2016, Romain Quey. */
/* See the COPYING file in the top-level directory. */

#include"net_domain_.h"

void
net_domain_cylinder_planes (double h, double Rad, int qty, double **eq)
{
  int i;
  double *n = ut_alloc_1d (3);
  double *r = ol_r_alloc ();
  double theta;
  double **g = ol_g_alloc ();

  eq[0][1] = 0;
  eq[0][2] = 0;
  eq[0][3] = -1;
  eq[0][0] = 0;

  eq[1][1] = 0;
  eq[1][2] = 0;
  eq[1][3] = 1;
  eq[1][0] = h;

  Rad *= cos (M_PI / qty);

  n[0] = -1;
  ol_r_set_this (r, 0, 0, -1);

  for (i = 0; i < qty; i++)
  {
    theta = 360 * ((double) i) / qty;
    ol_rtheta_g (r, theta, g);
    ol_g_vect_vect (g, n, eq[i + 2] + 1);
    eq[i + 2][0] =
      Rad + (eq[i + 2][1] * Rad + eq[i + 2][2] * Rad + eq[i + 2][3] * Rad);
  }

  ol_r_free (n);
  ol_r_free (r);
  ol_g_free (g);

  return;
}

void
net_domain_tesspoly_planes (struct TESS Tess, int id, int *pqty, double **eq)
{
  int i, face;

  (*pqty) = Tess.PolyFaceQty[id];
  for (i = 1; i <= Tess.PolyFaceQty[id]; i++)
  {
    face = Tess.PolyFaceNb[id][i];
    ut_array_1d_memcpy (eq[i - 1], 4, Tess.FaceEq[face]);
    if (Tess.PolyFaceOri[id][i] == -1)
      ut_array_1d_scale (eq[i - 1], 4, -1);
  }

  return;
}

void
net_domain_clip (struct POLY *pDomain, double **eq, int qty)
{
  int i, j;
  double **cubesize = ut_alloc_2d (3, 2);
  struct POLYMOD Polymod;

  cubesize[0][0] = -1e6;
  cubesize[0][1] = 1e6;
  cubesize[1][0] = -1e6;
  cubesize[1][1] = 1e6;
  cubesize[2][0] = -1e6;
  cubesize[2][1] = 1e6;

  net_domain_cube (cubesize, pDomain);

  for (i = 1; i <= 6; i++)
    (*pDomain).FacePoly[i] = -qty - 1;

  neut_polymod_set_zero (&Polymod);

  neut_poly_polymod ((*pDomain), &Polymod);

  int *BadVer = NULL;

  for (i = 0; i < qty; i++)
  {
    BadVer = ut_alloc_1d_int (Polymod.VerQty + 1);

    for (j = 1; j <= Polymod.VerQty; j++)
      if (Polymod.VerUse[j] == 1)
	if (ut_space_planeside (eq[i], Polymod.VerCoo[j] - 1) == 1)
	  BadVer[++BadVer[0]] = j;

    if (BadVer[0] != 0)
      PolyhedronModification (-(i + 1), eq[i], &Polymod, BadVer);

    ut_free_1d_int (BadVer);
  }

  // neut_debug_polymod (stdout, Polymod);

  neut_poly_free (pDomain);

  // neut_debug_polymod (stdout, Polymod);
  net_polymod_poly (Polymod, pDomain);
  // neut_debug_poly (stdout, *pDomain);

  neut_polymod_free (&Polymod);

  ut_free_2d (cubesize, 3);

  return;
}

void
net_domain_sphere_planes (double rad, int qty, double **eq)
{
  int i, j;
  double **pts = ut_alloc_2d (qty, 3);

  ut_space_sphere_points (qty, 1, pts);

  for (i = 0; i < qty; i++)
  {
    ut_array_1d_memcpy (eq[i] + 1, 3, pts[i]);
    eq[i][0] = rad;
    for (j = 0; j < 3; j++)
      eq[i][0] += rad * eq[i][j + 1];
  }

  ut_free_2d (pts, qty);

  return;
}

void
net_domain_transform (struct TESS *pDomain, char* string)
{
  int dir, status;
  double *v = ol_r_alloc ();
  double theta;
  double **g = ol_g_alloc ();
  double *eq = ut_alloc_1d (4);
  double *coo = ut_alloc_1d (3);

  if (!strncmp (string, "rotate(", 7))
  {
    status = sscanf (string, "rotate(%lf,%lf,%lf,%lf)", v, v + 1, v + 2, &theta);
    if (status != 4)
      abort ();
    ol_r_set_unit (v);
    ol_rtheta_g (v, theta, g);
    neut_tess_rotate (pDomain, g);
  }

  else if (!strncmp (string, "translate(", 10))
  {
    status = sscanf (string, "translate(%lf,%lf,%lf)", v, v + 1, v + 2);
    if (status != 3)
      abort ();
    neut_tess_shift (pDomain, v[0], v[1], v[2]);
  }

  else if (!strncmp (string, "scale(", 6))
  {
    status = sscanf (string, "scale(%lf,%lf,%lf)", v, v + 1, v + 2);
    if (status != 3)
      abort ();
    neut_tess_scale (pDomain, v[0], v[1], v[2]);
  }

  else if (!strncmp (string, "split(", 6))
  {
    double **bbox = ut_alloc_2d (3, 2);
    struct SEEDSET SSet;
    neut_seedset_set_zero (&SSet);
    SSet.Type = ut_alloc_1d_char (1);
    neut_seedset_addseed (&SSet, coo, 0);
    neut_tess_bbox (*pDomain, bbox);
    dir = string[6] - 'x';
    if (dir < 0 || dir > 2)
      ut_print_message (2, 3, "Cannot read `%s'.\n", string);

    eq[0] = - 0.5 * (bbox[dir][1] + bbox[dir][0]);
    eq[dir + 1] = -1;
    net_tess_clip (SSet, pDomain, eq);
    neut_seedset_free (&SSet);
  }

  else
    abort ();

  ol_r_free (v);
  ol_g_free (g);
  ut_free_1d (eq);
  ut_free_1d (coo);

  return;
}

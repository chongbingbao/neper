/* This file is part of the Neper software package. */
/* Copyright (C) 2003-2016, Romain Quey. */
/* See the CGEOMYING file in the top-level directory. */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef  NEUT_VOX_GEOM_H
#define  NEUT_VOX_GEOM_H

/// \brief Get the rpoint a point belongs to.
/// \return 0 on success, -1 on failure.
///
///
  extern int neut_tesr_point_pos (struct TESR Tesr, double *coo, int *rptpos);

/// \brief Get the coordinate of a rpoint.
/// \return 0 on success, -1 on failure.
///
///
  extern int neut_tesr_coo (struct TESR Tesr, int *rptpos, double *coo);

/// \brief Get the centre of a cell of a TESR structure.
/// \return 0 on success, -1 on failure.
///
///
  extern int neut_tesr_cell_centre (struct TESR Tesr, int cell, double *coo);

/// \brief Get the number of voxels in a cell of a TESR structure.
/// \return 0 on success, -1 on failure.
///
///
  extern int neut_tesr_cell_voxqty (struct TESR Tesr, int cell);

  extern int neut_tesr_voxqty (struct TESR Tesr);

/// \brief Get the volume of a cell of a TESR structure.
/// \return 0 on success, -1 on failure.
///
///
  extern int neut_tesr_cell_volume (struct TESR Tesr, int cell, double *vol);
  extern int neut_tesr_cell_area (struct TESR Tesr, int cell, double *area);
  extern int neut_tesr_cell_size (struct TESR Tesr, int cell, double *size);

  extern void neut_tesr_centre (struct TESR Tesr, double *coo);

  extern void neut_tesr_cell_diameq (struct TESR Tesr, int cell,
				     double *pval);
  extern void neut_tesr_cell_radeq (struct TESR Tesr, int cell, double *pval);

  extern void neut_tesr_cell_points (struct TESR Tesr, int cell,
				     int ***pts, int *pptqty);

  extern void neut_tesr_cell_boundpoints (struct TESR Tesr, int cell,
					  int ***pts, int *pptqty, int connectivity);

  extern int neut_tesr_cell_boundpoints_test(struct TESR Tesr, int cell,
         int i, int j, int k, int connectivity);

  extern void neut_tesr_cell_cornerpoints (struct TESR Tesr, int cell,
					   int ***pts, int *pptqty);

  extern void neut_tesr_cell_convexity (struct TESR Tesr, int cell,
					double *pval);

#include"net_utils/net_utils.h"

#endif				/* NEUT_VOX_GEOM_H */

#ifdef __cplusplus
}
#endif

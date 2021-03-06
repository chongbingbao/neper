# This file is part of the Neper software package.
# Copyright (C) 2003-2016, Romain Quey.
# See the COPYING file in the top-level directory.

cmake_minimum_required(VERSION 2.8)
project(neper)

set (NEPER_VERSION \"3.0.1-5\")
set (NEPER_EMAIL \"neper-users@lists.sourceforge.net\")

# should we use the optional libraries?
option (HAVE_GSL           "Use the GNU Scientific Library (GSL)"        ON )
option (HAVE_LIBMATHEVAL   "Use libmatheval"                             ON )
option (HAVE_NLOPT         "Use NLopt"                                   ON )
option (HAVE_LIBSCOTCH     "Use libscotch"                               OFF)
option (HAVE_PTHREAD       "Use pthread"                                 OFF)
option (DEVEL_OPTIMIZATION "Enable optimization (ON for production use)" ON )
option (DEVEL_DEBUGGING_TEST "Enable debugging tests (OFF for production use)"   OFF)
option (DEVEL_DEBUGGING_FLAG "Enable debugging compilation flag (OFF for production use)"   OFF)
option (DEVEL_PROFILING    "Enable profiling (OFF for production use)"   OFF)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/contrib/ut")
include_directories("${PROJECT_SOURCE_DIR}/contrib/orilib")
include_directories("${PROJECT_SOURCE_DIR}/contrib/ann")
include_directories("${PROJECT_SOURCE_DIR}/neut")
include_directories("${PROJECT_SOURCE_DIR}/neper_t")
include_directories("${PROJECT_SOURCE_DIR}/neper_m")
include_directories("${PROJECT_SOURCE_DIR}/neper_v")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_BINARY_DIR}/contrib/ut")
include_directories("${PROJECT_BINARY_DIR}/contrib/orilib")
include_directories("${PROJECT_BINARY_DIR}/contrib/ann")
include_directories("${PROJECT_BINARY_DIR}/neut")
include_directories("${PROJECT_BINARY_DIR}/neper_t")
include_directories("${PROJECT_BINARY_DIR}/neper_m")
include_directories("${PROJECT_BINARY_DIR}/neper_v")

# configure a header file to pass some of the CMake settings to the
# source code
configure_file (
  "${PROJECT_SOURCE_DIR}/neper_config.h.in"
  "${PROJECT_BINARY_DIR}/neper_config.h"
)

add_definitions(-Wall -W)
if (DEVEL_OPTIMIZATION)
  add_definitions(-O3)
endif ()

if (DEVEL_DEBUGGING_FLAG)
  add_definitions(-g -Werror)
endif ()

if (DEVEL_PROFILING)
  add_definitions(-pg)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif ()

add_subdirectory(contrib/ut)
add_subdirectory(contrib/orilib)
add_subdirectory(contrib/ann)
add_subdirectory(neut)
add_subdirectory(neper_t)
add_subdirectory(neper_m)
add_subdirectory(neper_v)

add_executable (neper neper.c)

set (EXTRA_LIBS ${EXTRA_LIBS} ut)
set (EXTRA_LIBS ${EXTRA_LIBS} orilib)
set (EXTRA_LIBS ${EXTRA_LIBS} neut)
set (EXTRA_LIBS ${EXTRA_LIBS} neper_t)
set (EXTRA_LIBS ${EXTRA_LIBS} neper_m)
set (EXTRA_LIBS ${EXTRA_LIBS} neper_v)
set (EXTRA_LIBS ${EXTRA_LIBS} ann) # must be after neut

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if (HAVE_LIBSCOTCH)
  find_package (LIBSCOTCH REQUIRED)
  set (EXTRA_LIBS ${EXTRA_LIBS} ${Scotch_LIBRARIES})
  include_directories (${Scotch_INCLUDE_DIRS})
  if (LIBSCOTCH)
    message ("Found libscotch")
  endif (LIBSCOTCH)
endif ()

if (HAVE_PTHREAD)
  find_package (Threads REQUIRED)
  set (EXTRA_LIBS ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif ()

target_link_libraries (neper ${EXTRA_LIBS})

install (TARGETS neper DESTINATION bin
         PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ
	 GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# CTEST
enable_testing ()
include(CTest)

# Module -T

add_test (T_n                     ./neper -T -n 5)
add_test (T_id                    ./neper -T -n 5 -id 2)
add_test (T_dim                   ./neper -T -n 5 -dim 2)
add_test (T_domain_cube           ./neper -T -n 5 -domain "cube(1,2,4)")
add_test (T_domain_cylinder       ./neper -T -n 5 -domain "cylinder(1,2,100)")
add_test (T_domain_sphere         ./neper -T -n 5 -domain "sphere(2,100)")
add_test (T_periodic              ./neper -T -n 5 -periodic 1)
add_test (T_periodic_x            ./neper -T -n 5 -periodic x)
add_test (T_morpho_voronoi        ./neper -T -n 5 -morpho voronoi)
add_test (T_morpho_gg             ./neper -T -n 5 -morpho gg)
add_test (T_morpho_centroidal     ./neper -T -n 5 -morpho centroidal)
add_test (T_morpho_cube           ./neper -T -n from_morpho -morpho "cube(2)")
add_test (T_morpho_cube3          ./neper -T -n from_morpho -morpho "cube(2,3,4)")
add_test (T_morpho_square         ./neper -T -n from_morpho -dim 2 -morpho "square(2)")
add_test (T_morpho_square2        ./neper -T -n from_morpho -dim 2 -morpho "square(2,3)")
add_test (T_morpho_lamellar_w     ./neper -T -n from_morpho -morpho "lamellar(w=0.1)")
add_test (T_morpho_lamellar_w2    ./neper -T -n from_morpho -morpho "lamellar(w=0.1:0.02)")
# add_test (T_morpho_tesr)
add_test (T_morphooptini_packing  ./neper -T -n 5 -morpho gg -morphooptiini coo:packing,weight:radeq)
add_test (T_morphooptini_random   ./neper -T -n 5 -morpho gg -morphooptiini coo:random,weight:avradeq)
# add_test (T_multiseed)
add_test (T_morphooptiobjective_cvm ./neper -T -n 5 -morpho gg -morphooptiobjective cvm)
add_test (T_morphooptiobjective_ad ./neper -T -n 5 -morpho gg -morphooptiobjective ad)
add_test (T_morphooptiobjective_FL2 ./neper -T -n 5 -morpho gg -morphooptiobjective FL2)
add_test (T_morphooptiobjective_FL2w ./neper -T -n 5 -morpho gg -morphooptiobjective FL2w)
add_test (T_morphooptidof          ./neper -T -n 5 -morpho gg -morphooptidof x,z)
add_test (T_morphooptistop_itermax ./neper -T -n 5 -morpho gg -morphooptistop itermax=10)
add_test (T_morphooptistop_val     ./neper -T -n 5 -morpho gg -morphooptistop val=0.3)
add_test (T_morphooptistop_time    ./neper -T -n 5 -morpho gg -morphooptistop time=0.3)
add_test (T_morphooptistop_timeval ./neper -T -n 5 -morpho gg -morphooptistop val=0.1,time=0.3)
add_test (T_morphooptialgo         ./neper -T -n 5 -morpho gg -morphooptialgo praxis)
add_test (T_morphooptigrid         ./neper -T -n 5 -morpho gg -morphooptigrid "regular(-1,10,110)")
add_test (T_morphooptismooth       ./neper -T -n 5 -morpho gg -morphooptismooth 0.1)
add_test (T_morphooptideltamax         ./neper -T -n 5 -morpho gg -morphooptideltamax 0.1)
add_test (T_morphooptinistep       ./neper -T -n 5 -morpho gg -morphooptiinistep diameq/5)
add_test (T_morphooptilogtime      ./neper -T -n 5 -morpho gg -morphooptilogtime iter,total,cumtotal)
add_test (T_morphooptilogvar       ./neper -T -n 5 -morpho gg -morphooptilogvar iter,id,x,y,z,w)
add_test (T_morphooptilogval       ./neper -T -n 5 -morpho gg -morphooptilogval iter,val,valmin)
add_test (T_morphooptilogdis       ./neper -T -n 5 -morpho gg -morphooptilogdis iter,x,tarpdf)
add_test (T_sort                   ./neper -T -n 5 -sort x+y+z)
add_test (T_scale                  ./neper -T -n 5 -scale 2:1:0.5)
add_test (T_transform_rmsat        ./neper -T -n 5 -for tesr -transform rmsat)
add_test (T_transform_rs           ./neper -T -n 5 -for tesr -transform "rasterscale(0.5,1,2)")
add_test (T_transform_grow         ./neper -T -n 5 -for tesr -domain "cylinder(1,1,100)" -transform grow)
add_test (T_ori                    ./neper -T -n 5 -ori "fibre(x,1,1,1)")
add_test (T_oricrysym              ./neper -T -n 5 -oricrysym cubic)
add_test (T_reg                    ./neper -T -n 100 -reg 1)
add_test (T_reg_fmax               ./neper -T -n 100 -reg 1 -fmax 10)
add_test (T_reg_sel                ./neper -T -n 100 -reg 1 -sel 0.01)
add_test (T_reg_rsel               ./neper -T -n 100 -reg 1 -rsel 0.5)
add_test (T_reg_mloop              ./neper -T -n 100 -reg 1 -mloop 1)
add_test (T_o                      ./neper -T -n 5 -o foo)
add_test (T_format                 ./neper -T -n 5 -format tess,geo,ply,obj,3dec,fe,tesr,ami)
add_test (T_tesrformat             ./neper -T -n 5 -format tesr -tesrformat binary8)
add_test (T_tesrsize               ./neper -T -n 5 -format tesr -tesrsize 10)
add_test (T_oridescriptor          ./neper -T -n 5 -oridescriptor R)
add_test (T_oriformat              ./neper -T -n 5 -oriformat fepx)
add_test (T_statcell              ./neper -T -n 5 -statcell "id,x,y,z,true,body,state,area,vol,size,diameq,radeq,sphericity,convexity,dihangleav,dihanglemin,dihanglemax,dihanglelist,vernb,edgenb,facenb,neighnb,verlist,edgelist,facelist,npolylist,facearealist,faceeqlist,vercoolist,scaleid(1)")
add_test (T_statseed              ./neper -T -n 5 -statseed id,x,y,z,w)
add_test (T_statver               ./neper -T -n 5 -statver  id,x,y,z,true,body,state,domtype,edgenb,facenb,polynb)
add_test (T_statedge              ./neper -T -n 5 -statedge id,x,y,z,true,body,state,domtype,length,cyl,vernb,facenb,polynb,facelist)
add_test (T_statface              ./neper -T -n 5 -statface id,x,y,z,true,body,state,domtype,area,diameq,radeq,circularity,dihangleav,dihanglemin,dihanglemax,dihanglelist,ff,vernb,edgenb,polynb,neighnb,verlist,edgelist)
add_test (T_statpoly              ./neper -T -n 5 -statpoly "id,x,y,z,true,body,state,area,vol,size,diameq,radeq,sphericity,convexity,dihangleav,dihanglemin,dihanglemax,dihanglelist,vernb,edgenb,facenb,neighnb,verlist,edgelist,facelist,npolylist,facearealist,faceeqlist,vercoolist,scaleid(1)")
# add_test (T_statpoint)
# add_test (T_checktess)
add_test (T_loadtess              ./neper -T -loadtess n5-id1.tess -statcell x -format tess,tesr)
add_test (T_loadtesr              ./neper -T -loadtesr n5-id1.tesr -statcell x -format tesr)

add_test (Tm2_n                     ./neper -T -n 5::5 -o n5x5)
add_test (Tm2_dim                   ./neper -T -n 5::5 -dim 2)
add_test (Tm2_domain_cube           ./neper -T -n 5::5 -domain "cube(1,2,4)")
add_test (Tm2_domain_cylinder       ./neper -T -n 5::5 -domain "cylinder(1,2,100)")
add_test (Tm2_domain_sphere         ./neper -T -n 5::5 -domain "sphere(2,100)")
add_test (Tm2_morpho_voronoi        ./neper -T -n 5::5 -morpho voronoi::voronoi)
add_test (Tm2_morpho_gg             ./neper -T -n 5::5 -morpho gg::gg)
add_test (Tm2_morpho_centroidal     ./neper -T -n 5::5 -morpho centroidal::centroidal)
add_test (Tm2_morpho_cube           ./neper -T -n from_morpho::5 -morpho "cube(2)::voronoi")
add_test (Tm2_morpho_square         ./neper -T -n from_morpho::5 -dim 2 -morpho "square(2)::voronoi")
add_test (Tm2_morpho_lamellar_w     ./neper -T -n 5::from_morpho -morpho "gg::lamellar(w=0.1)")
add_test (Tm2_morpho_lamellar_w2    ./neper -T -n 5::from_morpho -morpho "gg::lamellar(w=0.1:0.02)")
# add_test (Tm2_morpho_tesr)
# add_test (Tm2_multiseed)
add_test (Tm2_sort                   ./neper -T -n 5::5 -sort x+y+z)
add_test (Tm2_scale                  ./neper -T -n 5::5 -scale 2:1:0.5)
add_test (Tm2_reg                    ./neper -T -n 5::5 -reg 1)

add_test (Tm3_n                     ./neper -T -n 3::3::3)
add_test (Tm3_dim                   ./neper -T -n 3::3::3 -dim 2)
add_test (Tm3_domain_cube           ./neper -T -n 3::3::3 -domain "cube(1,2,4)")
add_test (Tm3_domain_cylinder       ./neper -T -n 3::3::3 -domain "cylinder(1,2,100)")
add_test (Tm3_domain_sphere         ./neper -T -n 3::3::3 -domain "sphere(2,100)")
add_test (Tm3_morpho_voronoi        ./neper -T -n 3::3::3 -morpho voronoi::voronoi::voronoi)
add_test (Tm3_morpho_gg             ./neper -T -n 3::3::3 -morpho gg::gg)
add_test (Tm3_morpho_centroidal     ./neper -T -n 2::2::2 -morpho centroidal::centroidal::centroidal)
add_test (Tm3_morpho_cube           ./neper -T -n from_morpho::3::3 -morpho "cube(2)::voronoi::voronoi")
add_test (Tm3_morpho_square         ./neper -T -n from_morpho::3::3 -dim 2 -morpho "square(2)::voronoi::voronoi")
add_test (Tm3_morpho_lamellar_w     ./neper -T -n 3::3::from_morpho -morpho "gg::gg::lamellar(w=0.1)")
add_test (Tm3_morpho_lamellar_w2    ./neper -T -n 3::3::from_morpho -morpho "gg::gg::lamellar(w=0.1:0.02)")
# add_test (Tm3_morpho_tesr)
# add_test (Tm3_multiseed)
add_test (Tm3_sort                   ./neper -T -n 3::3::3 -sort x+y+z)
add_test (Tm3_scale                  ./neper -T -n 3::3::3 -scale 2:1:0.3)
add_test (Tm3_reg                    ./neper -T -n 3::3::3 -reg 1)

# Module -M
add_test (M_pretest1             ./neper -T -n 5 -id 1 -reg 1 -format tess,tesr)
add_test (M_pretest2             ./neper -T -n 5 -id 1 -reg 1 -dim 2 -format tess,tesr -transform rmsat -o n5-id1-dim2)
add_test (M_meshing              ./neper -M n5-id1.tess -mesh3dclconv 1)
add_test (M_meshing_hex          ./neper -M n5-id1.tess -elt hex)
add_test (M_cl                   ./neper -M n5-id1.tess -mesh3dclconv 1 -cl 0.5)
add_test (M_rcl                  ./neper -M n5-id1.tess -mesh3dclconv 1 -rcl 0.7)
add_test (M_cl_hex               ./neper -M n5-id1.tess -elt hex -cl 0.5)
add_test (M_rcl_hex              ./neper -M n5-id1.tess -elt hex -rcl 0.7)
add_test (M_cl_hex_dim2          ./neper -M n5-id1-dim2.tess -elt hex -cl 0.5)
add_test (M_rcl_hex_dim2         ./neper -M n5-id1-dim2.tess -elt hex -rcl 0.7)
add_test (M_cl_expr              ./neper -M n5-id1.tess -mesh3dclconv 1 -cl 0.25,id==2:0.5)
add_test (M_rcl_expr             ./neper -M n5-id1.tess -mesh3dclconv 1 -rcl 1,id==2:0.7)
add_test (M_dim                  ./neper -M n5-id1.tess -mesh3dclconv 1 -dim 1,2)
add_test (M_dim2                 ./neper -M n5-id1.tess -mesh3dclconv 1 -dim all)
add_test (M_dim_expr             ./neper -M n5-id1.tess -mesh3dclconv 1 -dim inputdim)
add_test (M_order                ./neper -M n5-id1.tess -mesh3dclconv 1 -order 2)
add_test (M_order_dim2           ./neper -M n5-id1-dim2.tess -mesh3dclconv 1 -dim 2 -order 2)
add_test (M_order_hex            ./neper -M n5-id1.tess -mesh3dclconv 1 -elt hex -order 2)
add_test (M_order_hex_dim2       ./neper -M n5-id1-dim2.tess -mesh3dclconv 1 -elt hex -order 2)
add_test (M_pl                   ./neper -M n5-id1.tess -mesh3dclconv 1 -pl 1)
add_test (M_clratio              ./neper -M n5-id1.tess -mesh3dclconv 1 -clratio 1:0.5:0.25)
add_test (M_clmin                ./neper -M n5-id1.tess -mesh3dclconv 1 -clmin 0.5)
add_test (M_meshqualmin          ./neper -M n5-id1.tess -mesh3dclconv 1 -meshqualmin 0)
add_test (M_meshqualexpr         ./neper -M n5-id1.tess -mesh3dclconv 1 -meshqualexpr Osize)
add_test (M_meshqualdisexpr      ./neper -M n5-id1.tess -mesh3dclconv 1 -meshqualdisexpr dis)
add_test (M_mesh2dalgo_mead      ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh2dalgo mead)
add_test (M_mesh2dalgo_dela      ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh2dalgo dela)
add_test (M_mesh2dalgo_fron      ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh2dalgo fron)
add_test (M_mesh2dalgo_netg      ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh3dalgo netg)
add_test (M_mesh3dalgo_netgnetg  ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh3dalgo netg:netg)
add_test (M_mesh3dalgo_netggmsh  ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh3dalgo netg:gmsh)
add_test (M_mesh3dalgo_netggmne  ./neper -M n5-id1.tess -mesh3dclconv 1 -mesh3dalgo netg:gmne)

if (HAVE_LIBSCOTCH)
  add_test (M_part                 ./neper -M -loadmesh n5-id1.msh -part 4)
  add_test (M_part2                ./neper -M -loadmesh n5-id1.msh -part 2:2)
  add_test (M_partbalancing        ./neper -M -loadmesh n5-id1.msh -part 2 -partbalan 0)
  add_test (M_part_dim2            ./neper -M n5-id1-dim2.tess -part 4)
endif ()

add_test (M_o                    ./neper -M -loadmesh n5-id1.msh -o foo)
add_test (M_format_inp           ./neper -M -loadmesh n5-id1.msh -for inp)
add_test (M_format_geof          ./neper -M -loadmesh n5-id1.msh -for geof)
add_test (M_format_fepx          ./neper -M -loadmesh n5-id1.msh -for fepx)
add_test (M_remesh1              ./neper -M n5-id1.msh -for tess,msh -o foo)
add_test (M_remesh2              ./neper -M n5-id1.tess,n5-id1.msh -o foo -for tess,msh)
add_test (M_remesh3              ./neper -M n5-id1.tess,n5-id1.msh -clratio 2:1:0.5 -o foo -for tess,msh)
add_test (M_nset1                ./neper -M n5-id1.tess -nset faces,edges)
add_test (M_nset2                ./neper -M n5-id1.tess -nset x0,y1z1,vertices)
add_test (M_nset3                ./neper -M n5-id1.tess -nset all)
add_test (M_faset                ./neper -M n5-id1.tess -faset z0)
add_test (M_statnode             ./neper -M -loadmesh n5-id1.msh -statnode id,x,y,z,dim,2dmeshp,2dmeshd,2dmeshv,2dmeshn)
add_test (M_statelt              ./neper -M -loadmesh n5-id1.msh -statelt id,x,y,z)
add_test (M_statelt0d            ./neper -M -loadmesh n5-id1.msh -statelt0d id,x,y,z,elset0d,true,body,domtype)
add_test (M_statelt1d            ./neper -M -loadmesh n5-id1.msh -statelt1d id,x,y,z,elset1d,length,true,body,domtype)
add_test (M_statelt2d            ./neper -M -loadmesh n5-id1.msh -statelt2d id,x,y,z,elset2d,area,true,body,domtype)
add_test (M_statelt3d            ./neper -M -loadmesh n5-id1.msh -statelt3d id,x,y,z,elset3d,vol,rr,true,body,2dmeshp,2dmeshd,2dmeshv,2dmeshn)
add_test (M_statelset            ./neper -M -loadmesh n5-id1.msh -statelset id,x,y,z)
add_test (M_statelset0d          ./neper -M -loadmesh n5-id1.msh -statelset0d id,x,y,z,eltnb,true,body,domtype)
add_test (M_statelset1d          ./neper -M -loadmesh n5-id1.msh -statelset1d id,x,y,z,length,eltnb,true,body,domtype)
add_test (M_statelset2d          ./neper -M -loadmesh n5-id1.msh -statelset2d id,x,y,z,eltnb,true,body,domtype)
add_test (M_statelset3d          ./neper -M -loadmesh n5-id1.msh -statelset3d id,x,y,z,vol,eltnb,rrav,rrmin,rrmax,Osize,eltnb,true,body)
# add_test (M_statpoint            ./neper -M -loadmesh n5-id1.msh -loadpoint coo -statpoint id,x,y,z,elt3d,elset3d,2dmeshp,2dmeshd,2dmeshv,2dmeshn)
add_test (M_tesr_elt_hex         ./neper -M n5-id1.tesr -elt hex -cl 0.1)
add_test (M_tesr_dim2            ./neper -M n5-id1-dim2.tesr -rcl 0.7 -for msh,geof,inp)

add_test (M_interface            ./neper -M n5-id1.tess -mesh3dclconv 1 -interface discontinuous -format msh,inp)
add_test (M_interface2           ./neper -M n5-id1.tess -mesh3dclconv 1 -interface cohesive -format msh,inp)

# Module -V
add_test (V_pretest1             ./neper -T -n 5 -id 1 -reg 1 -statver x -statedge x -statface x -statpoly x -statcell x -for tess,ori)
add_test (V_pretest2             ./neper -T -n 5 -id 1 -reg 1 -dim 2 -o n5-id1-dim2 -statedge x -statface x -statcell x)
add_test (V_pretest3             ./neper -M n5-id1.tess -mesh3dclconv 1 -statnode x -statelt0d x -statelt1d x -statelt2d x -statelt3d x -statelset3d x)
add_test (V_pretest4             ./neper -M n5-id1-dim2.tess -mesh3dclconv 1 -statnode x -statelt0d x -statelt1d x -statelt2d x -statelset2d x)
add_test (V_pretest5             ./neper -M n5-id1.tess -mesh3dclconv 1 -statnode x,y,z -o xyz)
add_test (V_vercol_rgb           ./neper -V n5-id1.tess -showcell none -showver all -datavercol blue -print foo)
add_test (V_vercol_rgb2          ./neper -V n5-id1.tess -showcell none -showver all -datavercol 10,20,30 -print foo)
add_test (V_vercol_id            ./neper -V n5-id1.tess -showcell none -showver all -datavercol id -print foo)
add_test (V_vercol_scal          ./neper -V n5-id1.tess -showcell none -showver all -datavercol scal:n5-id1.stver -print foo)
add_test (V_vercol_scal_colscheme ./neper -V n5-id1.tess -showcell none -showver all -datavercol scal:n5-id1.stver -datavercolscheme black,white -print foo)
add_test (V_verscale             ./neper -V n5-id1.tess -showcell none -showver all -datavercol scal:n5-id1.stver -dataverscale -1:1 -print foo)
add_test (V_verscale2            ./neper -V n5-id1.tess -showcell none -showver all -datavercol scal:n5-id1.stver -dataverscale 0:0.1:1 -print foo)
add_test (V_verscaletitle        ./neper -V n5-id1.tess -showcell none -showver all -datavercol scal:n5-id1.stver -dataverscaletitle foo -print foo)
add_test (V_vertrs               ./neper -V n5-id1.tess -showcell none -showver all -datavercol id -datavertrs 0.5 -print foo)
add_test (V_edgecol_rgb          ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol blue -print foo)
add_test (V_edgecol_rgb2         ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol 10,20,30 -print foo)
add_test (V_edgecol_id           ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol id -print foo)
add_test (V_edgecol_scal         ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol scal:n5-id1.stedge -print foo)
add_test (V_edgecol_scal_colscheme ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol scal:n5-id1.stedge -dataedgecolscheme black,white -print foo)
add_test (V_edgescale            ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol scal:n5-id1.stedge -dataedgescale -1:1 -print foo)
add_test (V_edgescale2           ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol scal:n5-id1.stedge -dataedgescale 0:0.1:1 -print foo)
add_test (V_edgescaletitle       ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol scal:n5-id1.stedge -dataedgescaletitle foo -print foo)
add_test (V_edgetrs              ./neper -V n5-id1.tess -showcell none -showedge all -dataedgecol id -dataedgetrs 0.5 -print foo)
add_test (V_facecol_rgb          ./neper -V n5-id1.tess -showcell none -showface all -datafacecol blue -print foo)
add_test (V_facecol_rgb2         ./neper -V n5-id1.tess -showcell none -showface all -datafacecol 10,20,30 -print foo)
add_test (V_facecol_id           ./neper -V n5-id1.tess -showcell none -showface all -datafacecol id -print foo)
add_test (V_facecol_scal         ./neper -V n5-id1.tess -showcell none -showface all -datafacecol scal:n5-id1.stface -print foo)
add_test (V_facecol_scal_colscheme ./neper -V n5-id1.tess -showcell none -showface all -datafacecol scal:n5-id1.stface -datafacecolscheme black,white -print foo)
add_test (V_facescale            ./neper -V n5-id1.tess -showcell none -showface all -datafacecol scal:n5-id1.stface -datafacescale -1:1 -print foo)
add_test (V_facescale2           ./neper -V n5-id1.tess -showcell none -showface all -datafacecol scal:n5-id1.stface -datafacescale 0:0.1:1 -print foo)
add_test (V_facescaletitle       ./neper -V n5-id1.tess -showcell none -showface all -datafacecol scal:n5-id1.stface -datafacescaletitle foo -print foo)
add_test (V_facetrs              ./neper -V n5-id1.tess -showcell none -showface all -datafacecol id -datafacetrs 0.5 -print foo)
add_test (V_polycol_rgb          ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol blue -print foo)
add_test (V_polycol_rgb2         ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol 10,20,30 -print foo)
add_test (V_polycol_id           ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol id -print foo)
add_test (V_polycol_scal         ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol scal:n5-id1.stpoly -print foo)
add_test (V_polycol_scal_colscheme ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol scal:n5-id1.stpoly -datapolycolscheme black,white -print foo)
add_test (V_polyscale            ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol scal:n5-id1.stpoly -datapolyscale -1:1 -print foo)
add_test (V_polyscale2           ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol scal:n5-id1.stpoly -datapolyscale 0:0.1:1 -print foo)
add_test (V_polyscaletitle       ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol scal:n5-id1.stpoly -datapolyscaletitle foo -print foo)
add_test (V_polytrs              ./neper -V n5-id1.tess -showcell none -showpoly all -datapolycol id -datapolytrs 0.5 -print foo)
add_test (V_cellcol_rgb          ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol blue -print foo)
add_test (V_cellcol_rgb2         ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol 10,20,30 -print foo)
add_test (V_cellcol_id           ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol id -print foo)
add_test (V_cellcol_scal         ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol scal:n5-id1.stcell -print foo)
add_test (V_cellcol_scal_colscheme ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol scal:n5-id1.stcell -datacellcolscheme black,white -print foo)
add_test (V_cellscale            ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol scal:n5-id1.stcell -datacellscale -1:1 -print foo)
add_test (V_cellscale2           ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol scal:n5-id1.stcell -datacellscale 0:0.1:1 -print foo)
add_test (V_cellscaletitle       ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol scal:n5-id1.stcell -datacellscaletitle foo -print foo)
add_test (V_celltrs              ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol id -datacelltrs 0.5 -print foo)
add_test (V_cellcol_ori ./neper -V n5-id1.tess -showcell none -showcell all -datacellcol ori -print foo)

add_test (V_verrad ./neper -V n5-id1.tess -showver all -dataverrad 0.05 -datavercol id -dataedgerad 0.02 -dataedgecol id -datapolycol id -print foo)
add_test (V_csys ./neper -V n5-id1.tess -showcsys 1 -print foo)
add_test (V_csyscoo ./neper -V n5-id1.tess -showcsys 1 -datacsyscoo 0.1:0.2:0.3 -print foo)
add_test (V_csyslength ./neper -V n5-id1.tess -showcsys 1 -datacsyslength 1 -print foo)
add_test (V_csysrad ./neper -V n5-id1.tess -showcsys 1 -datacsysrad 0.1 -print foo)
add_test (V_csyslabel ./neper -V n5-id1.tess -showcsys 1 -datacsyslabel foo:bar:baz -print foo)
add_test (V_csyscol ./neper -V n5-id1.tess -showcsys 1 -datacsyscol red -print foo)

add_test (V_nodecol ./neper -V n5-id1.msh -showelt none -shownode all -datanodecol id -print foo)
add_test (V_nodecol_scal ./neper -V n5-id1.msh -showelt none -shownode all -datanodecol scal:n5-id1.stnode -print foo)
add_test (V_nodecolscheme ./neper -V n5-id1.msh -showelt none -shownode all -datanodecol scal:n5-id1.stnode -datanodecolscheme black,white -print foo)
add_test (V_nodescale ./neper -V n5-id1.msh -showelt none -shownode all -datanodecol scal:n5-id1.stnode -datanodescale -1:1 -print foo)
add_test (V_nodescale2 ./neper -V n5-id1.msh -showelt none -shownode all -datanodecol scal:n5-id1.stnode -datanodescale 0:0.1:1 -print foo)

add_test (V_elsetcol ./neper -V n5-id1.msh -showelt none -showelset3d all -dataelsetcol id -print foo)
add_test (V_elsetcol_scal ./neper -V n5-id1.msh -showelt none -showelset3d all -dataelsetcol scal:n5-id1.stelset3d -print foo)
add_test (V_elsetcolscheme ./neper -V n5-id1.msh -showelt none -showelset3d all -dataelsetcol scal:n5-id1.stelset3d -dataelsetcolscheme black,white -print foo)
add_test (V_elsetscale ./neper -V n5-id1.msh -showelt none -showelset3d all -dataelsetcol scal:n5-id1.stelset3d -dataelsetscale -1:1 -print foo)
add_test (V_elsetscale2 ./neper -V n5-id1.msh -showelt none -showelset3d all -dataelsetcol scal:n5-id1.stelset3d -dataelsetscale 0:0.1:1 -print foo)
add_test (V_elsetscaletitle ./neper -V n5-id1.msh -showelt none -showelset3d all -dataelsetcol scal:n5-id1.stelset3d -dataelsetscaletitle foo -print foo)
add_test (V_elsetcol_ori ./neper -V n5-id1.msh -showelset3d all -dataelsetcol ori:n5-id1.ori -print foo)

add_test (V_nodecoo ./neper -V n5-id1.msh -datanodecoo xyz.stnode -print foo)
add_test (V_nodecoo_disp ./neper -V n5-id1.msh -datanodecoo disp:xyz.stnode -print foo)
add_test (V_nodecoofact ./neper -V n5-id1.msh -datanodecoo xyz.stnode -datanodecoofact 2 -print foo)
add_test (V_slicemesh ./neper -V n5-id1.msh -dataelset3dcol id -slicemesh "x=0.2,y=0.4,z=0.6" -print foo)
add_test (V_slicemesh2 ./neper -V n5-id1.msh -dataelset3dcol id -datanodecoo xyz.stnode -slicemesh "x=0.2,y=0.4,z=0.6" -print foo)
add_test (V_showmesh1 ./neper -V n5-id1.tess,n5-id1.msh -showtess 1 -showmesh 0 -print foo)
add_test (V_showmesh2 ./neper -V n5-id1.tess,n5-id1.msh -showtess 1 -showmesh 1 -print foo)
add_test (V_slicemesh3 ./neper -V n5-id1.tess,n5-id1.msh -slicemesh "x=0.2,y=0.4,z=0.6" -showtess 1 -showmesh 0 -showmeshslice 0 -print foo)
add_test (V_slicemesh4 ./neper -V n5-id1.tess,n5-id1.msh -slicemesh "x=0.2,y=0.4,z=0.6" -showtess 0 -showmesh 1 -showmeshslice 0 -print foo)
add_test (V_slicemesh5 ./neper -V n5-id1.tess,n5-id1.msh -slicemesh "x=0.2,y=0.4,z=0.6" -showtess 0 -showmesh 0 -showmeshslice 1 -print foo)
add_test (V_show ./neper -V n5-id1.tess -showpoly "z<0.5" -showface "z>0.9" -showedge "(y>0.9)&&(x<0.1)" -showver "((x>0.9)&&(y>0.9))&&(z>0.9)" -print foo)
add_test (V_showfaceinter ./neper -V n5-id1.tess -showpoly none -showface all -showfaceinter 1 -print foo)
add_test (V_showelt ./neper -V n5-id1.msh -showelt3d all -showelt1d all -print foo)
add_test (V_dataelt3d ./neper -V n5-id1.msh -dataelt3dcol id -showshadow 0 -print foo)
add_test (V_cameracoo1 ./neper -V n5-id1.tess -cameracoo x+2*vx:y+vy:3 -print foo)
add_test (V_cameracoo2 ./neper -V n5-id1.msh -datanodecoo xyz.stnode -cameracoo x+2*vx:y+vy:3 -print foo)
add_test (V_cameracoo3 ./neper -V n5-id1.tess,n5-id1.msh -datanodecoo xyz.stnode -cameracoo x+2*vx:y+vy:3 -print foo)
add_test (V_cameralookat1 ./neper -V n5-id1.tess -cameralookat x:y:1 -print foo)
add_test (V_cameralookat2 ./neper -V n5-id1.msh -cameralookat x:y:1 -print foo)
add_test (V_cameralookat3 ./neper -V n5-id1.tess,n5-id1.msh -cameralookat x:y:1 -print foo)
add_test (V_cameraangle ./neper -V n5-id1.tess -cameraangle 12 -print foo)
add_test (V_camerasky ./neper -V n5-id1.tess -camerasky 1:0:0 -print foo)
add_test (V_cameraprojection ./neper -V n5-id1.tess -cameraprojection orthographic -print foo)
add_test (V_imagesize ./neper -V n5-id1.tess -imagesize 900:800 -print foo)
add_test (V_imagebackground ./neper -V n5-id1.tess -imagebackground black -print foo)
add_test (V_imageantialias ./neper -V n5-id1.tess -imageantialias 1 -print foo)
add_test (V_imageformat ./neper -V n5-id1.tess -imageformat pov -print foo)
add_test (V_loop ./neper -V n5-id1.msh -loop F 1 0.1 1.2 -datanodecoo xyz.stnode -datanodecoofact F -print fooF -endloop)

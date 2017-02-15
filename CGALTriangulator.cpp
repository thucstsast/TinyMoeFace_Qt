#include <iostream>
#include "CGALTriangulator.h"

CGALTriangulator::CGALTriangulator()
{

}

void CGALTriangulator::triangulate(CDT& cdt)
{
    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
    std::cout << "Meshing the triangulation with default criterias..."
              << std::endl;
    Mesher mesher(cdt);
    mesher.refine_mesh();
    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
    std::cout << "Meshing with new criterias..." << std::endl;
    // 0.125 is the default shape bound. It corresponds to abound 20.6 degree.
    // 0.5 is the upper bound on the length of the longuest edge.
    // See reference manual for Delaunay_mesh_size_traits_2<K>.
    mesher.set_criteria(Criteria(0.125, 15.0));
    mesher.refine_mesh();
    std::cout << "Number of vertices: " << cdt.number_of_vertices() << std::endl;
}

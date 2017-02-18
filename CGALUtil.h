#ifndef CGALUTIL_H
#define CGALUTIL_H

#include <QPointF>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CGAL::Delaunay_mesher_2<CDT, Criteria> Mesher;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;
uint qHash(const Vertex_handle &key);

class CGALUtil
{
public:
    template<class T>
    static QPointF toQtPointF(const T& cgalPoint);
};

template<class T>
QPointF CGALUtil::toQtPointF(const T &cgalPoint)
{
    return QPointF(cgalPoint.x(), cgalPoint.y());
}

#endif // CGALUTIL_H

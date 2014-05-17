#ifndef _KRYLOV_TOPO_
#define _KRYLOV_TOPO_

#include <cstddef>
#include <set>
#include <unordered_map>
#include <vector>
#include <cilk/cilk.h>

namespace kry
{


class Node
{
  public:
    uint16_t id;
    double x, y;
    std::vector<Node*> nbrs;
    std::vector<double> A;
    size_t degree(){ return nbrs.size(); }

    void addNeighbor(Node *n, double a)
    {
      nbrs.push_back(n);
      A.push_back(a);
    }
};

class System
{
  public:
    std::unordered_map<uint16_t, Node> nodes;

    void node(uint16_t id, double y)
    {
      Node &n = nodes[id];
      n.id = id;
      n.y = y;
    }

    void connect(uint16_t id1, uint16_t id2, double a)
    {
      Node &n1 = nodes[id1];
      Node &n2 = nodes[id2];

      n1.addNeighbor(&n2, a);
      n2.addNeighbor(&n1, a);

    }

    size_t size(){ return nodes.size(); }

};

void
normalize(std::vector<double> &v)
{
  double norm{0};
  for(size_t i=0; i<v.size(); ++i) norm+= pow(v[i], 2);
  norm = sqrt(norm);
  v.data()[0:v.size()] /= norm;
}

class Arnoldi
{
  public:
    System *s;
    size_t alpha{0};
    double *H;

    std::vector<std::vector<double>> q;
    std::vector<std::vector<double>> h;

    Arnoldi(System *s, std::vector<double> guess) : s{s} 
    {
      q.push_back(guess);
    }
   
    void point(Node *n)
    {
      for(size_t i=0; i<n->degree(); ++i)
      {
        q[alpha+1][n->id] += n->A[i] * q[alpha][n->id];
      }
    }

    void go()
    {
      normalize(q[0]);
      q.push_back(std::vector<double>(s->size()));
      for(auto &p : s->nodes) { cilk_spawn point(&p.second); }
    }
};

}

#endif

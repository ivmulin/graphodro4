#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <cstddef>
#include <cstring>
#include <graphodro4/core/igraph.hpp>
#include <random>
#include <vector>

class GraphFactory {
   public:
    static size_t complete(IGraph& g, size_t n);

    static size_t completeBipartite(IGraph& g, size_t n, size_t m);

    static size_t tree(IGraph& g, size_t n);

    static size_t star(IGraph& g, size_t n);

    static size_t cycle(IGraph& g, size_t n);

    static size_t path(IGraph& g, size_t n);

    static size_t wheel(IGraph& g, size_t n);

    static size_t random(IGraph& g, size_t n, const double p);

    static size_t cubic(IGraph& g, size_t n);

    static size_t fixedComponents(IGraph& g, size_t n, size_t k,
                                  bool addNoise = true);

    static size_t forest(IGraph& g, size_t n, size_t k);

    static size_t fixedBridges(IGraph& g, size_t n, size_t k,
                               const double p = 0.3);

    static size_t fixedArticulationPoints(IGraph& g, size_t n, size_t k,
                                          const double p = 0.3);

    static size_t fixedTwoBridges(IGraph& g, size_t n, size_t k,
                                  const double p = 0.3);

    static size_t halin(IGraph& g, size_t n);

   private:
    static std::random_device rd;
    static std::mt19937 gen;

   private:
    static void generatePruferCode(std::vector<size_t>& code, size_t n);

    static void decodePruferCode(IGraph& g, std::vector<size_t>& code,
                                 size_t n);

    static std::vector<size_t> splitVertices(size_t n, size_t k);

    static size_t populateComponent(IGraph& g, size_t offset, size_t n_i,
                                    size_t max_i, const double p_i);
};

#endif  // GENERATOR_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "vec.h"

struct Mesh {
    std::vector<vec3> points;
    std::vector<long> offsets;
    std::vector<long> facets;
    long nfacets;
};

// Very simple ply loader
struct PlyLoader {

    Mesh load(std::string filename) {
        Mesh m;

        std::ifstream f(filename, std::ios_base::in);

        std::string line;

        std::getline(f, line);
        if (line != "ply") {
            std::cerr << "Not valid ply file format." << std::endl;
            return m;
        }
        
        std::getline(f, line);
        if (line != "format ascii 1.0") {
            std::cerr << "Unsupported ply file format." << std::endl;
            return m;
        }

        // std::vector<vec3> points;
        // // std::vector<std::vector<long>> facets;
        // std::vector<long> offsets;
        // std::vector<long> facets;
        int nverts;
        long counter = 0;
        while (std::getline(f, line)) {

            const char *ptr = line.c_str();

            if (line.rfind("comment", 0) == 0) {
                continue;
            }
                   
            if (line.rfind("element vertex", 0) == 0) {
                char *end;
                nverts = strtol(ptr + 14, &end, 10);
            }

            if (line.rfind("element face", 0) == 0) {
                char *end;
                m.nfacets = strtol(ptr + 12, &end, 10);
            }

            if (line == "end_header") {
                break;
            }

        }

        // Read vertices
        for (int i = 0; i < nverts; ++i) {
            std::getline(f, line);
            const char *ptr = line.c_str();
            char *end;

            auto x = strtod(ptr, &end);
            auto y = strtod(end, &end);
            auto z = strtod(end, &end);

            m.points.emplace_back(x,y,z);
        }

        // Read facets
        for (int i = 0; i < m.nfacets; ++i) {
            std::getline(f, line);
            const char *ptr = line.c_str();
            char *end;

            auto n = strtol(ptr, &end, 10);
            m.offsets.push_back(n);
            // std::vector<long> facet;
            for (int j = 0; j < n; ++j) {
                auto idx = strtol(end, &end, 10);
                m.facets.push_back(idx);
                // facet.push_back(idx);
            }         
            // facets.push_back(facet);
        }

        return m;

    }

};
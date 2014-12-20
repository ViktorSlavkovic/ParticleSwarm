#ifndef SWARM_H_INCLUDED
#define SWARM_H_INCLUDED

#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <climits>
#include <iostream>

#include <Geo2D.hpp>

using namespace std;
using namespace Geo2D;

namespace opt {
    class swarm {
        private:
            vector<Vector <double> > x;
            vector<Vector <double> > v;
            vector<Vector <double> > pb;
            Vector<double> gb;
            double gb_val;
            uint64_t n;
            double c1, c2, w, v_max;
            double x_min, x_max, y_min, y_max;

            double (*f)(double, double);
        public:
            swarm(double (*cost)(double, double), uint64_t n, double v_max, double x_min, double x_max, double y_min, double y_max);
            ~swarm();

            void update(double dt, bool rand_w=false);
            void update(double dt, double gb_x, double gb_y, bool rand_w=false);

            uint64_t getSwarmSize() const;

            void setCognitive(double val);
            void setSocial(double val);
            void setInertia(double val);

            double getCognitive() const;
            double getSocial() const;
            double getInertia() const;

            double getParticlePos_x(uint64_t idx) const;
            double getParticlePos_y(uint64_t idx) const;
    };
}

#endif // SWARM_H_INCLUDED

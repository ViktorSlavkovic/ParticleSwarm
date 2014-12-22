#include "swarm.h"

using namespace std;
using namespace opt;
using namespace Geo2D;

swarm::swarm(double (*cost)(double, double), uint64_t n, double v_max, double x_min, double x_max, double y_min, double y_max) {
    f=cost;

    this->n=n;
    this->c1=1.494;
    this->c2=1.494;
    this->w=0.729;

    this->v_max=v_max;

    this->x_min=x_min;
    this->x_max=x_max;
    this->y_min=y_min;
    this->y_max=y_max;

    gb_val=LLONG_MAX;

    srand(time(0));

    x.resize(n);
    for (vector<Vector<double> >::size_type i=0; i<x.size(); i++) {
        x[i].setX((double)rand()/RAND_MAX*(x_max-x_min)+x_min);
        x[i].setY((double)rand()/RAND_MAX*(y_max-y_min)+y_min);
    }
    v.resize(n,Vector<double>(0,0));
    pb.resize(n);
    for (vector<Vector<double> >::size_type i=0; i<x.size(); i++) pb[i]=x[i];

    gb_val=f(x[0].getX(), x[0].getY());
    gb=x[0];
    for (vector<Vector<double> >::size_type i=1; i<x.size(); i++) {
        double pom=f(x[i].getX(), x[i].getY());
        if (pom<gb_val) {
            gb_val=pom;
            gb=x[i];
        }
    }
}

swarm::~swarm() {

}

void swarm::update(double dt, double gb_x, double gb_y, bool rand_w) {
    gb.setX(gb_x);
    gb.setY(gb_y);

    double px,py,px_old,py_old,pom;
    for (vector<Vector<double> >::size_type i=0; i<x.size(); i++) {
        px_old=x[i].getX();
        py_old=x[i].getY();

        v[i] =  v[i] * w * ( (rand_w) ? ( (double)rand() / RAND_MAX ) : 1 )
                + (pb[i]-x[i]) * c1 * ( (double)rand() / RAND_MAX )
                + (gb-x[i]) * c2 * ( (double)rand() / RAND_MAX );

        if (v[i].modulus()>v_max)
            v[i]*=v_max/v[i].modulus();

        x[i]=x[i]+v[i]*dt;

        px=x[i].getX();
        py=x[i].getY();
        if (px<x_min) x[i].setX(2*x_min-px);
        if (px>x_max) x[i].setX(2*x_max-px);
        if (py<y_min) x[i].setY(2*y_min-py);
        if (py>y_max) x[i].setY(2*y_max-py);

        pom=f(x[i].getX(), x[i].getY());

        if (pom<f(px_old,py_old)) pb[i]=x[i];
    }
}

void swarm::update(double dt, bool rand_w) {
    double px,py,px_old,py_old,pom;
    for (vector<Vector<double> >::size_type i=0; i<x.size(); i++) {

        px_old=x[i].getX();
        py_old=x[i].getY();

        v[i] =  v[i] * w * ( (rand_w) ? ( (double)rand() / RAND_MAX ) : 1 )
                + (pb[i]-x[i]) * c1 * ( (double)rand() / RAND_MAX )
                + (gb-x[i]) * c2 * ( (double)rand() / RAND_MAX );

        if (v[i].modulus()>v_max)
            v[i]*=v_max/v[i].modulus();

        x[i]=x[i]+v[i]*dt;

        px=x[i].getX();
        py=x[i].getY();
        if (px<x_min) x[i].setX(2*x_min-px);
        if (px>x_max) x[i].setX(2*x_max-px);
        if (py<y_min) x[i].setY(2*y_min-py);
        if (py>y_max) x[i].setY(2*y_max-py);

        pom=f(x[i].getX(), x[i].getY());

        if (pom<f(px_old,py_old)) pb[i]=x[i];
        if (pom<gb_val) {
            gb_val=pom;
            gb=x[i];
        }

    }
}


uint64_t swarm::getSwarmSize() const { return n; }

double swarm::getParticlePos_x(uint64_t idx) const { return x[idx].getX(); }

double swarm::getParticlePos_y(uint64_t idx) const { return x[idx].getY(); }

void swarm::setCognitive(double val) { c1=val; }

void swarm::setSocial(double val) { c2=val; }

void swarm::setInertia(double val) { w=val; }

double swarm::getCognitive() const { return c1; }

double swarm::getSocial() const { return c2; }

double swarm::getInertia() const { return w; }

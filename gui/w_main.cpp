#include "w_main.h"
#include "ui_w_main.h"

#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
using namespace std;

w_main::w_main(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::w_main)
{
    ui->setupUi(this);
}

w_main::~w_main()
{
    delete ui;
}

void w_main::on_btn_run_clicked()
{
    unsigned long long n = (unsigned long long) ui->sb_n->value();

    double c1 = ui->sb_c1->value();
    double c2 = ui->sb_c2->value();
    double w = ui->sb_w->value();
    double vmax = ui->sb_vmax->value();
    double dt = ui->sb_dt->value();

    bool known_gb = ui->rb_kgb->isChecked();
    bool rand_w = ui->rb_randw->isChecked();

    bool display_fps = ui->rb_displayFPS->isChecked();
    bool lock_fps = ui->rb_lockFPS->isChecked();
    int fps = (int) ui->sb_FPS->value();

    string s;
    ostringstream sout(s);
    //sout<<"echo %cd% > out.txt";
    sout<<"ps "<<" -swarm_size "<<n<<" -cognitive "<<c1<<" -social "<<c2<<" -inertia "<<w<<" -v_max "<<vmax<<" -dt "<<dt;
    if (known_gb) sout<<" -known_gb ";
    if (rand_w) sout<<" -rand_w ";
    if (display_fps) sout<<" -display_fps ";
    if (lock_fps) sout<<" -lock_fps "<<fps;

    s=sout.str();

    ofstream fout("call.txt");
    fout<<s<<endl;
    fout.close();

    system(s.c_str());
}

void w_main::on_rb_lockFPS_toggled(bool checked)
{
    ui->sb_FPS->setEnabled(checked);
}

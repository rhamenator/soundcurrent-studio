// SPDX-License-Identifier: GPL-3.0-only
#include "readable_peak.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
void check(bool ok,const char *why){if(!ok)throw std::runtime_error(why);}
int main(){
    soundcurrent::ReadablePeak a,b;
    check(a.update(.1,0),"first reading");b.update(.1,0);
    int updates=0;
    for(int ms=1;ms<=1000;++ms){
        const double peak=ms<400?.5:.01;
        if(a.update(peak,ms))++updates;
        if(ms%20==0)b.update(peak,ms);
    }
    check(updates==5,"numeric rate is not 5 Hz");
    check(std::abs(a.db()-b.db())<1e-9,"ballistics depend on refresh rate");
    check(a.db()>-40 && a.db()<-15,"no smoothed decay");
    a.update(1.1,1001);check(a.clipping(1200),"short clipping transient lost");
    check(!a.clipping(2100),"clipping hold never clears");
    a.reset();check(a.update(.1,2500) && std::abs(a.db()+20)<1e-9,"reset retains state");
    std::cout<<"PASS: 5 Hz numeric updates, independent refresh rate, smoothed decay, transient clip hold/reset\n";
}

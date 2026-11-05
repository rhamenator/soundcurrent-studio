// SPDX-License-Identifier: GPL-3.0-only
#include "live_reconfiguration.h"
#include <stdexcept>
#include <iostream>
void check(bool ok,const char *why){if(!ok)throw std::runtime_error(why);}
int main(){
    int live=2,stops=0,starts=0;
    try{soundcurrent::replaceLiveSession(live,6,[]{throw std::runtime_error("unsupported");},[&]{++stops;},[&]{++starts;});}catch(const std::runtime_error &){}
    check(live==2 && stops==0 && starts==0,"preflight rejection stopped playback");
    soundcurrent::replaceLiveSession(live,6,[]{},[&]{++stops;},[&]{++starts;});
    check(live==6 && stops==1 && starts==1,"layout not applied");
    bool rejected=false;
    try{soundcurrent::replaceLiveSession(live,8,[]{},[&]{++stops;},[&]{++starts;if(live==8)throw std::runtime_error("restart");});}catch(const std::runtime_error &){rejected=true;}
    check(rejected && live==6 && stops==2 && starts==3,"failed restart not rolled back");
    try{soundcurrent::replaceLiveSession(live,8,[]{},[]{},[]{throw std::runtime_error("failure");});}catch(const std::runtime_error &){}
    check(live==6,"rollback failure corrupted saved live session");
    std::cout<<"PASS: preflight leaves audio running, apply/restart, restoration attempt, failure preservation\n";
}

#include "PID.h"

#include <iostream>
#include <vector>
using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double _Kp, double _Ki, double _Kd) {
    Kp = _Kp;
    Ki = _Ki;
    Kd = _Kd;
    
    p_error = 0;
    i_error = 0;
    d_error = 0;

    prev_cte = 0;
}


void PID::UpdateControlGains(std::vector<double>controls){
    Kp = controls[0];
    Ki = controls[1];
    Kd = controls[2];
}


void PID::UpdateError(double cte) {
    // proportional error is set to the new cte
    p_error = cte;
    
    // integral error is the sum
    i_error += cte;
    
    // differential error is the difference between the new cte and the old one divided by Δt (=1)
    d_error = cte - prev_cte;
    prev_cte = cte;
}

double PID::TotalError() {
    return - Kp * p_error - Ki * i_error - Kd * d_error;
}


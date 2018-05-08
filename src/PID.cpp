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

//double sum(std::vector<double> v){
//    double _sum = 0.;
//    for(auto &n: v){
//        _sum += n;
//    }
//    return _sum;
//}
//
//void PID::twiddle(double best_err, double tol){
//    int n_params = 3;
//    std::vector<double> dp = {1., 1., 1.};
//    std::vector<double>
//    int it = 0;
//    while (sum(dp) > tol)
//        sdt:cout << "Iteration "<< it <<", best error = "<< best_err;
//    }
//}


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


//
//  Twiddle.cpp
//  pid
//
//  Created by Ezistas Batururimi on 08/05/2018.
//

#include "Twiddle.hpp"

//Twiddle::Twiddle(){};
Twiddle::~Twiddle(){};

void Twiddle::Init(){
    _params = {0., 0., 0.};
    _dp = {1., 1., 1.};
}

void Twiddle::run(double error, double tol){
    // best_error not yet set
    if(_best_err < __DBL_EPSILON__){
        _best_err = error;
        return;
    }
}

double Twiddle::Kp(){
    return _params[0];
}
double Twiddle::Ki(){
    return _params[1];
}
double Twiddle::Kd(){
    return _params[2];
}



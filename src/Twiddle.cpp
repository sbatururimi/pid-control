//
//  Twiddle.cpp
//  pid
//
//  Created by Ezistas Batururimi on 08/05/2018.
//

#include "Twiddle.hpp"
#include <iostream>


//Twiddle::Twiddle(){};
Twiddle::~Twiddle(){};

void Twiddle::Init(){
    _params = {0., 0., 0.};
    _dp = {1., 1., 1.};
}

double sum(std::vector<double> v){
    double _sum = 0.;
    for(auto &n: v){
        _sum += n;
    }
    return _sum;
}


bool Twiddle::_isStepUnknown(){
    return (_step & StepUnknown) == StepUnknown;
}


void Twiddle::markBestErrorSet(){
    _step |= BestErrorSet;
}

void Twiddle::run(double error, double tol){
    // best_error not yet set
    //    std::cout << "eps= "<<__DBL_EPSILON__ << std::endl;
    //    if(_best_err < __DBL_EPSILON__){
    
    if(_isStepUnknown()){
        _best_err = error;
        _it = 0;
        markBestErrorSet();
        return;
    }
    
    if(sum(_dp) < tol){
        std::cout << "Iteration " << _it << ", best error = " << error;
        
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



//
//  Twiddle.cpp
//  pid
//
//  Created by Ezistas Batururimi on 08/05/2018.
//

#include "Twiddle.hpp"

Twiddle::Twiddle();
Twiddle::~Twiddle();

void Twiddle::Init(std::vector<double> p, std::vector<double> dp){
    if(p.size() > 0){
        _params = p;
    }
    else{
        _params = {0., 0., 0.};
    }
    
    if (dp.size() > 0) {
        _dp = dp;
    }
    else{
        _dp = {1., 1., 1.};
    }
}

void Twiddle::run(double tol, double error){
    // best_error not yet set
    if(_best_err < __DBL_EPSILON__){
        _best_err = error;
        return;
    }
}



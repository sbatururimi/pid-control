//
//  Twiddle.hpp
//  pid
//
//  Created by Ezistas Batururimi on 08/05/2018.
//

#ifndef Twiddle_hpp
#define Twiddle_hpp

#include <stdio.h>
#include <vector>

class Twiddle{
private:
    std::vector<double> _params;
    std::vector<double> _dp;
    
    double _best_err;

public:
    Twiddle(): _best_err(0.);
    virtual ~Twiddle();
    void Init(std::vector<double> p, std::vector<double> dp);
    void run(double tol = 0.2, double error);
};


#endif /* Twiddle_hpp */

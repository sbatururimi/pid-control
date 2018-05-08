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
    Twiddle(): _best_err(0.){};
//    Twiddle();
    virtual ~Twiddle();
    void Init();
    void run(double error, double tol = 0.2);
    
    double Kp();
    double Ki();
    double Kd();
    
    std::vector<double> params(){
        return _params;
    }
};


#endif /* Twiddle_hpp */

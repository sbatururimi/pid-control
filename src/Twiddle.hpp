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


const uint8_t StepUnknown = 0;
const uint8_t BestErrorSet = 0x1;

class Twiddle{
private:
    std::vector<double> _params;
    std::vector<double> _dp;
    
    int _it;
    int _index;
    uint8_t _step;
    
    double _best_err;
    
    // Steps using a 8 bit mask
    bool _isStepUnknown();
    
    /*
     * Specify that the best error has been initialized
     */
    void markBestErrorSet();

public:
    Twiddle(): _index(0), _step(StepUnknown), _best_err(0.) {};
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

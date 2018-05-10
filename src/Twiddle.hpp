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


const uint8_t StepUnknown = 0x0;
const uint8_t BestErrorSet = 0x1;
const uint8_t DpSumChecked = 0x2;
const uint8_t UpdateNextParam = 0x4;
const uint8_t UpdateDpAgain = 0x8;

class Twiddle{
private:
    std::vector<double> _params;
    std::vector<double> _dp;
    
    int _it;
    int _index;
    uint8_t _step;
    
    double _best_err;
    
    // Steps using a 8 bit mask
    bool _isBestErrorSet();
    bool _isStepDpSumCheckedSet();
    bool _isUpdateNextParamSet();
    bool _isUpdateDpAgainSet();
    
    /*
     * Specify that the best error has been initialized
     */
    void _setBestError();
    void _setDpSumChecked();
    void _setUpdateNextParam();
    void _setUpdateDpAgain();
    
    void _unsetDpSumChecked();
    void _unsetUpdateNextParam();
    void _unsetUpdateDpAgain();
    
    void _incrementVectorWithDp();

public:
    Twiddle(): _index(0), _step(StepUnknown), _best_err(0.) {};
//    Twiddle();
    virtual ~Twiddle();
    void Init();
    void run(double error, bool &shouldResetSimulator, double tol = 0.0001);
    
    double Kp();
    double Ki();
    double Kd();
    
    std::vector<double> params(){
        return _params;
    }
};


#endif /* Twiddle_hpp */

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
//    _params = {0.15, 0., 2.5};
    _params = {0.17, 0, 2.51};
    
//    _params = {0, 0., 0};
    _dp = {0.01, 0.01, 0.01};
}
void printVectorValues(std::vector<double> v, std::string name){
    std::cout << name <<" = [";
    for (int i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i == v.size() - 1) {
            std::cout << "]" << std::endl;
        }
        else{
            std::cout << ", ";
        }
    }
    
}


double sum(std::vector<double> v){
    double _sum = 0.;
    for(auto &n: v){
        _sum += n;
    }
    return _sum;
}

//------ Srt a bit in the mask
bool Twiddle::_isBestErrorSet(){
    return (_step & BestErrorSet) == BestErrorSet;
}

bool Twiddle::_isStepDpSumCheckedSet(){
    return (_step & DpSumChecked) == DpSumChecked;
}

bool Twiddle::_isUpdateNextParamSet(){
    return (_step & UpdateNextParam) == UpdateNextParam;
}

bool Twiddle::_isUpdateDpAgainSet(){
    return (_step & UpdateDpAgain) == UpdateDpAgain;
}


//----Set bits
void Twiddle::_setBestError(){
    _step |= BestErrorSet;
}

void Twiddle::_setDpSumChecked(){
    _step |= DpSumChecked;
}

void Twiddle::_setUpdateNextParam(){
    _step |= UpdateNextParam;
}

void Twiddle::_setUpdateDpAgain(){
    _step |= UpdateDpAgain;
}

//-----------Unset a bit in the mask
void Twiddle::_unsetDpSumChecked(){
    uint8_t mask = ~DpSumChecked;
    _step &= mask;
}


void Twiddle::_unsetUpdateNextParam(){
    uint8_t mask = ~UpdateNextParam;
    _step &= mask;
}

void Twiddle::_unsetUpdateDpAgain(){
    uint8_t mask = ~UpdateDpAgain;
    _step &= mask;
}

//----------Other methods

void Twiddle::run(double error, bool &shouldResetSimulator, bool DEBUG, double tol){
    // if best error not set, early stop
    if(!_isBestErrorSet()){
        _best_err = error;
        _it = 0;
        _setBestError();
    }
    
    // do we need to check the sum(dp) at this step? (if inside the for-loop, then we don't need to do that,
    // otherwise we are doing another while loop and we need to check that).
    if (!_isStepDpSumCheckedSet()) {
        double s = sum(_dp);
        if(s > tol){
            
            std::cout << std::endl << "Iteration " << _it << ", best error = " << _best_err << ", sum(dp) = "<< s <<  std::endl;
            printVectorValues(_params, "p");
            printVectorValues(_dp, "dp");
            std::cout << std::endl;
            
             _params[_index] += _dp[_index];
            
            shouldResetSimulator = true;
            
            // specify that the next time we doesn't need to check the sum
            _setDpSumChecked();
            return;
        }
        if (DEBUG) {
            std::cout<<"Optimal values found" << std::endl << std::endl;
            printVectorValues(_params, "p");
            printVectorValues(_dp, "dp");
        }
        
        
        return;
    }
    
    if (_isUpdateNextParamSet()) {
        _unsetUpdateNextParam();
        
        // for-loop finished, do a new iteration in the while-loop
        if(_index == _params.size()){
            if (DEBUG) {
                std::cout << "for-loop finished" << std::endl;
            }
            _index = 0;
            ++_it;
            _unsetDpSumChecked();
            run(error, shouldResetSimulator, DEBUG);
            return;
        }
         _params[_index] += _dp[_index];
        shouldResetSimulator = true;
        
        if (DEBUG) {
            std::cout << "update next param, index=" << _index << std::endl;
            printVectorValues(_params, "p");
            printVectorValues(_dp, "dp");
        }
        return;
    }
    
    
    // still considering the same index, that is the same dp[index]
    if (!_isUpdateNextParamSet()) {
        // inside the for-loop (check the python code reference)
        if (_isUpdateDpAgainSet()) {
            _unsetUpdateDpAgain();
            
            if (error < _best_err) {
                _best_err = error;
                _dp[_index] *= 1.1;
            }
            else{
                _params[_index] += _dp[_index];
                _dp[_index] *= 0.9;
            }
            
            if (DEBUG) {
                std::cout << "update dp again, go to next index after index=" << _index << std::endl;
                printVectorValues(_params, "p");
                printVectorValues(_dp, "dp");
            }
            
            ++_index;
            _setUpdateNextParam();
            
            run(error, shouldResetSimulator, DEBUG);
            return;
        }
        
        if(!_isUpdateDpAgainSet()){
            if (error < _best_err) {
                _best_err = error;
                _dp[_index] *= 1.1;
                
                if (DEBUG) {
                    std::cout << "update dp, go to next index after index=" << _index  << std::endl;
                    printVectorValues(_params, "p");
                    printVectorValues(_dp, "dp");;
                }
                
                ++_index;
                _setUpdateNextParam();
                
                // go to the next i of the for-loop
                run(error, shouldResetSimulator, DEBUG);
                return;
            }
            else{
                _params[_index] -= 2 * _dp[_index];
                
                if (DEBUG) {
                    std::cout << "reset param, index=" << _index << std::endl;
                    printVectorValues(_params, "p");
                    printVectorValues(_dp, "dp");
                }
                
                _setUpdateDpAgain();
                shouldResetSimulator = true;
                return;
            }
        }
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

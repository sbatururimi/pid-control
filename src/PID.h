#ifndef PID_H
#define PID_H

#include <vector>

class PID {
public:
    /*
     * Errors
     */
    double p_error;
    double i_error;
    double d_error;
    
    double prev_cte;
    
    /*
     * Coefficients
     */
    double Kp;
    double Ki;
    double Kd;
    
    
    /*
     * Constructor
     */
    PID();
    
    /*
     * Destructor.
     */
    virtual ~PID();
    
    void twiddle(double best_err, double tol=0.0001);
    /*
     * Initialize PID.
     */
    void Init(double Kp, double Ki, double Kd);
    
    
    /*
     * Update the PID error variables given cross track error.
     */
    void UpdateError(double cte);
    
    /*
     * Calculate the total PID error.
     */
    double TotalError();
    
//    void UpdateControlGain(int index, double value);
    void UpdateControlGains(std::vector<double>controls);
};

#endif /* PID_H */

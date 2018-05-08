#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
    auto found_null = s.find("null");
    auto b1 = s.find_first_of("[");
    auto b2 = s.find_last_of("]");
    if (found_null != std::string::npos) {
        return "";
    }
    else if (b1 != std::string::npos && b2 != std::string::npos) {
        return s.substr(b1, b2 - b1 + 1);
    }
    return "";
}

double sum(std::vector<double> v){
    double _sum = 0.;
    for(auto &n: v){
        _sum += n;
    }
    return _sum;
}

//void PID::twiddle(double best_err, double tol){
//    
//    while (sum(dp) > tol)
//        sdt:cout << "Iteration "<< it <<", best error = "<< best_err;
//}


int main()
{
    uWS::Hub h;
    
    PID pid;
    // TODO: Initialize the pid variable.
    // p term-the car quickly overshoot, trying to steer the car toward the center of the road
    // i term: no bias present for the simulator
    // d term:helps to counterpart the p-term
//    double Kp = 0.15;
//    double Ki = 0.0;
//    double Kd = 2.5;
    double Kp = 0.;
    double Ki = 0.;
    double Kd = 0.;
    pid.Init(Kp, Ki, Kd);
    

    clock_t begin;
    int count = 0;
    int n = 100;
    double err = 0.;
    bool is_run_cycle = true;
    
    std::vector<double> dp = {1., 1., 1.};
    double best_err;
    int it = 0;
    int index = 0;
    bool first_twiddle_run = true;
//    bool compute_pid = false;
    
    
    h.onMessage([&pid, &count, &err, &dp, &it, &best_err, &index, &is_run_cycle, & first_twiddle_run,n ]
                (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
        // "42" at the start of the message means there's a websocket message event.
        // The 4 signifies a websocket message
        // The 2 signifies a websocket event
        if (length && length > 2 && data[0] == '4' && data[1] == '2')
        {
            auto s = hasData(std::string(data).substr(0, length));
            if (s != "") {
                auto j = json::parse(s);
                std::string event = j[0].get<std::string>();
                if (event == "telemetry") {
                    // j[1] is the data JSON object
//                    clock_t end = clock();
//                    std::cout << double(end - begin) / CLOCKS_PER_SEC << " sec" << std::endl;
//
////                    std::cout << j << std::endl;
//                    std::cout << count << " step";
//                    std::cout << "---" << std::endl;
                    
                    double cte = std::stod(j[1]["cte"].get<std::string>());
                    double speed = std::stod(j[1]["speed"].get<std::string>());
                    double angle = std::stod(j[1]["steering_angle"].get<std::string>());
                    double steer_value = 0.0;
                    /*
                     * TODO: Calcuate steering value here, remember the steering value is
                     * [-1, 1].
                     * NOTE: Feel free to play around with the throttle and speed. Maybe use
                     * another PID controller to control the speed!
                     */
                    // twiddle
                    if (is_run_cycle) {
                        pid.UpdateError(cte);
                        steer_value += pid.TotalError();
                        
                        
                        // DEBUG
                        std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;
                        
                        // adjust the move using the PID controller
                        json msgJson;
                        msgJson["steering_angle"] = steer_value;
                        msgJson["throttle"] = 0.3;
                        auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                        std::cout << msg << std::endl;
                        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                        
                        
                        if(count >= n){
                            err += cte * cte;
                        }
                        count++;
                        
                        if (count == 2 * n) {
                            err = err / n;
//                            best_err = err;
                            
                            // initial values for twiddle computed
                            is_run_cycle = false;
                            
                            // reset values
                            count = 0;
//                            err = 0.;
                        }
                    }
                    
                    // compute control gains (Kp, Ki, Kd) using twiddle
                    if (!is_run_cycle) {
                        double tol = 0.2;
                        double sum_dp = sum(dp);
                        
                        if (first_twiddle_run == 0) {
                            first_twiddle_run = false;
                            best_err = err;
                        }
                        
                        if(index <= dp.size() && sum_dp > tol){
                            if (index < dp.size()) {
                                // (1) update a control gain + reset the simulator
                                //                                std::cout << "Iteration " << it <<", best error = "<< best_err;
                                pid.UpdateControlGain(index, dp[index]);
                                
                                // reset the simulator
                                std::string reset_msg = "42[\"reset\",{}]";
                                ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
                                
                                is_run_cycle = true;
                            }
                            
                            // if we have already updated 1 control gain, then try to adjust the dp
                            int i = index - 1;
                            if (i >= 0) {
                                if(err < best_err){
                                    best_err = err;
                                    dp[i] *= 1.1;
                                }
                                else{                                    
                                    pid.UpdateControlGain(i, - 2 * dp[i]);
                                    
                                    // reset the simulator
                                    std::string reset_msg = "42[\"reset\",{}]";
                                    ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
                                    
                                    is_run_cycle = true;
                                    
                                    
                                }
                            }
                            
                            index++;
                            
                            
                            
//                            if (index < dp.size()) {
//                                // (1) update a control gain + reset the simulator
////                                std::cout << "Iteration " << it <<", best error = "<< best_err;
//                                pid.UpdateControlGain(index, dp[index]);
//
//                                // reset the simulator
//                                std::string reset_msg = "42[\"reset\",{}]";
//                                ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
//
//                                is_run_cycle = true;
//                                index++;
//                            }
                        }
                        else if(sum_dp > tol){
                            it += 1;
                        }
                    }
                }
            } else {
                // Manual driving
                std::string msg = "42[\"manual\",{}]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            }
        }
    });
    
    // We don't need this since we're not using HTTP but if it's removed the program
    // doesn't compile :-(
    h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
        const std::string s = "<h1>Hello world!</h1>";
        if (req.getUrl().valueLength == 1)
        {
            res->end(s.data(), s.length());
        }
        else
        {
            // i guess this should be done more gracefully?
            res->end(nullptr, 0);
        }
    });
    
    h.onConnection([&h, &begin](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
        std::cout << "Connected!!!" << std::endl;
        begin = clock();
    });
    
    h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
        ws.close();
        std::cout << "Disconnected" << std::endl;
    });
    
    int port = 4567;
    if (h.listen(port))
    {
        std::cout << "Listening to port " << port << std::endl;
    }
    else
    {
        std::cerr << "Failed to listen to port" << std::endl;
        return -1;
    }
    h.run();
}

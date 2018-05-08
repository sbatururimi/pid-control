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
    bool twiddle_started = false;
    
    std::vector<double> dp = {1., 1., 1.};
    double best_err;
    int it = 0;
    
    
    h.onMessage([&pid, &count, &err, &dp, &it, &twiddle_started, &best_err, /*begin,*/ n ](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
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
                    pid.UpdateError(cte);
                    steer_value += pid.TotalError();
                    
                    
                    // DEBUG
                    std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;
                    
                    json msgJson;
                    msgJson["steering_angle"] = steer_value;
                    msgJson["throttle"] = 0.3;
//                    msgJson["throttle"] = (1 - std::abs(steer_value)) * 0.5 + 0.2;
                    auto msg = "42[\"steer\"," + msgJson.dump() + "]";
                    std::cout << msg << std::endl;
                    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                    
                    if (!twiddle_started) {
                        if(count >= n){
                            err += cte * cte;
                        }
                        
                        if (count == 2 * n) {
                            err = err / n;
                            best_err = err;
                            count = 0;
                            twiddle_started = true;
                            
                        }                        
                        count++;
                    }
                    
                    if (twiddle_started) {
                        it = 0;
                        double tol = 0.2;
                        if (sum(dp) > tol) {
                            std::cout << "Iteration " << it <<", best error = "<< best_err;
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

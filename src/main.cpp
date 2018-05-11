#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include "Twiddle.hpp"
#include <math.h>

#include <chrono>

using namespace std::chrono;

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


int main()
{
    uWS::Hub h;
    
    // TODO: Initialize the pid variable.
    // p term-the car quickly overshoot, trying to steer the car toward the center of the road
    // i term: no bias present for the simulator
    // d term:helps to counterpart the p-term
    //    double Kp = 0.15;
    //    double Ki = 0.0;
    //    double Kd = 2.5;
    
    Twiddle twiddle;
    twiddle.Init();
    
    PID pid;
    pid.Init(twiddle.Kp(), twiddle.Ki(), twiddle.Kd());
    
    int count = 0;
    int n = 0;
    double err = 0.;
    high_resolution_clock::time_point begin;
//    time_t begin = 0;
    
    
    //    h.onMessage([&pid, &count, &err, &dp, &it, &best_err, &index, &is_run_cycle, & first_twiddle_run,n ]
    //                (uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    h.onMessage([&begin, &pid, &twiddle, &count, &err, &n](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
        // "42" at the start of the message means there's a websocket message event.
        // The 4 signifies a websocket message
        // The 2 signifies a websocket event
        if (length && length > 2 && data[0] == '4' && data[1] == '2'){
            auto s = hasData(std::string(data).substr(0, length));
            if (s != "") {
                auto j = json::parse(s);
                std::string event = j[0].get<std::string>();
                if (event == "telemetry") {
                    // j[1] is the data JSON object
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
                    pid.UpdateError(cte);
//                    steer_value += pid.TotalError();
                    steer_value = pid.TotalError();
                    
                    // DEBUG
                    bool DEBUG = false;
                    if (DEBUG) {
                        std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;
                    }
                    
                    
                    // adjust the move using the PID controller
                    json msgJson;
                    msgJson["steering_angle"] = steer_value;
                    msgJson["throttle"] = 0.3;
                    auto msg = "42[\"steer\"," + msgJson.dump() + "]";
//                    std::cout << msg << std::endl;
                    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
                    
                    
//                    std::cout << begin << std::endl;
                    high_resolution_clock::time_point end = high_resolution_clock::now();
                    auto duration = duration_cast<milliseconds>( end - begin ).count() / 1000.;
//                    std::cout << "duration: " << duration << std::endl;
//                    if (count > 2 * n - 1) {
                    if (duration > 36.176) { // in front of the bridge
                        err = err / n;
                        bool shouldResetSimulator = false;
                        twiddle.run(err, shouldResetSimulator, true);
                        pid.UpdateControlGains(twiddle.params());

                        // reset
                        err = 0.;
                        count = 0;
                        n = 0;

                        // do we need to reset the simulator?
                        if (shouldResetSimulator) {
                            std::string reset_msg = "42[\"reset\",{}]";
                            ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
                        }
                    }
                
                    if (duration > 10){
//                    if(count >= n){
                        n++;
                        err += cte * cte;
                    }
                    count++;
                    
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
        begin = high_resolution_clock::now();
//        std::time_t now_c = std::chrono::system_clock::to_time_t(begin);
//        std::cout << now_c;
        
//        time(&begin);
//        std::cout << begin << std::endl;
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

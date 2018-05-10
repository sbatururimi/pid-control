# **PID control Cloning** 

## Writeup


**PID Control Project Project**

The goals / steps of this project are the following:
Using the cross track error (CTE) and the velocity (mph) provided by the simulator, we must compute the appropriate steering angle.


[//]: # (Image References)

[image1]: ./output/Artboard.png "Callbacks to simulator"
[image2]: ./output/bit_mask.png "Callbacks to simulator"
[image3]: ./output/twiddle_mask.png "Twiddle - mask"


## Rubric Points
I started by implementing a simple PID controller and by tuning the PID hyperparameters I applied the general processing flow.
Then I used an implementation of the twiddle algorithm to tune further these hyperparameters.

### PID controller
The following observation and approach waas used to perform a simple tuning of the hyperparameters:
1) I have first set all PID parameters (p-term, i-term and d-term) to 0. And augmented each separatelly first.
2) the p term-the car quickly overshoot, trying to steer the car toward the center of the road.
I started with it, putting other parameters to 0.
3) i-term: no bias present for the simulator. Otherwise makes the car to go in circles
4) d-term:helps to counterpart the p-term, smoothing the approach to the center line

Using this approach the following values were proved to be good enough:
p = 0.15
i = 0.
d = 2.5

### Twiddle algorithm
Consider the following implementation of the twiddle algorithm in python:

```
def twiddle(tol=0.2): 
    p = [0, 0, 0]
    dp = [1, 1, 1]
    robot = make_robot()
    x_trajectory, y_trajectory, best_err = run(robot, p)

    it = 0
    while sum(dp) > tol:
        print("Iteration {}, best error = {}".format(it, best_err))
        for i in range(len(p)):
            p[i] += dp[i]
            robot = make_robot()
            x_trajectory, y_trajectory, err = run(robot, p)

            if err < best_err:
                best_err = err
                dp[i] *= 1.1
            else:
                p[i] -= 2 * dp[i]
                robot = make_robot()
                x_trajectory, y_trajectory, err = run(robot, p)

                if err < best_err:
                    best_err = err
                    dp[i] *= 1.1
                else:
                    p[i] += dp[i]
                    dp[i] *= 0.9
        it += 1
    return p
```

In our case, the make_robot can't be called directly and instead we should reset the simulator, update the PID controller with newer parameters and then switch back to where we was in the twiddle algorithm.
In order to reset the simulator, I use a "reset" message:

```
std::string reset_msg = "42[\"reset\",{}]";
ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
```

Now, let's consider the callbacks to the simulator. These are the following ones:
![alt text][image1]

In order to keep track of the twiddle execution and the switch back to the simulator call, I use a 4-bit mask (**_step** in the code):
![alt text][image1]

The order has been slightly changed in the c++ code but still follows the python code. For the logic behind the mask, here the scheme:
![alt text][image2]

---
### Files Submitted & Code Quality

The project includes the following files:
* `PID.cpp` containing the PID controller implementation
* `Twiddle.cpp` the twiddle algorithm implementation

## What next
It should be interesting to use the time step instead of the number of loops when dealing with the error computation also the twiddle implementation is not very well optimized for a callback call and investigation may be done in this direction.

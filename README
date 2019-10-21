# One dimensional diffusion over a rough energh landscape
This code models diffusion of non interacting brownian particles in a roguh, one dimensional energy landscape. 
The results agree with experiment at short times and with the asymptotic long time theoretical predictions: 
At very short and very long length scales, the particles behave as if they were in a flat landscape. 
It is in the intermediate regieme where the dips in the lanscape slow down the dynamics.

#### Implementation
+ C++ implementataion of brownianan dynamics, verified to reproduce experimenal results
+ Can continue from last checkpoint if execution interrupted
+ Model parameters determined from config file

#### Execution
```bash
make clean && make
./rough_sim
```

#### Development log
2012/12/01  
- Landscape generated from Gaussian distributed RNG and spherical array.
- Currently not implemented: Periodic boundary mirrors, rescaling of epsilon.
- Config file reading/writing/parsing working
- Particle initialisation tested
- Initial dynamics loop implemented with Metropolis step
2012/12/03 12:01 
- Implemented rescaling of landscape to reflect the desired value of epsilon
- The state vector file does not need to be updated with the landscape and the initial particle positions each time. The only part which changes is the current time and the current particle positions. In fact, once the landscape has been created, epsilon is not needed anymore.
- Implemented periodic boundary in landscape and in particle positions
2012/12/04
- Starting to implement the state persistence routines, wondering how I can save the random seed. Turns out that using a custom RNG might be the best way to go. Don't worry about it for now but maybe get a copy of Mersenne Twister and add it to the random factory.

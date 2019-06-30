# socp_interface

A simple but versatile C++ interface for Second Order Cone Solvers.

Currently, `ECOS` and `MOSEK` are supported. The former is included as submodule.

Download and compile with:  
`git clone --recurse-submodules https://github.com/EmbersArc/socp_interface.git`  
`cd socp_interface`  
`mkdir build`  
`cd build`  
`cmake ..`  
`make`  

See `example.cpp` for a basic usecase.
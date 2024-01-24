import os

env = Environment(ENV = os.environ, CXX="/data/sanchez/tools/gcc-12.2.0/bin/g++")
env.Append( CPPFLAGS = [ "-g", "-std=c++20", "-Wextra", "-Wall", "-Werror" ] )

env.Append( CPPPATH = [ "/data/sanchez/tools/axelf/fmt/include" ])
env.Append( LIBPATH = [ "/data/sanchez/tools/axelf/fmt/build" ] )
env.Append( LIBS = [ "fmt" ])

env.Append( CPPPATH = [ "/data/sanchez/users/axelf/solvers/matrixmarket" ])

env.Program( "spmv", Glob("*.cpp"))
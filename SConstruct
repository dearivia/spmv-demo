import os

env = Environment(ENV = os.environ)
env.Append( CPPFLAGS = [ "-g", "-std=c++20", "-Wextra", "-Wall", "-Werror" ] )

assert "FMTROOT" in os.environ
assert "MATRIXMARKET" in os.environ

MATRIXMARKET = os.environ["MATRIXMARKET"]
FMTROOT = os.environ["FMTROOT"]

env.Append( CPPPATH = [ f"{FMTROOT}/include" ])
env.Append( LIBPATH = [ f"{FMTROOT}/build" ] )
env.Append( LIBS = [ "fmt" ])

env.Append( CPPPATH = [ MATRIXMARKET ])

env.Program( "spmv", Glob("*.cpp"))

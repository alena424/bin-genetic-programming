#
# BIN - cellular automata - majority problem
# 
# @author Alena Tesařová (xtesar36)
# @date 16.4.2021
#

CPP=g++
CPPFLAGS=-Wall -std=gnu++11

all: ca_major
ca_gp: ca_major.cpp ca_sim.hpp
	$(CPP) $(CPPFLAGS) -o $@ $^

zip:
	zip xtesar36.zip ca_major.cpp ca_sim.hpp Makefile

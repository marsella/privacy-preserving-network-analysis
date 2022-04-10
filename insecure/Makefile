
FLAGS=-Wall -Wextra -std=c++11
.PHONY: data

econ: linear.cpp bank.h
	g++ $(FLAGS) -o $@ $<

naive: naive.cpp bank.h
	g++ $(FLAGS) -o $@ $<

test: test_matrix.cpp matrix_utils.h
	g++ $(FLAGS) -o $@ $<

data:: data/geninput.py
	python data/geninput.py

clean:
	rm econ naive test

CXX = clang++
CXXFLAGS = -std=c++17 -I/opt/homebrew/opt/gmp/include
LDFLAGS = -L/opt/homebrew/opt/gmp/lib -lntl -lgmp

all: rsa bonus

rsa: rsa.cpp
	$(CXX) $(CXXFLAGS) rsa.cpp -o rsa $(LDFLAGS)

bonus: bonus.cpp
	$(CXX) $(CXXFLAGS) bonus.cpp -o bonus $(LDFLAGS)

clean:
	rm -f rsa bonus

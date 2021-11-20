CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

lhll_engine: *.cpp *.hpp
	g++ $(CFLAGS) -o lhll_engine.out *.cpp $(LDFLAGS)

.PHONY: test clean

test: lhll_engine
	./lhll_engine.out

clean:
	rm -f lhll_engine

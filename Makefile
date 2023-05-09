CXX := g++
CXXFLAGS := -O2 -std=c++14
CXXFLAGS := $(CXXFLAGS) -pedantic -Wall

CXXFLAGS := $(CXXFLAGS) -fsanitize=address -fsanitize=leak
LDFLAGS := $(LDFLAGS) -fsanitize=address -fsanitize=leak

LDLIBS := -lm

SOURCES := test_arraydeq.c
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))

TESTS	:= $(patsubst %.o,%,$(OBJECTS))

.PHONY: clean

test: run_tests

run_tests: $(TESTS)
	./test_arraydeq

$(TESTS): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

-include $(DEPENDS)

%.o: %.cpp Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJECTS) $(TESTS) $(DEPENDS)


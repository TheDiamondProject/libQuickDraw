# Copyright (c) 2019 Tom Hancocks
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

C-SRC := $(shell find src -type f \( -name "*.c" \))
C-OBJ := $(C-SRC:%.c=%.o)

TEST-SRC := $(shell find tests -type f \( -name "*.c" \))

.PHONY: all
all: libQuickDraw.a

.PHONY: clean
clean:
	- rm tests/testrunner
	- rm *.a *.o

.PHONY: run-all-tests
run-all-tests: testrunner
	./testrunner
	
testrunner: libQuickDraw.a
	$(CC) -o testrunner -I./submodules -I./src -DUNIT_TEST $(TEST-SRC) submodules/libUnit/unit.c libQuickDraw.a

libQuickDraw.a: $(C-OBJ)
	$(AR) -r $@ $^

%.o: %.c
	$(CC) -Wall -Wpedantic -Werror -std=c11 -c -I./submodules -I./src -o $@ $^

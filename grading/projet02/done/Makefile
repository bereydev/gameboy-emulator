
GTK_LIBS := `pkg-config --libs gtk+-3.0`

.PHONY: clean new style feedback submit1 submit2 submit

CFLAGS += -std=c11 -Wall -pedantic -g 

CFLAGS += -Wextra -Wfloat-equal -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings -Wconversion -Wunreachable-code

CFLAGS += -Wuninitialized -Wcast-qual -Winit-self -Wmissing-declarations 

CPPFLAGS += -DDEBUG

CPPFLAGS += -DBLARGG

# ----------------------------------------------------------------------
# feel free to update/modifiy this part as you wish

# all those libs are required on Debian, feel free to adapt it to your box
LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

all:: 

TARGETS := 
CHECK_TARGETS := unit-test-alu_ext
OBJS = 
OBJS_NO_STATIC_TESTS =
OBJS_STATIC_TESTS = 
OBJS = $(OBJS_STATIC_TESTS) $(OBJS_NO_STATIC_TESTS)

# ----------------------------------------------------------------------
# This part is to make your life easier. See handouts how to make use of it.


clean::
	-@/bin/rm -f *.o *~ $(CHECK_TARGETS)

new: clean all

static-check:
	scan-build -analyze-headers --status-bugs -maxloop 64 make CC=clang new

style:
	astyle -n -o -A8 -xt0 *.[ch]

# all those libs are required on Debian, adapt to your box
$(CHECK_TARGETS): LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

check:: $(CHECK_TARGETS)
	$(foreach target,$(CHECK_TARGETS),./$(target) &&) true

# target to run tests
check:: all
	@if ls tests/*.*.sh 1> /dev/null 2>&1; then \
	  for file in tests/*.*.sh; do [ -x $$file ] || echo "Launching $$file"; ./$$file || exit 1; done; \
	fi

IMAGE=chappeli/feedback:latest
feedback:
	@docker pull $(IMAGE)
	@docker run -it --rm -v ${PWD}:/home/tester/done $(IMAGE)

SUBMIT_SCRIPT=../provided/submit.sh
submit1: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 1

submit2: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 2

submit:
	@printf 'what "make submit"??\nIt'\''s either "make submit1" or "make submit2"...\n'

libsid.so: sidlib.c
	$(CC) -fPIC -shared $(CPPFLAGS) $(CFLAGS) $(GTK_INCLUDE) $^ -o $@

libsid_demo.o: CFLAGS += $(GTK_INCLUDE)

libsid_demo: LDFLAGS += -L.
libsid_demo: LDLIBS += $(GTK_LIBS) -lsid
libsid_demo: libsid_demo.o libsid.so

alu.o: alu.c alu.h bit.h error.h
bit.o: bit.c bit.h error.h
bootrom.o: bootrom.c bootrom.h bus.h memory.h component.h gameboy.h cpu.h \
 alu.h bit.h timer.h cartridge.h error.h
bus.o: bus.c bus.h memory.h component.h error.h bit.h
cartridge.o: cartridge.c cartridge.h component.h memory.h bus.h error.h
component.o: component.c component.h memory.h error.h
cpu-alu.o: cpu-alu.c error.h bit.h alu.h cpu-alu.h opcode.h cpu.h bus.h \
 memory.h component.h cpu-storage.h util.h cpu-registers.h
cpu.o: cpu.c error.h opcode.h bit.h cpu.h alu.h bus.h memory.h \
 component.h cpu-alu.h cpu-registers.h cpu-storage.h util.h
cpu-registers.o: cpu-registers.c cpu-registers.h cpu.h alu.h bit.h bus.h \
 memory.h component.h error.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h cpu.h alu.h bus.h component.h util.h cpu-registers.h gameboy.h \
 timer.h cartridge.h
error.o: error.c
gameboy.o: gameboy.c gameboy.h cpu.h alu.h bit.h bus.h memory.h \
 component.h timer.h cartridge.h error.h bootrom.h
libsid_demo.o: libsid_demo.c sidlib.h
main.o: main.c cpu-storage.h memory.h opcode.h bit.h cpu.h alu.h bus.h \
 component.h util.h
memory.o: memory.c memory.h error.h
opcode.o: opcode.c opcode.h bit.h
sidlib.o: sidlib.c sidlib.h
test-cpu-week08.o: test-cpu-week08.c opcode.h bit.h cpu.h alu.h bus.h \
 memory.h component.h cpu-storage.h util.h error.h
test-gameboy.o: test-gameboy.c gameboy.h cpu.h alu.h bit.h bus.h memory.h \
 component.h timer.h cartridge.h util.h error.h
timer.o: timer.c timer.h component.h memory.h bit.h cpu.h alu.h bus.h \
 error.h cpu-storage.h opcode.h util.h
unit-test-alu.o: unit-test-alu.c tests.h error.h alu.h bit.h
unit-test-alu_ext.o: unit-test-alu_ext.c tests.h error.h alu.h bit.h \
 alu_ext.h
unit-test-bit.o: unit-test-bit.c tests.h error.h bit.h
unit-test-bus.o: unit-test-bus.c tests.h error.h bus.h memory.h \
 component.h util.h
unit-test-cartridge.o: unit-test-cartridge.c tests.h error.h cartridge.h \
 component.h memory.h bus.h cpu.h alu.h bit.h
unit-test-component.o: unit-test-component.c tests.h error.h bus.h \
 memory.h component.h
unit-test-cpu.o: unit-test-cpu.c tests.h error.h alu.h bit.h opcode.h \
 util.h cpu.h bus.h memory.h component.h cpu-registers.h cpu-storage.h \
 cpu-alu.h
unit-test-cpu-dispatch.o: unit-test-cpu-dispatch.c tests.h error.h alu.h \
 bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h
unit-test-cpu-dispatch-week08.o: unit-test-cpu-dispatch-week08.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h gameboy.h \
 timer.h cartridge.h util.h unit-test-cpu-dispatch.h cpu.c cpu-alu.h \
 cpu-registers.h cpu-storage.h
unit-test-cpu-dispatch-week09.o: unit-test-cpu-dispatch-week09.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu.c cpu-alu.h cpu-registers.h cpu-storage.h
unit-test-memory.o: unit-test-memory.c tests.h error.h bus.h memory.h \
 component.h
unit-test-timer.o: unit-test-timer.c util.h tests.h error.h timer.h \
 component.h memory.h bit.h cpu.h alu.h bus.h
util.o: util.c



unit-test-bit: unit-test-bit.o bit.o
unit-test-alu: unit-test-alu.o alu.o bit.o error.o
unit-test-component: unit-test-component.o error.o component.o memory.o
unit-test-memory: unit-test-memory.o error.o bus.o memory.o component.o bit.o
unit-test-bus: unit-test-bus.o error.o bus.o component.o bit.o memory.o
unit-test-cpu: unit-test-cpu.o alu.o bit.o error.o cpu.o cpu-registers.o cpu-storage.o cpu-alu.o bus.o component.o memory.o opcode.c
unit-test-cpu-dispatch-week08: unit-test-cpu-dispatch-week08.o error.o alu.o bit.o  bus.o memory.o component.o opcode.o gameboy.o cpu-alu.o cpu-registers.o cpu-storage.o timer.o cartridge.o bootrom.o
test-cpu-week08: test-cpu-week08.o opcode.o bit.o alu.o bus.o memory.o component.o cpu-storage.o error.o cpu-alu.o cpu.o cpu-registers.o
unit-test-cpu-dispatch-week09: unit-test-cpu-dispatch-week09.o error.o alu.o bit.o bus.o memory.o component.o opcode.o  cpu-alu.o cpu-registers.o cpu-storage.o
unit-test-timer: unit-test-timer.o error.o timer.o component.o memory.o bit.o alu.o bus.o cpu-storage.o cpu-registers.o cpu.o opcode.o cpu-alu.o
unit-test-cartridge: unit-test-cartridge.o error.o cartridge.o component.o component.o memory.o bus.o alu.o bit.o 
unit-test-cpu-dispatch: unit-test-cpu-dispatch.o cpu-storage.o cpu-registers.o cpu-alu.o opcode.o alu.o component.o memory.o bus.o bit.o error.o
	gcc -L . unit-test-cpu-dispatch.o cpu-storage.o cpu-registers.o cpu-alu.o opcode.o alu.o component.o memory.o bus.o bit.o error.o -lcs212gbcpuext  -lcheck -lm -lrt -pthread -lsubunit  -o unit-test-cpu-dispatch
unit-test-alu_ext: unit-test-alu_ext.o cpu-storage.o cpu-registers.o cpu-alu.o alu.o bus.o bit.o error.o
	gcc -L . unit-test-alu_ext.o cpu-storage.o cpu-registers.o cpu-alu.o alu.o bus.o bit.o error.o -lcs212gbcpuext -lcheck -lm -lrt -pthread -lsubunit -o unit-test-alu_ext
test-gameboy: LDFLAGS += -L.
test-gameboy: LDLIBS += -lcs212gbcpuext
test-gameboy: test-gameboy.o gameboy.o cpu.o alu.o bit.o bus.o memory.o component.o timer.o cartridge.o error.o bootrom.o cpu-storage.o cpu-registers.o cpu-alu.o opcode.o




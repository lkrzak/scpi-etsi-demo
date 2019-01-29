# To build this project you need GNU Make and GCC

# List all object files created from C .c sources:
COBJ = obj/main.o obj/libscpi/src/parser.o obj/libscpi/src/units.o obj/libscpi/src/error.o obj/libscpi/src/fifo.o obj/libscpi/src/expression.o obj/libscpi/src/ieee488.o obj/libscpi/src/lexer.o obj/libscpi/src/minimal.o obj/libscpi/src/utils.o obj/scpi_etsi_test/scpi_etsi_test.o
# All dependencies:
DEPS = $(COBJ:.o=.d)


# The 'make all' rule:
all: dirs scpi-etsi-demo.exe

dirs:
	@mkdir -p obj obj/libscpi/src obj/scpi_etsi_test


# Goal to compile .c source files into object files
$(COBJ) : obj/%.o : %.c
	@echo Compiling C: $<
	@gcc -c -fdiagnostics-show-option -Og -std=c99 -ggdb -g3 -ffunction-sections -fdata-sections -I. -Ilibscpi/inc -Iscpi_etsi_test -fdiagnostics-show-option -Og -std=c99 -ggdb -g3 -Wa,-ahlms=$(@:.o=.lst) -MMD -MF $(@:.o=.d) -Wno-attributes $< -o $@ 


# Goal to link .elf file from all object files and libraries
%.exe : $(COBJ)
	@echo Making elf file: $@
	@gcc $(COBJ) --output $@ -static -Wl,--gc-sections -Wl,-\(  -Wl,-\) -Wl,--gc-sections -Wl,-\(    -Wl,-\) 


clean:
	@echo Cleaning...
	@rm -f scpi-etsi-demo.exe
	@rm -f -r obj | exit 0
	@echo Cleaning done

# pull in dependency info for existing .o files:
-include $(DEPS)
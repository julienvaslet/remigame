librariesDirectory = ./lib
includesDirectory = ./include
sourcesDirectory = ./source
testsDirectory = ./tests
applicationName = remigame
debugMaxLevel = 3

windowsSDLConfig = /usr/i686-w64-mingw32/sys-root/mingw/bin/sdl2-config
windowsCompiler = i686-w64-mingw32-g++ -static-libgcc -static-libstdc++
windowsCompilerOptions = -Wall -I $(includesDirectory) -c `$(windowsSDLConfig) --cflags` $(debugFlag)
windowsLinker = i686-w64-mingw32-g++ -static-libgcc -static-libstdc++
windowsLinkerOptions = `$(windowsSDLConfig) --libs` -lopengl32 -lglu32
windowsLibraries = ./windowslib

compiler = g++
compilerOptions = -Wall -I $(includesDirectory) -c `sdl2-config --cflags` $(debugFlag)
linker = g++
linkerOptions = `sdl2-config --libs` -lGL -lGLU
libraries = $(librariesDirectory)/*.o

all: $(applicationName)

$(applicationName): $(applicationName).o
	$(linker) $(applicationName).o -o $(applicationName) `find $(librariesDirectory) -name '*.o' -type f` $(linkerOptions)

$(applicationName).o: libraries
	$(compiler) $(compilerOptions) main.cpp -o $(applicationName).o
	
libraries:
	@( for source in `cd $(sourcesDirectory); find . -name '*.cpp' -type f`; \
	do \
		mkdir -p $(librariesDirectory)/`dirname $$source` ; \
		if [ ! -e $(librariesDirectory)/$$source.o -o $(sourcesDirectory)/$$source -nt $(librariesDirectory)/$$source.o ] ; then \
			echo "$(compiler) $(compilerOptions) $(sourcesDirectory)/$$source -o $(librariesDirectory)/$$source.o" ; \
			$(compiler) $(compilerOptions) $(sourcesDirectory)/$$source -o $(librariesDirectory)/$$source.o ; \
			if [ $$? -eq 1 ] ; then \
				exit 1 ; \
			fi ; \
		fi ; \
	done )

run: all
	@( ./$(applicationName) $(applicationParams) )
	
test_%: libraries
	@( test="$@"; \
	test="./tests/$${test#test_}"; \
	if [ ! -e "$$test.cpp" ] ; then \
		echo "Test \"$$test.cpp\" does not exist." ; \
		exit 1 ; \
	else \
		rm -f $$test.o $$test ; \
		echo "Compilation of test: $$test" ; \
		$(compiler) $(compilerOptions) $$test.cpp -o $$test.o ; \
		if [ -e "$$test.o" ] ; then \
			$(linker) $$test.o -o $$test `find $(librariesDirectory) -name '*.o' -type f` $(linkerOptions) ; \
			if [ -e "$$test" ] ; then \
				echo "Running test: $$test" ; \
				$$test ; \
			else \
				echo "Link failed." ; \
			fi ; \
		else \
			echo "Compilation failed." ; \
		fi ; \
	fi )

tests: libraries
	@( for test in `find $(testsDirectory) -name '*.cpp' -type f`; \
	do \
		test=`basename "$${test%.*}"` ; \
		make --no-print-directory test_$$test ; \
	done )

debug: 
	@( make --no-print-directory debug0 )

debug%:
	@( target="$@"; \
	target=$${target#debug}; \
	for level in `seq $$target $(debugMaxLevel)`; \
	do \
		flags="-DDEBUG$$level $$flags"; \
	done ; \
	make --no-print-directory run debugFlag="$$flags -g" )

cleanlib:
	rm -rf $(librariesDirectory)/*
	rm -rf $(windowsLibraries)/*
	
clean:
	find . -name '*~' | xargs rm -f
	rm -f $(applicationName).o $(applicationName) $(applicationName).exe.o $(applicationName).exe

windows:
	@( make --no-print-directory compiler="$(windowsCompiler)" compilerOptions="$(windowsCompilerOptions)" linker="$(windowsLinker)" linkerOptions="$(windowsLinkerOptions)" librariesDirectory="$(windowsLibraries)" applicationName=$(applicationName).exe)
	

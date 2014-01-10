librariesDirectory = ./lib
includesDirectory = ./include
sourcesDirectory = ./source
applicationSuffix = 
applicationDirectory = ./apps
testsDirectory = ./tests
debugMaxLevel = 3
debugFlag = -g -DDEBUG0 -DDEBUG1 -DDEBUG2 -DDEBUG3


# windows options to test:  -MD -Os -s
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

all: applications

applications:
	@( for app in `find $(applicationDirectory) -name '*.cpp' -type f`; \
	do \
		app=`basename "$${app%.*}"` ; \
		make --no-print-directory app_$$app ; \
	done )

run_%: app_%
	@( ./$< )

app_%: $(applicationDirectory)/%.o$(applicationSuffix)
	$(linker) $< -o $@$(applicationSuffix) `find $(librariesDirectory) -name '*.o' -type f` $(linkerOptions)

$(applicationDirectory)/%.o$(applicationSuffix): $(applicationDirectory)/%.cpp libraries
	$(compiler) $(compilerOptions) $< -o $@
	
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
	rm -f $(applicationDirectory)/*.o
	@( for app in `find $(applicationDirectory) -name '*.cpp' -type f`; \
	do \
		app=app_`basename "$${app%.*}"` ; \
		echo "rm -f $${app}"; \
		rm -f $${app} ; \
		echo "rm -f $${app}.exe"; \
		rm -f $${app}.exe ; \
	done )

windows:
	@( make --no-print-directory applicationSuffix=".exe" compiler="$(windowsCompiler)" compilerOptions="$(windowsCompilerOptions)" linker="$(windowsLinker)" linkerOptions="$(windowsLinkerOptions)" librariesDirectory="$(windowsLibraries)" applicationName=$(applicationName).exe)


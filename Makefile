librariesDirectory = ./lib
includesDirectory = ./include
sourcesDirectory = ./source
applicationSuffix = 
applicationDirectory = ./apps
testsDirectory = ./tests
debugMaxLevel = 3
#debugFlag =
debugFlag = -g -DDEBUG0 -DDEBUG1 -DDEBUG2 -DDEBUG3
binDirectory = ./linux64

windowsSDLConfig32 = /usr/i686-w64-mingw32/sys-root/mingw/bin/sdl2-config
windowsCompiler32 = i686-w64-mingw32-g++ -static-libgcc -static-libstdc++ -MD -Os -s
windowsCompilerOptions32 = -Wall -I $(includesDirectory) -c `$(windowsSDLConfig32) --cflags` $(debugFlag)
windowsLinker32 = i686-w64-mingw32-g++ -static-libgcc -static-libstdc++
windowsLinkerOptions32 = `$(windowsSDLConfig32) --libs` -lopengl32 -lglu32 -lSDL2_image -MD -Os -s
windowsLibraries32 = ./windowslib32
windowsDirectory32 = ./windows32

windowsSDLConfig64 = /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config
windowsCompiler64 = x86_64-w64-mingw32-g++ -static-libgcc -static-libstdc++ -MD -Os -s
windowsCompilerOptions64 = -Wall -I $(includesDirectory) -c `$(windowsSDLConfig64) --cflags` $(debugFlag)
windowsLinker64 = x86_64-w64-mingw32-g++ -static-libgcc -static-libstdc++
windowsLinkerOptions64 = `$(windowsSDLConfig64) --libs` -lopengl32 -lglu32 -lSDL2_image -MD -Os -s
windowsLibraries64 = ./windowslib64
windowsDirectory64 = ./windows64

compiler = g++
compilerOptions = -Wall -I $(includesDirectory) -c `sdl2-config --cflags` $(debugFlag)
linker = g++
linkerOptions = `sdl2-config --libs` -lGL -lGLU -lSDL2_image
libraries = $(librariesDirectory)/*.o

all: applications

applications:
	@( for app in `find $(applicationDirectory) -name '*.cpp' -type f`; \
	do \
		app=`basename "$${app%.*}"` ; \
		make --no-print-directory $(binDirectory)/$$app ; \
	done )

run_%: $(binDirectory)/%
	@( app="$@"; \
	app="$${app#run_}" ; \
	$(binDirectory)/$${app} )

$(binDirectory)/%: $(binDirectory)/%.o
	$(linker) $< -o $@$(applicationSuffix) `find $(librariesDirectory) -name '*.o' -type f` $(linkerOptions)

$(binDirectory)/%.o: $(applicationDirectory)/%.cpp libraries
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
	rm -rf $(windowsLibraries32)/*
	rm -rf $(windowsLibraries64)/*
	
clean:
	find . -name '*~' | xargs rm -f
	rm -f $(binDirectory)/*.o
	rm -f $(windowsDirectory32)/*.{o,d}
	rm -f $(windowsDirectory64)/*.{o,d}
	@( for app in `find $(applicationDirectory) -name '*.cpp' -type f`; \
	do \
		app=`basename "$${app%.*}"` ; \
		echo "rm -f $(binDirectory)/$${app}"; \
		rm -f $(binDirectory)/$${app} ; \
		echo "rm -f $(windowsDirectory64)/$${app}_x86_64.exe"; \
		rm -f $(windowsDirectory64)/$${app}_x86_64.exe ; \
		echo "rm -f $(windowsDirectory32)/$${app}_i686.exe"; \
		rm -f $(windowsDirectory32)/$${app}_i686.exe ; \
	done )
	
windows: windows_x86_64 windows_i686

windows_x86_64:
	@( make --no-print-directory applicationSuffix="_x86_64.exe" binDirectory="$(windowsDirectory64)" compiler="$(windowsCompiler64)" compilerOptions="$(windowsCompilerOptions64)" linker="$(windowsLinker64)" linkerOptions="$(windowsLinkerOptions64)" librariesDirectory="$(windowsLibraries64)" applicationName="$(applicationName)_x86_64.exe")
	
windows_i686:
	@( make --no-print-directory applicationSuffix="_i686.exe" binDirectory="$(windowsDirectory32)" compiler="$(windowsCompiler32)" compilerOptions="$(windowsCompilerOptions32)" linker="$(windowsLinker32)" linkerOptions="$(windowsLinkerOptions32)" librariesDirectory="$(windowsLibraries32)" applicationName="$(applicationName)_i686.exe")


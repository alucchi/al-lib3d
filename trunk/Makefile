MAKEALIB = 1
CC   = g++
CLEAR= @clear
ECHO = @echo
RM   = @rm -f
STTY = @stty
TPUT = @tput

INTERFACES   = Application.h Ase.h Body.h converter.h Display.h Log.h Object.h Object_3DS.h Renderer.h TextureManager.h Maths/math3D.h Maths/Matrix4.h tinyxml/tinyxml.h tinyxml/tinystr.h
REALISATIONS = $(INTERFACES:.h=.cpp) main.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxmlparser.cpp
OBJECTS       = $(REALISATIONS:.cpp=.o)

CFLAG        = -D USE_SDL -O2 #-g
LDFLAG = -lSDLmain -lSDL
EXECUTABLE = lib3dGp2x
INCLUDE = -I . -I Maths -I tinyxml

%.o : %.cpp %.h defs.h
	$(ECHO) "Compiling $< -> $(<:.cpp=.o)"
	$(CC) $(INCLUDE) $(CFLAG) -c $< -o $(<:.cpp=.o)

$(EXECUTABLE) : $(OBJECTS)
	$(ECHO) "Linking"
	$(CC) $(LDFLAG) $(OBJECTS) -o $(EXECUTABLE) $(LIB)

all : $(EXECUTABLE)

clr :
	$(ECHO) "Cleaning..."
	$(RM) core
	$(RM) $(OBJECTS)
	$(RM) $(EXECUTABLE)
	$(ECHO) "Cleaning over"

clean : clr

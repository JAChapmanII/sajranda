SRCDIR=src
BINDIR=bin
SOURCES=$(wildcard $(SRCDIR)/*.cpp)
HEADERS=$(wildcard $(SRCDIR)/*.hpp)
OBJS=$(SOURCES:.cpp=.o)
EXEC=sajranda

CXXFLAGS=

LDFLAGS=
LDFLAGS+=-lsfml-graphics -lsfml-window -lsfml-system -lGL

ifdef profile
CXXFLAGS+=-pg
endif

ifndef nowall
CXXFLAGS+=-Wextra -pedantic -Wmain -Weffc++ -Wswitch-default -Wswitch-enum
CXXFLAGS+=-Wmissing-include-dirs -Wmissing-declarations -Wunreachable-code
CXXFLAGS+=-Winline -Wfloat-equal -Wundef -Wcast-align -Wredundant-decls
CXXFLAGS+=-Winit-self -Wshadow
endif

ifdef release
CXXFLAGS+=-O3 -s
else
CXXFLAGS+=-g
endif

all: $(EXEC)

full: $(EXEC)

$(EXEC): $(OBJS)
	mkdir -p $(BINDIR)
	$(CXX) -o $(BINDIR)/$(EXEC) $(LDFLAGS) $?

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

documentation:
	mkdir -p $(DOCDIR)
	$(DOXY) $(DCONFIG)

clean:
	rm -f $(BINDIR)/$(EXEC)
	rm -f $(SRCDIR)/*.o


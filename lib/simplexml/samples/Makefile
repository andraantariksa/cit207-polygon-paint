SHELL=/bin/sh



top_srcdir=..
srcdir=.
prefix=/usr/local
exec_prefix=${prefix}

CXX=c++
RM=rm
INCLUDES=-I${top_srcdir}/lib
CXXFLAGS=${INCLUDES}  -ggdb
LIBS=-L${top_srcdir}/lib -lsimplexml
LDFLAGS=
MAKEDEPEND=@MAKEDEPEND@

SRCS = simpletest.cc

OBJS = $(SRCS:%.cc=%.o)

PROG = $(SRCS:%.cc=%)


all: ${PROG}

shared: all

depend:
	${MAKEDEPEND} ${CXXFLAGS} ${SRCS}

install_shared: install

install:

clean:
	${RM} -f ${OBJS} $(PROG) *~ core *.bak

distclean: clean

${PROG}: ${OBJS}
	$(CXX) -o $@ $@.o $(LDFLAGS) $(LIBS) $(EXTRALIBS)
	strip $@


.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<


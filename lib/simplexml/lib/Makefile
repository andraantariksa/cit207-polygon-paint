lib_name = simplexml

CXX = c++
AR = ar
RANLIB = ranlib
RM = rm
INSTALL = /usr/local/bin/install -c

CXXFLAGS = -ggdb
LDFLAGS = -ggdb

STATIC_LIB=lib${lib_name}.a
SHARED_LIB=lib${lib_name}.so.0.1.0
SO_NAME=lib${lib_name}.so.0.1.0

SRCS = simplexml.cc
OBJS = $(SRCS:.cc=.o)


all: ${STATIC_LIB}

both: ${STATIC_LIB} ${SHARED_LIB}

shared: CXXFLAGS += -fPIC
shared: ${SHARED_LIB}

install: ${STATIC_LIB}
	${INSTALL_DATA} ${STATIC_LIB} ${lib_dir}

install_shared: ${SHARED_LIB}
	${INSTALL_DATA} ${SHARED_LIB} ${lib_dir}
	ldconfig

clean: 
	${RM} -f ${OBJS} ${STATIC_LIB} ${SHARED_LIB} lib$(lib_name).so* *~ core *.bak

distclean: clean

depend: ${SRCS}
	makedepend -- ${CXXFLAGS} -- ${SRCS}


${SHARED_LIB}: ${OBJS}
	$(CXX) -shared -Wl,-soname,${SO_NAME} -o ${SHARED_LIB} $(OBJS)
	ln -sf $(SHARED_LIB) lib$(lib_name).so.0.7
	ln -sf $(SHARED_LIB) lib$(lib_name).so.0
	ln -sf $(SHARED_LIB) lib$(lib_name).so


${STATIC_LIB}: ${OBJS}
	${AR} cr ${STATIC_LIB} ${OBJS}
	${RANLIB} ${STATIC_LIB}

.SUFFIX: .cc .o
.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

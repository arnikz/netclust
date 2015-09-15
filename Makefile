NET = net
NETI = netindex
NETC = netclust
OBJ_NETI = netindex_main.o netindex.o
OBJ_NETC = netclust_main.o netclust.o
INC_NETI = net.h netindex.h
INC_NETC = net.h netclust.h
VERSION = 1.0
TAR = $(NET)-$(VERSION)-src.tar

.PHONY : all
all : $(NETI) $(NETC)

$(NETI) : $(OBJ_NETI)

$(NETC) : $(OBJ_NETC)

$(OBJ_NETI) : $(INC_NETI)

$(OBJ_NETC) : $(INC_NETC)

.PHONY : clean
clean :
	-$(RM) $(NETI) $(OBJ_NETI) $(NETC) $(OBJ_NETC)

.PHONY : dist
dist :
	-$(MAKE) clean all
	-tar -cf $(TAR) $(NETI) $(NETC) $(NET)*.{h,c} README.md LICENSE Makefile
	-gzip $(TAR)

.PHONY : cleandist
cleandist :
	-$(RM) $(TAR).gz

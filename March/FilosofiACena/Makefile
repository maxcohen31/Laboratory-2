# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

CC              = gcc -std=c11
ifdef DEBUG
CFLAGS         += -Wall -g -fno-inline-functions -DDEBUG
else
CFLAGS	       += -Wall -O3 -DNDEBUG
endif
INCS            = -I . -I ./utils/includes
LIBS            = -pthread
SOURCES         = $(wildcard filosofi*.c)
TARGET          = $(SOURCES:.c=)

.PHONY: all clean cleanall 


%: %.c
	$(CC) $(INCS) $(CFLAGS) -o $@ $< $(LIBS)

all: $(TARGET)

filosofi-c: filosofi-c.c libmonitor.so
	$(CC) $(INCS) $(CFLAGS) -o $@ $< -Wl,-rpath,./ -L . -lmonitor $(LIBS)

libmonitor.so: monitor.c monitor.h
	$(CC) $(INCS) $(CFLAGS) -fPIC -c -o monitor.o $<
	$(CC) -shared monitor.o -o $@

clean: 
	-rm -fr *.o *~ *.so
cleanall: clean
	-rm -fr $(TARGET)

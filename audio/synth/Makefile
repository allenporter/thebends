SUBDIRS = oscillators modulation envelope controller
ECHO = echo
INSTALL_DIR = /usr/local

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS) libsynth.a

$(SUBDIRS):
	@$(ECHO) "Making $@ ..."
	@$(MAKE) -C $@

libsynth.a: $(SUBDIRS)
	@for dir in $(SUBDIRS); do \
	  ar xv $$dir/lib$$dir.a ; \
	done
	$(AR) rcv $@ *.o
	-rm *.o __*

install: libsynth.a
	cp libsynth.a /usr/local/lib/
	@for dir in $(SUBDIRS); do \
	  mkdir -p /usr/local/include/$$dir ; \
	  cp -rp $$dir/*.h /usr/local/include/$$dir/ ; \
	done

clean:
	-$(RM) *.o *.a __*
	@for dir in $(SUBDIRS); do \
	  $(ECHO) "Cleaning $$dir ..."; \
	  $(MAKE) -C $$dir clean; \
	done

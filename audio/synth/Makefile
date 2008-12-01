SUBDIRS = oscillators controller
ECHO = echo

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(ECHO) "Making $@ ..."
	@$(MAKE) -C $@

clean:
	@for dir in $(SUBDIRS); do \
	  $(ECHO) "Cleaning $$dir ..."; \
	  $(MAKE) -C $$dir clean; \
	done

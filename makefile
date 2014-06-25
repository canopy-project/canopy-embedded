
.PHONY: default
default:
	$(MAKE) -C cano
	$(MAKE) -C libcanopy

.PHONY: install
install:
	cp cano/cano /usr/bin
	cp libcanopy/libcanopy.so /usr/lib
	cp libcanopy/include/canopy.h /usr/include
	cp libcanopy/include/sddl.h /usr/include

.PHONY: distclean
distclean:
	rm /usr/bin/cano
	rm /usr/lib/libcanopy.so
	rm /usr/include/canopy.h
	rm /usr/include/sddl.h

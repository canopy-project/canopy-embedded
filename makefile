
.PHONY: default
default:
	$(MAKE) -C libcanopy

.PHONY: install
install:
	cp libcanopy/libcanopy.so /usr/lib
	cp libcanopy/include/canopy.h /usr/include
	cp libcanopy/include/sddl.h /usr/include
	if [ ! -f /etc/canopy/canopy.conf.json ]; then \
	    echo "Creating default config file."; \
	    mkdir -p /etc/canopy; \
	    cp libcanopy/resources/canopy.conf.default.json /etc/canopy/canopy.conf.json; \
	fi

.PHONY: distclean
distclean:
	rm -f /usr/bin/cano         # old name for executable
	rm -f /usr/lib/libcanopy.so
	rm -f /usr/include/canopy.h
	rm -f /usr/include/sddl.h
	rm -rf /etc/canopy

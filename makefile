
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
	if [ ! -f /etc/canopy/canopy.conf.json ]; then \
	    echo "Creating default config file."; \
	    mkdir -p /etc/canopy; \
	    cp libcanopy/resources/canopy.conf.default.json /etc/canopy/canopy.conf.json; \
	fi
	if [ ! -f /etc/canopy/uuid ]; then \
	    cano/cano uuid --install; \
	fi

.PHONY: distclean
distclean:
	rm /usr/bin/cano
	rm /usr/lib/libcanopy.so
	rm /usr/include/canopy.h
	rm /usr/include/sddl.h
	rm -r /etc/canopy

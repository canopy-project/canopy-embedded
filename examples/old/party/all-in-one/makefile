TARGETS := party_relay 
.PHONY: default
default: $(TARGETS)
party_relay: party_relay.c ../common/cano_rpi_gpio.c
	gcc $^ -g -lcanopy -lcurl -lwebsockets -o $@

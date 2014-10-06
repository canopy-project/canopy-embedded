TARGETS := sample_device_1 
.PHONY: default
default: $(TARGETS)
sample_device_1: sample_device_1.c
	gcc $< -g -lcanopy -lwebsockets -lcurl -o $@

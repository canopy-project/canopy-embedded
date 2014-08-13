TARGETS := fan 
.PHONY: default
default: $(TARGETS)
fan: fan.c
	gcc $< -g -lcanopy -lcurl -lwebsockets -o $@

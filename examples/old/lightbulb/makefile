TARGETS := lightbulb 
.PHONY: default
default: $(TARGETS)
lightbulb: lightbulb.c
	gcc $< -g -lcanopy -lcurl -lwebsockets -o $@

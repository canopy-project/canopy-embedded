SOURCE_FILES := \
        example_post_sample.c \
        ../../src/canopy.c \
        ../../3rdparty/libred/src/red_string.c \
        ../../3rdparty/libred/src/red_json.c \
        ../../3rdparty/libred/src/red_hash.c \
        ../../3rdparty/libred/src/red_log.c \
        ../../src/http/st_http_curl.c \
        ../../src/options/st_options.c \
        ../../src/websocket/st_websocket.c \


all:
	gcc -I../../src -I../../include -I../../3rdparty/libred/include -I../../3rdparty/libred/under_construction $(SOURCE_FILES) -lcurl -lwebsockets -Wall -Werror -g

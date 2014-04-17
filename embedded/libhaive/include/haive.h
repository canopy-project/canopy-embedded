/*
 * Copyright 2014 - Greg Prisament
 */

#ifndef HAIVE_INCLUDED
#define HAIVE_INCLUDED

typedef void * HaiveContext;
typedef void * HaiveReport;

HaiveContext haive_init();

bool haive_connect(const char *websocket_url);

bool haive_register_callback(HaiveContext ctx, HaiveCallbackEnum cb, HaiveCallbackRoutine fn, void *extra);

bool haive_load_device_description(HaiveContext haive, const char *filename);
bool haive_load_device_description_file(HaiveContext haive, FILE *file);
bool haive_load_device_description_string(HaiveContext haive, const char *szDesc);

HaiveReport haive_begin_report(HaiveContext haive);
bool haive_report_i8(HaiveReport report, const char *parameter, int8_t value);
bool haive_report_u8(HaiveReport report, const char *parameter, uint8_t value);
bool haive_report_i32(HaiveReport report, const char *parameter, int32_t value);
bool haive_report_u32(HaiveReport report, const char *parameter, uint32_t value);
bool haive_report_string(HaiveReport report, const char *parameter, const char *value);

bool haive_finish_report(HaiveReport report);
bool haive_send_report(HaiveReport report);

void haive_shutdown(HaiveContext haive);
#endif

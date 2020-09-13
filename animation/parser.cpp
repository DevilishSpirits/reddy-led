#include "parser.hpp"
#include <cstring>
#include <cmath>
#include <limits>

animation::parser::parser(void)
{
	yajl_parser = yajl_alloc(&callbacks,NULL,this);
}
animation::parser::~parser(void)
{
	yajl_free(yajl_parser);
}
std::string animation::parser::get_error(yajl_status status, const char *jsonText, size_t jsonTextLength)
{
	switch (status) {
		case yajl_status_ok:
			return std::string("Parsing success"); // <- If you see that there's a problem...
		case yajl_status_error: {
			unsigned char *error_str = yajl_get_error(yajl_parser,1,reinterpret_cast<const unsigned char*>(jsonText),jsonTextLength);
			parse_error = std::string(reinterpret_cast<const char*>(error_str));
			yajl_free_error(yajl_parser,error_str);
		}
		case yajl_status_client_canceled:
			return parse_error;
		default:
			return std::string("Unknow status");
	}
}

void animation::parser::reset_stop(void)
{
	memset(&current_stop,0,sizeof(current_stop));
}
void animation::parser::reset_init_state(void)
{
	memset(&current_state,0,sizeof(current_state));
}

int animation::parser::yajl_null(void *ctx)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	parser->parse_error = "Unexpected null";
	return 0;
}
int animation::parser::yajl_boolean(void *ctx, int boolVal)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	parser->parse_error = "Unexpected boolean";
	return 0;
}

int animation::parser::yajl_integer(void *ctx, long long integerVal)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	// Check if the integer fit in a uint16_t
	if (integerVal < 0) {
		parser->parse_error = "Negative values are not supported";
		return 0;
	} else if (integerVal > std::numeric_limits<uint16_t>::max()) {
		parser->parse_error = "s values are not supported";
		return 0;
	}
	// Dispatch
	switch (parser->current_key) {
		case KEY_NONE:
		case KEY_STOP:
			parser->current_state.current = integerVal;
			return 1;
		case KEY_NEXT_INDEX:
			parser->current_stop.next_index = integerVal;
			return 1;
		default:
			return animation::parser::yajl_double(ctx,integerVal);
	}
}
int animation::parser::yajl_double(void *ctx, double doubleVal)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	if (doubleVal < 0) {
		parser->parse_error = "Negative values are not supported";
		return 0;
	}
	switch (parser->current_key) {
		case KEY_RED:
			parser->current_stop.color.red = doubleVal;
			return 1;
		case KEY_GREEN:
			parser->current_stop.color.green = doubleVal;
			return 1;
		case KEY_BLUE:
			parser->current_stop.color.blue = doubleVal;
			return 1;
		case KEY_WHITE:
			parser->current_stop.color.white = doubleVal;
			return 1;
		case KEY_DURATION:
			parser->current_stop.duration = doubleVal*10;
			return 1;
		case KEY_NEXT_INDEX:
		case KEY_STOP: {
			double i;
			if (std::modf(doubleVal,&i)) {
				parser->parse_error = "\"next_index\" and \"stop\" only take integers";
				return 0;
			} else return yajl_integer(ctx,i);
		}
		default:
			parser->parse_error = "Unexpected number";
			return 0;
	}
}
int animation::parser::yajl_string(void *ctx, const unsigned char * stringVal, size_t stringLen)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	parser->parse_error = "Unexpected string";
	return 0;
}

int animation::parser::yajl_map_key(void *ctx, const unsigned char *key_u, size_t stringLen)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	// TODO Use a less naive way
	const char* key = reinterpret_cast<const char*>(key_u);
	if (parser->tree_state == TREE_ROOT_MAP) {
		// We are in the root, expect a "stops", "init_indexes" or "init_step" subkey
		if (!strncmp(key,"stops",stringLen))
			parser->current_key = KEY_STOPS;
		else if (!strncmp(key,"init",stringLen))
			parser->current_key = KEY_INIT;
		else parser->current_key = KEY_NONE;
	} else if (parser->tree_state == TREE_STOP_ELEMENT) {
		// We are in a stop element, expect a color component, "duration" or the "next_index" subkey
		if (!strncmp(key,"red",stringLen))
			parser->current_key = KEY_RED;
		else if (!strncmp(key,"green",stringLen))
			parser->current_key = KEY_GREEN;
		else if (!strncmp(key,"blue",stringLen))
			parser->current_key = KEY_BLUE;
		else if (!strncmp(key,"white",stringLen))
			parser->current_key = KEY_WHITE;
		else if (!strncmp(key,"duration",stringLen))
			parser->current_key = KEY_DURATION;
		else if (!strncmp(key,"next_index",stringLen))
			parser->current_key = KEY_NEXT_INDEX;
		else parser->current_key = KEY_NONE;
	} else if (parser->tree_state == TREE_INIT_ELEMENT) {
		// We are in a init element, expect the starting "stop" and "remaining" subkeys
		if (!strncmp(key,"stop",stringLen))
			parser->current_key = KEY_STOP;
		else parser->current_key = KEY_NONE;
	} else parser->current_key = KEY_NONE;
	return 1;
}
int animation::parser::yajl_start_map(void *ctx)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) {
		parser->skip_depth++;
		return 1;
	}
	switch (parser->current_key) {
		case KEY_NONE:
			if (parser->tree_state == TREE_TOP_LEVEL) 
				parser->tree_state = TREE_ROOT_MAP;
			else parser->skip_depth++; // Unknow key: skipping
			return 1;
		default:
			if (parser->tree_state == TREE_STOPS_ARRAY) {
				parser->tree_state = TREE_STOP_ELEMENT;
				return 1;
			} else if (parser->tree_state == TREE_INIT_ARRAY) {
				parser->tree_state = TREE_INIT_ELEMENT;
				return 1;
			} else {
				parser->parse_error = "Unexpected map";
				return 0;
			}
	}
}
int animation::parser::yajl_end_map(void *ctx)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) {
		parser->skip_depth--;
		return 1;
	}
	switch (parser->tree_state) {
		case TREE_STOP_ELEMENT:
			if (!parser->emit_stop(parser->current_stop))
				return 0;
			parser->reset_stop();
			parser->tree_state = TREE_STOPS_ARRAY;
			return 1;
		case TREE_INIT_ELEMENT:
			if (!parser->emit_init_state(parser->current_state))
				return 0;
			parser->reset_init_state();
			parser->tree_state = TREE_INIT_ARRAY;
			return 1;
		case TREE_ROOT_MAP:
			parser->tree_state = TREE_TOP_LEVEL;
			return 1;
		default:
			parser->parse_error = "Unexpected end-of-map";
			return 0;
	}
}
int animation::parser::yajl_start_array(void *ctx)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	if (parser->skip_depth) return 1;
	switch (parser->current_key) {
		case KEY_STOPS:
			parser->tree_state = TREE_STOPS_ARRAY;
			return 1;
		case KEY_INIT:
			parser->tree_state = TREE_INIT_ARRAY;
			return 1;
		default:
			parser->parse_error = "Unexpected array";
			return 0;
	}
}
int animation::parser::yajl_end_array(void *ctx)
{
	animation::parser *parser = static_cast<animation::parser*>(ctx);
	int ret_val;
	if (!parser->skip_depth) {
		parser->tree_state = TREE_ROOT_MAP;
	}
	return 1;
}

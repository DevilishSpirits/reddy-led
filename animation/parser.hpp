#pragma once
#include <yajl_parse.h>
#include <animation.hpp>
#include <string>
namespace animation {
	class parser {
		private:
			/** JSON parser handle
			 *
			 */
			yajl_handle yajl_parser;
			/** The current position in the tree
			 *
			 */
			enum {
				/** Root of the JSON
				 */
				TREE_TOP_LEVEL,
				/** The top-level JSON map
				 */
				TREE_ROOT_MAP,
				/** In the "stops" array
				 *
				 * But not in a stop element
				 */
				TREE_STOPS_ARRAY,
				/** An element of the "stops" array
				 */
				TREE_STOP_ELEMENT,
				/** In the "init_indexes" array
				 */
				TREE_INIT_INDEXES,
			} tree_state = TREE_TOP_LEVEL;
			/** Last key name
			 *
			 * This variable store the current key
			 */
			enum {
				/** No or unknown key
				 */
				KEY_NONE,
				/** "stops"
				 */
				KEY_STOPS,
				/** "init_indexes"
				 */
				KEY_INIT_INDEXES,
				/** "red"
				 */
				KEY_RED,
				/** "green"
				 */
				KEY_GREEN,
				/** "blue"
				 */
				KEY_BLUE,
				/** "white"
				 */
				KEY_WHITE,
				/** "duration"
				 */
				KEY_DURATION,
				/** "next_index"
				 */
				KEY_NEXT_INDEX,
			} current_key;
			/** Depth in the skipped element
			 *
			 * This value is used to easily skip unknow keys.
			 */
			unsigned short skip_depth = 0;
			static int yajl_null(void *ctx);
			static int yajl_boolean(void *ctx, int boolVal);
			static int yajl_integer(void *ctx, long long integerVal);
			static int yajl_double (void *ctx, double doubleVal);
			static int yajl_string (void *ctx, const unsigned char * stringVal, size_t stringLen);
			
			static int yajl_start_map(void *ctx);
			static int yajl_map_key(void *ctx, const unsigned char * key, size_t stringLen);
			static int yajl_end_map(void *ctx);
			static int yajl_start_array(void *ctx);
			static int yajl_end_array(void *ctx);
			const yajl_callbacks callbacks = {yajl_null,yajl_boolean,yajl_integer,yajl_double,NULL,yajl_string,yajl_start_map,yajl_map_key,yajl_end_map,yajl_start_array,yajl_end_array};
			
			animation::stop<animation::basic_rgbw<double>> current_stop;
			
			/** Reset current_stop
			 *
			 * Clear current_stop member to defaults values (all zeros)
			 */
			void reset_stop(void);
		protected:
			/** Parse error string
			 */
			std::string parse_error;
			
			virtual int emit_initial_index(uint16_t value) = 0;
			virtual int emit_stop(animation::stop<animation::basic_rgbw<double>> &stop) = 0;
		public:
			parser(void);
			~parser(void);
			/** Parse some datas
			 */
			yajl_status parse(const char *jsonText, size_t jsonTextLength) {
				return yajl_parse(yajl_parser,reinterpret_cast<const unsigned char*>(jsonText),jsonTextLength);
			}
			/** End-parsing
			 *
			 * This function must be called because YAJL may still have unprocessed
			 * datas in buffers because it can't determine the exact type.
			 */
			yajl_status complete_parse(void) {
				return yajl_complete_parse(yajl_parser);
			}
			
			/** Retrieve error string
			 * \param status The result of the parse() or complete_parse() call
			 */
			std::string get_error(yajl_status status, const char *jsonText, size_t jsonTextLength);
	};
}

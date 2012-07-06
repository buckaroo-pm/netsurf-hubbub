#include <stddef.h>
#include "hubbub/parser.h"
#include <proto/hubbub.h>

	/* hubbub/errors.h */
	const char *hubbub_error_to_string(hubbub_error error)
	{
		return IHubbub->hubbub_error_to_string(error);
	}

	/* hubbub/hubbub.h */
	hubbub_error hubbub_initialise(const char *aliases_file,
		hubbub_allocator_fn alloc, void *pw)
	{
		return HUBBUB_OK;
	}

	hubbub_error hubbub_finalise(hubbub_allocator_fn alloc, void *pw)
	{
		return HUBBUB_OK;
	}

	/* hubbub/parser.h */
	hubbub_error hubbub_parser_create(const char *enc, bool fix_enc,
		hubbub_allocator_fn alloc, void *pw, hubbub_parser **parser)
	{
		return IHubbub->hubbub_parser_create(enc, fix_enc, alloc, pw, parser);
	}

	hubbub_error hubbub_parser_destroy(hubbub_parser *parser)
	{
		return IHubbub->hubbub_parser_destroy(parser);
	}

	hubbub_error hubbub_parser_setopt(hubbub_parser *parser,
		hubbub_parser_opttype type, hubbub_parser_optparams *params)
	{
		return IHubbub->hubbub_parser_setopt(parser, type, params);
	}

	hubbub_error hubbub_parser_parse_chunk(hubbub_parser *parser,
		const uint8_t *data, size_t len)
	{
		return IHubbub->hubbub_parser_parse_chunk(parser, data, len);
	}

	hubbub_error hubbub_parser_completed(hubbub_parser *parser)
	{
		return IHubbub->hubbub_parser_completed(parser);
	}

	const char *hubbub_parser_read_charset(hubbub_parser *parser,
		hubbub_charset_source *source)
	{
		return IHubbub->hubbub_parser_read_charset(parser, source);
	}

	hubbub_error hubbub_parser_insert_chunk(hubbub_parser *parser,
		const uint8_t *data, size_t len)
	{
		return IHubbub->hubbub_parser_insert_chunk(parser, data, len);
	}

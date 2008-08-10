/*
 * This file is part of Hubbub.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2008 Andrew Sidwell <takkaria@netsurf-browser.org>
 */

#include <assert.h>
#include <string.h>

#include "treebuilder/modes.h"
#include "treebuilder/internal.h"
#include "treebuilder/treebuilder.h"
#include "utils/utils.h"


/**
 * Handle token in "after after frameset" insertion mode
 *
 * \param treebuilder  The treebuilder instance
 * \param token        The token to handle
 * \return True to reprocess token, false otherwise
 */
hubbub_error handle_after_after_frameset(hubbub_treebuilder *treebuilder,
		const hubbub_token *token)
{
	hubbub_error err = HUBBUB_OK;

	switch (token->type) {
	case HUBBUB_TOKEN_CHARACTER:
		if (process_characters_expect_whitespace(treebuilder,
				token, true)) {
			treebuilder->context.mode = IN_FRAMESET;
			err = HUBBUB_REPROCESS;
		}
		break;
	case HUBBUB_TOKEN_COMMENT:
		process_comment_append(treebuilder, token,
				treebuilder->context.document);
		break;
	case HUBBUB_TOKEN_DOCTYPE:
		/** \todo parse error */
		/* ignore token */
		break;
	case HUBBUB_TOKEN_START_TAG:
	{
		element_type type = element_type_from_name(treebuilder,
				&token->data.tag.name);

		if (type == HTML) {
			/* Process as if "in body" */
			handle_in_body(treebuilder, token);
		} else {
			/** \todo parse error */
			treebuilder->context.mode = IN_FRAMESET;
			err = HUBBUB_REPROCESS;
		}
	}
		break;
	case HUBBUB_TOKEN_END_TAG:
		/** \todo parse error */
		treebuilder->context.mode = IN_FRAMESET;
		err = HUBBUB_REPROCESS;
		break;
	case HUBBUB_TOKEN_EOF:
		break;
	}

	return err;
}


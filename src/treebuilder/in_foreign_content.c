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
#include "utils/string.h"


/*** Attribute-correction stuff ***/

#define S(s)		s, SLEN(s)

/**
 * UTF-8 encoding of U+FFFD REPLACEMENT CHARACTER
 */
static const uint8_t u_fffd[3] = { '\xEF', '\xBF', '\xBD' };
static const hubbub_string u_fffd_str = { u_fffd, sizeof(u_fffd) };

/**
 * Mapping table for case changes
 */
typedef struct
{
	const char *attr;	/**< Lower case attribute name */
	size_t len;		/**< Length of name in bytes */
	const char *proper;	/**< Correctly cased version */
} case_changes;

static const case_changes svg_attributes[] = {
	{ S("attributename"),		"attributeName" },
	{ S("attributetype"),		"attributeType" },
	{ S("basefrequency"),		"baseFrequency" },
	{ S("baseprofile"),		"baseProfile" },
	{ S("calcmode"),		"calcMode" },
	{ S("clippathunits"),		"clipPathUnits" },
	{ S("contentscripttype"),	"contentScriptType" },
	{ S("contentstyletype"),	"contentStyleType" },
	{ S("diffuseconstant"),		"diffuseConstant" },
	{ S("edgemode"),		"edgeMode" },
	{ S("externalresourcesrequired"),"externalResourcesRequired" },
	{ S("filterres"),		"filterRes" },
	{ S("filterunits"),		"filterUnits" },
	{ S("glyphref"),		"glyphRef" },
	{ S("gradienttransform"),	"gradientTransform" },
	{ S("gradientunits"),		"gradientUnits" },
	{ S("kernelmatrix"),		"kernelMatrix" },
	{ S("kernelunitlength"),	"kernelUnitLength" },
	{ S("keypoints"),		"keyPoints" },
	{ S("keysplines"),		"keySplines" },
	{ S("keytimes"),		"keyTimes" },
	{ S("lengthadjust"),		"lengthAdjust" },
	{ S("limitingconeangle"),	"limitingConeAngle" },
	{ S("markerheight"),		"markerHeight" },
	{ S("markerunits"),		"markerUnits" },
	{ S("markerwidth"),		"markerWidth" },
	{ S("maskcontentunits"),	"maskContentUnits" },
	{ S("maskunits"),		"maskUnits" },
	{ S("numoctaves"),		"numOctaves" },
	{ S("pathlength"),		"pathLength" },
	{ S("patterncontentunits"),	"patternContentUnits" },
	{ S("patterntransform"),	"patternTransform" },
	{ S("patternunits"),		"patternUnits" },
	{ S("pointsatx"),		"pointsAtX" },
	{ S("pointsaty"),		"pointsAtY" },
	{ S("pointsatz"),		"pointsAtZ" },
	{ S("preservealpha"),		"preserveAlpha" },
	{ S("preserveaspectratio"),	"preserveAspectRatio" },
	{ S("primitiveunits"),		"primitiveUnits" },
	{ S("refx"),			"refX" },
	{ S("refy"),			"refY" },
	{ S("repeatcount"),		"repeatCount" },
	{ S("repeatdur"),		"repeatDur" },
	{ S("requiredextensions"),	"requiredExtensions" },
	{ S("requiredfeatures"),	"requiredFeatures" },
	{ S("specularconstant"),	"specularConstant" },
	{ S("specularexponent"),	"specularExponent" },
	{ S("spreadmethod"),		"spreadMethod" },
	{ S("startoffset"),		"startOffset" },
	{ S("stddeviation"),		"stdDeviation" },
	{ S("stitchtiles"),		"stitchTiles" },
	{ S("surfacescale"),		"surfaceScale" },
	{ S("systemlanguage"),		"systemLanguage" },
	{ S("tablevalues"),		"tableValues" },
	{ S("targetx"),			"targetX" },
	{ S("targety"),			"targetY" },
	{ S("textlength"),		"textLength" },
	{ S("viewbox"),			"viewBox" },
	{ S("viewtarget"),		"viewTarget" },
	{ S("xchannelselector"),	"xChannelSelector" },
	{ S("ychannelselector"),	"yChannelSelector" },
	{ S("zoomandpan"),		"zoomAndPan" },
};

static const case_changes svg_tagnames[] = {
	{ S("altglyph"),		"altGlyph" },
	{ S("altglyphdef"),		"altGlyphDef" },
	{ S("altglyphitem"),		"altGlyphItem" },
	{ S("animatecolor"),		"animateColor" },
	{ S("animatemotion"),		"animateMotion" },
	{ S("animatetransform"),	"animateTransform" },
	{ S("clippath"),		"clipPath" },
	{ S("feblend"),			"feBlend" },
	{ S("fecolormatrix"),		"feColorMatrix" },
	{ S("fecomponenttransfer"),	"feComponentTransfer" },
	{ S("fecomposite"),		"feComposite" },
	{ S("feconvolvematrix"),	"feConvolveMatrix" },
	{ S("fediffuselighting"),	"feDiffuseLighting" },
	{ S("fedisplacementmap"),	"feDisplacementMap" },
	{ S("fedistantlight"),		"feDistantLight" },
	{ S("fedropshadow"),		"feDropShadow"},
	{ S("feflood"),			"feFlood" },
	{ S("fefunca"),			"feFuncA" },
	{ S("fefuncb"),			"feFuncB" },
	{ S("fefuncg"),			"feFuncG" },
	{ S("fefuncr"),			"feFuncR" },
	{ S("fegaussianblur"),		"feGaussianBlur" },
	{ S("feimage"),			"feImage" },
	{ S("femerge"),			"feMerge" },
	{ S("femergenode"),		"feMergeNode" },
	{ S("femorphology"),		"feMorphology" },
	{ S("feoffset"),		"feOffset" },
	{ S("fepointlight"),		"fePointLight" },
	{ S("fespecularlighting"),	"feSpecularLighting" },
	{ S("fespotlight"),		"feSpotLight" },
	{ S("fetile"),			"feTile" },
	{ S("feturbulence"),		"feTurbulence" },
	{ S("foreignobject"),		"foreignObject" },
	{ S("glyphref"),		"glyphRef" },
	{ S("lineargradient"),		"linearGradient" },
	{ S("radialgradient"),		"radialGradient" },
	{ S("textpath"),		"textPath" },
};

#undef S

/**
 * Adjust MathML attributes
 *
 * \param treebuilder  Treebuilder instance
 * \param tag          Tag to adjust the attributes of
 */
void adjust_mathml_attributes(hubbub_treebuilder *treebuilder,
		hubbub_tag *tag)
{
	size_t i;
	UNUSED(treebuilder);

	for (i = 0; i < tag->n_attributes; i++) {
		hubbub_attribute *attr = &tag->attributes[i];
		const uint8_t *name = attr->name.ptr;
		size_t len = attr->name.len;

		if (hubbub_string_match(name, len, 
				(const uint8_t *) "definitionurl", 
				SLEN("definitionurl"))) {
			attr->name.ptr = (uint8_t *) "definitionURL";
		}
	}
}

/**
 * Adjust SVG attributes.
 *
 * \param treebuilder	Treebuilder instance
 * \param tag		Tag to adjust the attributes of
 */
void adjust_svg_attributes(hubbub_treebuilder *treebuilder,
		hubbub_tag *tag)
{
	size_t i;
	UNUSED(treebuilder);

	for (i = 0; i < tag->n_attributes; i++) {
		hubbub_attribute *attr = &tag->attributes[i];

		const uint8_t *name = attr->name.ptr;
		size_t len = attr->name.len;
		size_t j;

		for (j = 0; j < N_ELEMENTS(svg_attributes); j++) {
			if (hubbub_string_match(name, len,
					(uint8_t *) svg_attributes[j].attr,
					svg_attributes[j].len)) {
				attr->name.ptr =
					(uint8_t *) svg_attributes[j].proper;
			}
		}
	}
}

/**
 * Adjust SVG tagnmes.
 *
 * \param treebuilder	Treebuilder instance
 * \param tag		Tag to adjust the name of
 */
void adjust_svg_tagname(hubbub_treebuilder *treebuilder,
		hubbub_tag *tag)
{
	const uint8_t *name = tag->name.ptr;
	size_t len = tag->name.len;
	size_t i;

	UNUSED(treebuilder);

	for (i = 0; i < N_ELEMENTS(svg_tagnames); i++) {
		if (hubbub_string_match(name, len,
				(uint8_t *) svg_tagnames[i].attr,
				svg_tagnames[i].len)) {
			tag->name.ptr =	(uint8_t *) svg_tagnames[i].proper;
		}
	}
}



#define S(s)		(uint8_t *) s, SLEN(s)

/**
 * Adjust foreign attributes.
 *
 * \param treebuilder	Treebuilder instance
 * \param tag		Tag to adjust the attributes of
 */
void adjust_foreign_attributes(hubbub_treebuilder *treebuilder,
		hubbub_tag *tag)
{
	size_t i;
	UNUSED(treebuilder);

	for (i = 0; i < tag->n_attributes; i++) {
		hubbub_attribute *attr = &tag->attributes[i];
		const uint8_t *name = attr->name.ptr;

		/* 10 == strlen("xlink:href") */
		if (attr->name.len >= 10 &&
				strncmp((char *) name, "xlink:", 
						SLEN("xlink:")) == 0) {
			size_t len = attr->name.len - 6;
			name += 6;

			if (hubbub_string_match(name, len, S("actuate")) ||
					hubbub_string_match(name, len,
							S("arcrole")) ||
					hubbub_string_match(name, len,
							S("href")) ||
					hubbub_string_match(name, len,
							S("role")) ||
					hubbub_string_match(name, len,
							S("show")) ||
					hubbub_string_match(name, len,
							S("title")) ||
					hubbub_string_match(name, len,
							S("type"))) {
				attr->ns = HUBBUB_NS_XLINK;
				attr->name.ptr += 6;
				attr->name.len -= 6;
			}
		/* 8 == strlen("xml:base") */
		} else if (attr->name.len >= 8 &&
				strncmp((char *) name, "xml:", 
						SLEN("xml:")) == 0) {
			size_t len = attr->name.len - 4;
			name += 4;

			if (hubbub_string_match(name, len, S("base")) ||
					hubbub_string_match(name, len,
							S("lang")) ||
					hubbub_string_match(name, len,
							S("space"))) {
				attr->ns = HUBBUB_NS_XML;
				attr->name.ptr += 4;
				attr->name.len -= 4;
			}
		} else if (hubbub_string_match(name, attr->name.len,
						S("xmlns"))) {
			attr->ns = HUBBUB_NS_XMLNS;
		} else if (hubbub_string_match(name, attr->name.len,
						S("xmlns:xlink"))) {
			attr->ns = HUBBUB_NS_XMLNS;
			attr->name.ptr += 6;
			attr->name.len -= 6;
		}

	}
}


#undef S


/*** Foreign content insertion mode ***/


/**
 * Returns true iff there is an element in scope that has a namespace other
 * than the HTML namespace.
 */
static bool element_in_scope_in_non_html_ns(hubbub_treebuilder *treebuilder)
{
	element_context *stack = treebuilder->context.element_stack;
	uint32_t node;

	assert((signed) treebuilder->context.current_node >= 0);

	for (node = treebuilder->context.current_node; node > 0; node--) {
		if (stack[node].ns != HUBBUB_NS_HTML)
			return true;
	}

	return false;
}

/**
 * Process a token as if in the secondary insertion mode.
 */
static hubbub_error process_as_in_secondary(hubbub_treebuilder *treebuilder,
		const hubbub_token *token)
{
	hubbub_error err;

	/* Because we don't support calling insertion modes directly,
	 * instead we set the current mode to the secondary mode,
	 * call the token handler, and then reset the mode afterward
	 * as long as it's unchanged, as this has the same effect */

	treebuilder->context.mode = treebuilder->context.second_mode;

	err = hubbub_treebuilder_token_handler(token, treebuilder);
	if (err != HUBBUB_OK) {
		treebuilder->context.mode = IN_FOREIGN_CONTENT;
		return err;
	}

	if (treebuilder->context.mode == treebuilder->context.second_mode)
		treebuilder->context.mode = IN_FOREIGN_CONTENT;

	if (treebuilder->context.mode == IN_FOREIGN_CONTENT &&
			!element_in_scope_in_non_html_ns(treebuilder)) {
		treebuilder->context.mode = treebuilder->context.second_mode;
	}

	return HUBBUB_OK;
}

/**
 * Handle certain start tags. Reprocessing is a must
 * after calling this function.
 */
static void candidate_foreign_breakout(hubbub_treebuilder *treebuilder)
{
	element_context *stack = treebuilder->context.element_stack;
	hubbub_ns ns;
	element_type type;
	void *node;

	/** \todo parse error */
	/** todo fragment case */

	element_stack_pop(treebuilder, &ns, &type, &node);
		treebuilder->tree_handler->unref_node(
				treebuilder->tree_handler->ctx,
				node);

	ns = stack[treebuilder->context.current_node].ns;
	type = current_node(treebuilder);
	size_t n_attrs = stack[
		treebuilder->context.current_node].n_attributes;
	hubbub_attribute *attrs =
		stack[treebuilder->context.current_node].
		attributes;
	while (ns != HUBBUB_NS_HTML &&
			!is_mathml_text_integration(type, ns) &&
			!is_html_integration(type, ns, attrs,
				n_attrs)
			) {

		element_stack_pop(treebuilder, &ns, &type, &node);
		treebuilder->tree_handler->unref_node(
				treebuilder->tree_handler->ctx,
				node);

		type = current_node(treebuilder);
		ns = stack[treebuilder->context.current_node].ns;
		n_attrs = stack[
			treebuilder->context.current_node].n_attributes;
		attrs =	stack[treebuilder->context.current_node].
			attributes;
	}
}


/**
 * Handle tokens in "in foreign content" insertion mode
 *
 * \param treebuilder  The treebuilder instance
 * \param token        The token to process
 * \return True to reprocess the token, false otherwise
 */
hubbub_error handle_in_foreign_content(hubbub_treebuilder *treebuilder,
		const hubbub_token *token)
{
	hubbub_ns cur_node_ns = treebuilder->context.element_stack[
			treebuilder->context.current_node].ns;

	element_type cur_node = current_node(treebuilder);
	hubbub_attribute *cur_node_attrs;
	size_t cur_node_n_attrs;

	cur_node_attrs = treebuilder->context.element_stack[
		treebuilder->context.current_node].attributes;

	cur_node_n_attrs = treebuilder->context.element_stack[
		treebuilder->context.current_node].n_attributes;

	if (treebuilder->context.current_node == 0 ||
				cur_node_ns == HUBBUB_NS_HTML) {
		return process_as_in_secondary(treebuilder, token);
	}

	hubbub_error err = HUBBUB_OK;
	const uint8_t *c;
	switch (token->type) {
	case HUBBUB_TOKEN_CHARACTER:
	{
		if(is_html_integration(cur_node, cur_node_ns,
					cur_node_attrs, cur_node_n_attrs) ||
				is_mathml_text_integration(cur_node, cur_node_ns)) {
			return process_as_in_secondary(treebuilder, token);
		}
		c = (token->data.character.ptr);
		if(c[0] == '\0') {
			err = append_text(treebuilder, &u_fffd_str);
			return err;
		} else if(*c != '\t' && *c != '\r' && *c != ' ' && *c != '\n' && *c != '\f') {
			treebuilder->context.frameset_ok = false;
		}

		err = append_text(treebuilder, &token->data.character);
	}
		break;
	case HUBBUB_TOKEN_COMMENT:
		err = process_comment_append(treebuilder, token,
				treebuilder->context.element_stack[
				treebuilder->context.current_node].node);
		break;
	case HUBBUB_TOKEN_DOCTYPE:
		/** \todo parse error */
		break;
	case HUBBUB_TOKEN_START_TAG:
	{
		bool handled = false;
		element_type type = element_type_from_name(treebuilder,
				&token->data.tag.name);

		if ((is_mathml_text_integration(cur_node, cur_node_ns) &&
					type != MGLYPH && type != MALIGNMARK) ||
				(type == SVG && cur_node_ns == HUBBUB_NS_MATHML &&
							cur_node == ANNOTATION_XML) ||
				is_html_integration(cur_node, cur_node_ns,
						     cur_node_attrs, cur_node_n_attrs)) {
			return process_as_in_secondary(treebuilder, token);
		} else if (type == B || type ==  BIG || type == BLOCKQUOTE ||
				type == BODY || type == BR || type == CENTER ||
				type == CODE || type == DD || type == DIV ||
				type == DL || type == DT || type == EM ||
				type == EMBED || type == H1 || type == H2 || 
				type == H3 || type == H4 || type == H5 || 
				type == H6 || type == HEAD || type == HR || 
				type == I || type == IMG || type == LI || 
				type == LISTING || type == MENU || 
				type == META || type == NOBR || type == OL || 
				type == P || type == PRE || type == RUBY || 
				type == S || type == SMALL || type == SPAN || 
				type == STRONG || type == STRIKE || 
				type == SUB || type == SUP || type == TABLE || 
				type == TT || type == U || type == UL || 
				type == VAR) {
			candidate_foreign_breakout(treebuilder);
			err = HUBBUB_REPROCESS;
			handled = true;
		} else if (type == FONT) {
			const hubbub_tag *tag = &token->data.tag;
			size_t i;
			bool found = false;

			for (i = 0; i < tag->n_attributes; i++) {
				hubbub_attribute *attr = &tag->attributes[i];
				const uint8_t *name = attr->name.ptr;
				size_t len = attr->name.len;

				if (hubbub_string_match(name, len, 
						(const uint8_t *) "color", 
						SLEN("color")) ||
					hubbub_string_match(name, len,
						(const uint8_t *) "face",
						SLEN("face")) ||
					hubbub_string_match(name, len,
						(const uint8_t *) "size",
						SLEN("size"))) {
					found = true;
					break;
				}
			}

			if (found) {
				candidate_foreign_breakout(treebuilder);
				err = HUBBUB_REPROCESS;
				handled = true;
			}
		}
		if (!handled) {
			hubbub_tag tag = token->data.tag;

			if (cur_node_ns == HUBBUB_NS_SVG) {
				adjust_svg_tagname(treebuilder, &tag);
				adjust_svg_attributes(treebuilder, &tag);
			} else if(cur_node_ns == HUBBUB_NS_MATHML) {
				adjust_mathml_attributes(treebuilder, &tag);
			}

			adjust_foreign_attributes(treebuilder, &tag);

			/* Set to the right namespace and insert */
			tag.ns = cur_node_ns;

			if (token->data.tag.self_closing) {
				err = insert_element(treebuilder, &tag, false);
				/** \todo ack sc flag */
			} else {
				err = insert_element(treebuilder, &tag, true);
			}
		}
	}
		break;
	case HUBBUB_TOKEN_END_TAG:
	{
		element_type type = element_type_from_name(treebuilder,
				&token->data.tag.name);
		uint32_t node;
		element_context *stack = treebuilder->context.element_stack;

		for (node = treebuilder->context.current_node; node > 0;) {
			if(stack[node].type == type) {
				hubbub_ns ns;
				element_type type;
				void *node_iterator;
				void *vnode = stack[node].node;
				do{
					element_stack_pop(treebuilder, &ns, &type, &node_iterator);
					treebuilder->tree_handler->unref_node(
							treebuilder->tree_handler->ctx,
							node_iterator);
				} while(node_iterator != vnode);
				return HUBBUB_OK;
			}
			if(stack[--node].ns == HUBBUB_NS_HTML) {
				break;
			}
		}
		if(node > 0) {
			err = process_as_in_secondary(treebuilder, token);
		}
	}
	break;
	case HUBBUB_TOKEN_EOF:
		candidate_foreign_breakout(treebuilder);
		err = HUBBUB_REPROCESS;
		break;
	}

	return err;
}

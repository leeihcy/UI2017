#pragma once
#include <usp10.h>
#pragma comment(lib, "usp10.lib")

namespace UI
{

bool ScriptItemizeWrap(
	const wchar_t* text, int textlen,
	std::vector<SCRIPT_ITEM>* items);

bool ScriptShapeWrap(
	const wchar_t* input, int input_length,       // IN: characters
	HFONT hfont, SCRIPT_CACHE* script_cache,// IN: font info
	SCRIPT_ANALYSIS* analysis,              // IN: from ScriptItemize
	std::vector<WORD>* logs,                // OUT: one per character
	std::vector<WORD>* glyphs,              // OUT: one per glyph
	std::vector<SCRIPT_VISATTR>* visattr);  // OUT: one per glyph
}
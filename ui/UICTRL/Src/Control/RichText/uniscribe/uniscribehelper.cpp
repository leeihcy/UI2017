#include "stdafx.h"
#include "uniscribehelper.h"

namespace UI
{

// Fills the given items array with the items for the input text.
// Returns true on success.
bool ScriptItemizeWrap(
	const wchar_t* text, 
	int textlen,
	std::vector<SCRIPT_ITEM>* items
)
{
	// Most applications won¡¯t need to set any control flags. 
	SCRIPT_CONTROL control;
	ZeroMemory(&control, sizeof(SCRIPT_CONTROL));

	// Initial state, you will probably want to keep this updated as you process
	// runs in order so that you can always give it the correct direction of the
	// surrounding text.
	SCRIPT_STATE state;
	ZeroMemory(&state, sizeof(SCRIPT_STATE));
	state.uBidiLevel = 0;  // 0 means that the surrounding text is left-to-right.

	int max_items = 16;
	while (true) 
	{
		// Make enough room for the output.
		items->resize(max_items);

		// We subtract one from max_items to work around a buffer overflow on some
		// older versions of Windows.
		int generated_items = 0;
		HRESULT hr = ScriptItemize(text, textlen, max_items - 1, &control,
			&state, &(*items)[0], &generated_items);

		if (SUCCEEDED(hr)) 
		{
			// It generated some items, so resize the array. Note that we add
			// one to account for the magic last item.
			items->resize(generated_items + 1);
			return true;
		}
		if (hr != E_OUTOFMEMORY)
		{
			// Some kind of error.
			return false;
		}

		// The input array isn't big enough, double and loop again.
		max_items *= 2;
	}

	return false;
}

// Called with the array output by callScriptItemize, this will 
bool ScriptShapeWrap(
	const wchar_t* input, int input_length,       // IN: characters
	HFONT hfont, SCRIPT_CACHE* script_cache,// IN: font info
	SCRIPT_ANALYSIS* analysis,              // IN: from ScriptItemize
	std::vector<WORD>* logs,                // OUT: one per character
	std::vector<WORD>* glyphs,              // OUT: one per glyph
	std::vector<SCRIPT_VISATTR>* visattr)  // OUT: one per glyph
{
	// Initial size guess for the number of glyphs recommended by Uniscribe
	glyphs->resize(input_length * 3 / 2 + 16);  
	visattr->resize(glyphs->size());

	// The logs array is the same size as the input.
	logs->resize(input_length);

	HDC temp_dc = NULL;  // Don't give it a DC unless we have to.
	HFONT old_font = NULL;
	HRESULT hr;
	while (true) 
	{
		int glyphs_used;
		hr = ::ScriptShape(
			temp_dc, 
			script_cache,
			input, 
			input_length, 
			logs->size(), 
			analysis,
			&(*glyphs)[0],
			&(*logs)[0], 
			&(*visattr)[0],
			&glyphs_used);

		if (SUCCEEDED(hr))
		{
			// It worked, resize the output list to the exact number it returned.
			glyphs->resize(glyphs_used);
			break;
		}

		// Different types of failure...
		if (hr == E_PENDING) 
		{
			// Need to select the font for the call. Don't do this if we don't have to
			// since it may be slow.
			temp_dc = GetDC(NULL);
			old_font = (HFONT)SelectObject(temp_dc, hfont);
			// Loop again...

		}
		else if (hr == E_OUTOFMEMORY)
		{
			// The glyph buffer needs to be larger. Just double it every time.
			glyphs->resize(glyphs->size() * 2);
			visattr->resize(glyphs->size() * 2);
			// Loop again...

		} 
		else if (hr == USP_E_SCRIPT_NOT_IN_FONT) 
		{
			// The font you selected doesn't have enough information to display
			// what you want. You'll have to pick another one somehow...
			// For our cases, we'll just return failure.
			break;

		}
		else 
		{
			// Some other failure.
			break;
		}
	}

	if (old_font)
	{
		SelectObject(temp_dc, old_font);  // Put back the previous font.
		ReleaseDC(NULL, temp_dc);
	}

	return SUCCEEDED(hr);
}
}
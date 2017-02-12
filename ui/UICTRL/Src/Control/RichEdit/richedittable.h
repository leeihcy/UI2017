#pragma once
// http://msdn.microsoft.com/en-us/library/windows/desktop/Hh768760(v=vs.85).aspx

namespace UI
{
//     Inserts one or more identical table rows with empty cells. 
// 
//     Parameters
//     wParam 
//         A pointer to a TABLEROWPARMS structure.
// 
//     lParam 
//         A pointer to a TABLECELLPARMS structure.
// 
//     Return value
//         Returns S_OK if the table is inserted, or an error code if not.
//
//    Remarks
//
//    If the cpStartRow member of the TABLEROWPARMS is ¨C1, this message deletes
//    the selected text (if any), and then inserts empty table rows with the row
//    and cell parameters given by wParam and lParam. It leaves the selection 
//    pointing to the start of the first cell in the first row. The client can 
//    then populate the table cells by pointing the selection (or an ITextRange) 
//    to the various cell end marks and inserting and formatting the desired text.
//    Such text can include nested table rows. Alternatively, if the cpStartRow
//    member of the TABLEROWPARMS is 0 or greater, table rows are inserted at the
//    character position given by cpStartRow. This only changes the current selection
//    if the table is inserted inside the selected text. 


#define EM_INSERTTABLE       (WM_USER + 232)

//
//     Replaces the selection with a blob that displays an image. 
//
//     Parameters
//         wParam 
//         Not used; must be zero. 
// 
//         lParam 
//         A pointer to a RICHEDIT_IMAGE_PARAMETERS structure that contains the image blob. 
//
//     Return value
//         Returns S_OK if successful, or one of the following error codes.
//         E_FAIL  Cannot insert the image. 
//         E_INVALIDARG The lParam parameter is NULL or points to an invalid image.  
//         E_OUTOFMEMORY  Insufficient memory is available.
//
#define EM_INSERTIMAGE       (WM_USER + 314)



typedef struct _RICHEDIT_IMAGE_PARAMETERS {
    LONG    xWidth;
    LONG    xHeight;
    LONG    Ascent;
    LONG    Type;
    LPCWSTR pwszAlternateText;
    IStream *pIStream;
} RICHEDIT_IMAGE_PARAMETERS;

typedef struct _tableRowParms {
    BYTE  cbRow;                  // The count of bytes in this structure.
    BYTE  cbCell;                 // The count of bytes in TABLECELLPARMS.
    BYTE  cCell;                  // The count of cells in a row, up to the maximum specified by MAX_TABLE_CELLS.
    BYTE  cRow;                   // The count of rows.
    LONG  dxCellMargin;           // The size of the left and right margins in a cell (\trgaph).
    LONG  dxIndent;               // The amount of left indentation, or right indentation if the fRTL member is TRUE (similar to \trleft).
    LONG  dyHeight;               // The height of a row (\trrh). 
    DWORD nAlignment:3;           // Row alignment \trql, trqr, \trqc),PFA_CENTER,PFA_LEFT,PFA_RIGHT
    DWORD fRTL:1;                 // Display cells in right-to-left order (\rtlrow).
    DWORD fKeep:1;                // Keep rows together (\trkeep).
    DWORD fKeepFollow:1;          // Keep the row on the same page as the following row (\trkeepfollow).
    DWORD fWrap:1;                // Wrap text to the right or left, depending on bAlignment (see \tdfrmtxtLeftN and \tdfrmtxtRightN).
    DWORD fIdentCells:1;          // Indent cells.
    LONG  cpStartRow;             // The character position that indicates where to insert table. A value of ¨C1 indicates the character position of the selection. 
    BYTE  bTableLevel;            // The table nesting level (EM_GETTABLEPARMS only).
    BYTE  iCell;                  // The index of the cell to insert or delete (EM_SETTABLEPARMS only).
} TABLEROWPARMS;

typedef struct _tableCellParms {
    LONG     dxWidth;             // The width of a cell (\cellx). 
    WORD     nVertAlign:2;        // The vertical alignment of cells (\clvertalt (def), \clvertalc, \clvertalb). It can be one of the following values.
								  //	0 The content appears at the top of a cell. 
								  //	1 The content is centered in the cell.
								  //	2 The content appears at the bottom of a cell.
    WORD     fMergeTop:1;         // Top cell for vertical merge (\clvmgf).
    WORD     fMergePrev:1;        // Merge with the cell above (\clvmrg).
    WORD     fVertical:1;         // Display text top to bottom, right to left (\cltxtbrlv).
    WORD     fMergeStart:1;       // Start set of horizontally merged cells (\clmgf).
    WORD     fMergeCont:1;        // Merge with the previous cell (\clmrg).
    WORD     wShading;            // Shading in .01% (\clshdng). This controls the amount of pattern foreground color (crForePat) and pattern background color (crBackPat) that is used to create the cell background color. If wShading is 0, the cell background is crBackPat. If it¡¯s 10000, the cell background is crForePat. Values of wShading in between are mixtures of the two pattern colors. 
    SHORT    dxBrdrLeft;          // Left border width, in twips (\clbrdrl\brdrwN).
    SHORT    dyBrdrTop;           // Top border width (\clbrdrt\brdrwN).
    SHORT    dxBrdrRight;         // Right border width (\clbrdrr\brdrwN).
    SHORT    dyBrdrBottom;        // Bottom border width (\clbrdrb\brdrwN).
    COLORREF crBrdrLeft;          // Left border color (\clbrdrl\brdrcf).
    COLORREF crBrdrTop;           // Top border color (\clbrdrt\brdrcf).
    COLORREF crBrdrRight;         // Right border color (\clbrdrr\brdrcf).
    COLORREF crBrdrBottom;        // Bottom border color (\clbrdrb\brdrcf).
    COLORREF crBackPat;           // Background color (\clcbpat).
    COLORREF crForePat;           // Foreground color (\clcfpat).
} TABLECELLPARMS;




}
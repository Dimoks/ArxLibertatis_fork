/*
===========================================================================
ARX FATALIS GPL Source Code
Copyright (C) 1999-2010 Arkane Studios SA, a ZeniMax Media company.

This file is part of the Arx Fatalis GPL Source Code ('Arx Fatalis Source Code'). 

Arx Fatalis Source Code is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Arx Fatalis Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Arx Fatalis Source Code.  If not, see 
<http://www.gnu.org/licenses/>.

In addition, the Arx Fatalis Source Code is also subject to certain additional terms. You should have received a copy of these 
additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Arx 
Fatalis Source Code. If not, please request a copy in writing from Arkane Studios at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing Arkane Studios, c/o 
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//////////////////////////////////////////////////////////////////////////////////////
//   @@        @@@        @@@                @@                           @@@@@     //
//   @@@       @@@@@@     @@@     @@        @@@@                         @@@  @@@   //
//   @@@       @@@@@@@    @@@    @@@@       @@@@      @@                @@@@        //
//   @@@       @@  @@@@   @@@  @@@@@       @@@@@@     @@@               @@@         //
//  @@@@@      @@  @@@@   @@@ @@@@@        @@@@@@@    @@@            @  @@@         //
//  @@@@@      @@  @@@@  @@@@@@@@         @@@@ @@@    @@@@@         @@ @@@@@@@      //
//  @@ @@@     @@  @@@@  @@@@@@@          @@@  @@@    @@@@@@        @@ @@@@         //
// @@@ @@@    @@@ @@@@   @@@@@            @@@@@@@@@   @@@@@@@      @@@ @@@@         //
// @@@ @@@@   @@@@@@@    @@@@@@           @@@  @@@@   @@@ @@@      @@@ @@@@         //
// @@@@@@@@   @@@@@      @@@@@@@@@@      @@@    @@@   @@@  @@@    @@@  @@@@@        //
// @@@  @@@@  @@@@       @@@  @@@@@@@    @@@    @@@   @@@@  @@@  @@@@  @@@@@        //
//@@@   @@@@  @@@@@      @@@      @@@@@@ @@     @@@   @@@@   @@@@@@@    @@@@@ @@@@@ //
//@@@   @@@@@ @@@@@     @@@@        @@@  @@      @@   @@@@   @@@@@@@    @@@@@@@@@   //
//@@@    @@@@ @@@@@@@   @@@@             @@      @@   @@@@    @@@@@      @@@@@      //
//@@@    @@@@ @@@@@@@   @@@@             @@      @@   @@@@    @@@@@       @@        //
//@@@    @@@  @@@ @@@@@                          @@            @@@                  //
//            @@@ @@@                           @@             @@        STUDIOS    //
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// ARX_Text
//////////////////////////////////////////////////////////////////////////////////////
//
// Description:
//		ARX Text Management
//
// Updates: (date) (person) (update)
//
// Copyright (c) 1999-2000 ARKANE Studios SA. All rights reserved
//////////////////////////////////////////////////////////////////////////////////////

#include "gui/Text.h"

#include <assert.h>

#include <SFML/System/Unicode.hpp>

#include "core/Localization.h"
#include "core/Core.h"
#include "graphics/Draw.h"
#include "io/Filesystem.h"
#include "io/Logger.h"

using std::string;

//-----------------------------------------------------------------------------
std::string lpszFontMenu;
std::string lpszFontIngame;;

char tUText[8192];

TextManager * pTextManage;
TextManager * pTextManageFlyingOver;

//-----------------------------------------------------------------------------
HFONT InBookFont	= NULL;
HFONT hFontRedist	= NULL;
HFONT hFontMainMenu = NULL;
HFONT hFontMenu		= NULL;
HFONT hFontControls = NULL;
HFONT hFontCredits	= NULL;

 
HFONT hFontInGame	= NULL;
HFONT hFontInGameNote = NULL;
 

extern long CHINESE_VERSION;
extern long EAST_EUROPE;

//-----------------------------------------------------------------------------
string FontError() {
	LPVOID lpMsgBuf;
	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    GetLastError(),
	    0, // Default language
	    (LPTSTR) &lpMsgBuf,
	    0,
	    NULL);
	return string("Font Error: ") + (LPCSTR)lpMsgBuf;
}
//-----------------------------------------------------------------------------

long ARX_UNICODE_ForceFormattingInRect(HFONT _hFont, const std::string& _lpszUText, int _iSpacingY, RECT _rRect)
{
	int iTemp = 0;

	if (danaeApp.m_pddsRenderTarget)
	{
		HDC hDC;

		if (SUCCEEDED(danaeApp.m_pddsRenderTarget->GetDC(&hDC)))
		{
			int		iLenght	= _lpszUText.length();
			int		iHeight	= 0;
			SIZE	sSize;
			int		iOldTemp;
			bool	bWrite;

			sSize.cx = sSize.cy = 0 ;

			SelectObject(hDC, _hFont);

			while (1)
			{
				bWrite			= true;
				int iLenghtCurr	= _rRect.left;
				iOldTemp		= iTemp;

				ARX_CHECK(iTemp < iLenght);

				for (; iTemp < iLenght ; iTemp++)
				{
					GetTextExtentPoint32(hDC,
					                      &_lpszUText[iTemp],
					                      1,
					                      &sSize);
					{
						if ((_lpszUText[iTemp] == _T('\n')) ||
								(_lpszUText[iTemp] == _T('*')))
						{
							iHeight		+= _iSpacingY + sSize.cy;
							bWrite		 = false;
							_rRect.top	+= _iSpacingY + sSize.cy;
							iTemp++;
							break;
						}
					}


					iLenghtCurr	+= sSize.cx;

					if (iLenghtCurr > _rRect.right)
					{
						iHeight += _iSpacingY + sSize.cy;

						if (CHINESE_VERSION)
						{
							iTemp--;
						}
						else
						{
							while ((_lpszUText[iTemp] != _T(' ')) && (iTemp > 0)) iTemp--;
						}

						bWrite		 = false;
						_rRect.top	+= _iSpacingY + sSize.cy;
						iTemp++;
						break;
					}
				}

				if ((iTemp == iLenght) ||
						((_rRect.top + sSize.cy) > _rRect.bottom))
				{
					break;
				}

			}

			danaeApp.m_pddsRenderTarget->ReleaseDC(hDC);
		}
	}

	return iTemp;
}

//-----------------------------------------------------------------------------
long ARX_UNICODE_FormattingInRect(HDC _hDC, std::string& text, int _iSpacingY, RECT & _rRect)
{
	size_t	iLenght = text.length();
	int iHeight = 0;
	SIZE sSize;
	size_t iOldTemp;
	bool bWrite;
	sSize.cx = sSize.cy = 0;

	size_t iTemp = 0;

	while (1)
	{
		bWrite = true;
		int iLenghtCurr = _rRect.left;
		iOldTemp = iTemp;

		for (; iTemp < iLenght; iTemp++)
		{
			GetTextExtentPoint32(_hDC,
			                      &text[iTemp],
			                      1,
			                      &sSize);

			if ((text[iTemp] == '\n') ||
			        (text[iTemp] == '*'))
			{
				iHeight += _iSpacingY + sSize.cy;
				text[iTemp] = _T('\0');
				bWrite = false;

				TextOutA(_hDC, _rRect.left, _rRect.top, &text[iOldTemp], strlen(&text[iOldTemp]));
				_rRect.top += _iSpacingY + sSize.cy;
				iTemp++;
				break;
			}
			
			iLenghtCurr += sSize.cx;

			if (iLenghtCurr > _rRect.right)
			{
				iHeight += _iSpacingY + sSize.cy;

				if (CHINESE_VERSION)
				{
					_TCHAR * ptexttemp = (_TCHAR *)malloc((iTemp - iOldTemp + 1) << 1);
					_tcsncpy(ptexttemp, &text[iOldTemp], iTemp - iOldTemp);
					ptexttemp[iTemp-iOldTemp] = _T('\0');

					TextOutA(_hDC, _rRect.left, _rRect.top, ptexttemp, strlen(ptexttemp));
					free((void *)ptexttemp);
					ptexttemp = NULL;
					iTemp--;
				}
				else
				{
					while ((text[iTemp] != _T(' ')) && (iTemp > 0)) iTemp--;

					text[iTemp] = _T('\0');

					if(!TextOutA(_hDC, _rRect.left, _rRect.top, &text[iOldTemp], strlen(&text[iOldTemp]))) {
						LogError << FontError() << " while displaying " << &text[iOldTemp];
					}
				}

				bWrite = false;
				_rRect.top += _iSpacingY + sSize.cy;
				iTemp++;
				break;
			}


		}

		if (iTemp == iLenght) break;

		if (iTemp == iOldTemp) break;
	}

	if (bWrite)
	{
		iHeight += _iSpacingY + sSize.cy;

		if (!TextOutA(_hDC, _rRect.left, _rRect.top, &text[iOldTemp], strlen(&text[iOldTemp])))
		{
			LogError << FontError() << " while displaying " << &text[iOldTemp];
		}

		_rRect.top += _iSpacingY + sSize.cy;
	}

	return iHeight;
}

//-----------------------------------------------------------------------------
long ARX_UNICODE_DrawTextInRect(float x, float y,
                                float maxx, float maxy,
                                const std::string& _text,
                                COLORREF col,
                                COLORREF bcol,
                                HFONT font,
                                HRGN hRgn,
                                HDC hHDC
                               )
{
	HDC hDC = NULL;

	// Get a DC for the surface. Then, write out the buffer
	if (danaeApp.m_pddsRenderTarget)
	{
		if (hHDC)
		{
			hDC = hHDC;
		}

		if (hHDC || SUCCEEDED(danaeApp.m_pddsRenderTarget->GetDC(&hDC)))
		{

			strcpy( tUText, _text.c_str() );

			if (hRgn)
			{
				SelectClipRgn(hDC,
							  hRgn);
			}

			if (bcol == 0x00FF00FF) SetBkMode(hDC, TRANSPARENT);
			else
			{
				SetBkMode(hDC, OPAQUE);
				SetBkColor(hDC, bcol);
			}

			SetTextColor(hDC, col);

			SelectObject(hDC,  font);

			RECT rect;
			rect.top	= (long)y;
			rect.left	= (long)x;
			rect.right	= (long)maxx;
		std::string text( tUText );
			long n = ARX_UNICODE_FormattingInRect(hDC, text, 0, rect);
			rect.top	= (long)y;
			rect.bottom	= ((long)y) + n;

			SelectClipRgn(hDC,
						  NULL);

			if (!hHDC)
			{
				danaeApp.m_pddsRenderTarget->ReleaseDC(hDC);
			}

			return n;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
long ARX_TEXT_Draw(LPDIRECT3DDEVICE7 pd3dDevice,
                   HFONT ef,
                   float x, float y,
                   long spacingx, long spacingy,
                   const std::string& car,
                   COLORREF colo, COLORREF bcol)
{
	if (car.empty() ) return 0;

	if (car[0] == 0) return 0;

	//ArxFont
	ARX_UNICODE_DrawTextInRect(x, y, 9999.f, 999.f, car, colo, bcol, ef);
	return 15 + spacingy;
}

long ARX_TEXT_DrawRect(LPDIRECT3DDEVICE7 pd3dDevice,
					   HFONT ef,
					   float x, float y,
					   long spacingx, long spacingy,
					   float maxx, float maxy,
					   const std::string& car,
					   COLORREF colo,
					   HRGN _hRgn,
					   COLORREF bcol,
					   long flags)
{

	bcol = RGB((bcol >> 16) & 255, (bcol >> 8) & 255, (bcol) & 255);

	colo = RGB((colo >> 16) & 255, (colo >> 8) & 255, (colo) & 255);
	return ARX_UNICODE_DrawTextInRect(x, y, maxx, maxy, car, colo, bcol, ef, _hRgn);
}


//-----------------------------------------------------------------------------
float DrawBookTextInRect(float x, float y, float maxx, float maxy, const std::string& text, COLORREF col, COLORREF col2, HFONT font)
{
	return (float)ARX_TEXT_DrawRect(GDevice, font,
									(BOOKDECX + x) * Xratio, (BOOKDECY + y) * Yratio, -3, 0,
									(BOOKDECX + maxx) * Xratio, (BOOKDECY + maxy) * Yratio, text, col, NULL, col2);
}

//-----------------------------------------------------------------------------
void DrawBookTextCenter(float x, float y, const std::string& text, COLORREF col, COLORREF col2, HFONT font)
{
	UNICODE_ARXDrawTextCenter((BOOKDECX + x)*Xratio, (BOOKDECY + y)*Yratio, text, col, col2, font);
}

//-----------------------------------------------------------------------------

long UNICODE_ARXDrawTextCenter(float x, float y, const std::string& str, COLORREF col, COLORREF bcol, HFONT font)
{


	HDC hDC;

	// Get a DC for the surface. Then, write out the buffer
	if (danaeApp.m_pddsRenderTarget)
	{
		if (SUCCEEDED(danaeApp.m_pddsRenderTarget->GetDC(&hDC)))
		{
			if (bcol == 0x00FF00FF) SetBkMode(hDC, TRANSPARENT);
			else
			{
				SetBkMode(hDC, OPAQUE);
				SetBkColor(hDC, bcol);
			}

			SetTextColor(hDC, col);

			SelectObject(hDC,  font);


			SIZE siz;
			GetTextExtentPoint32(hDC,         // handle to DC
			                        str.c_str(),           // character string
			                        str.length(),   // number of characters
			                        &siz          // size
			                       );
			RECT rect;
			rect.top = (long)y;
			rect.bottom = (long)999;
			rect.left = (long)x - (siz.cx >> 1);
			rect.right = (long)999;

			TextOut(hDC, rect.left, rect.top, str.c_str(), str.length());

			danaeApp.m_pddsRenderTarget->ReleaseDC(hDC);
			return siz.cx;
		}
	}

	return 0;
}



long UNICODE_ARXDrawTextCenteredScroll(float x, float y, float x2, const std::string& str, COLORREF col, COLORREF bcol, HFONT font, int iTimeScroll, float fSpeed, int iNbLigne, int iTimeOut)
{
	RECT rRect;
	ARX_CHECK_LONG(y);
	ARX_CHECK_LONG(x + x2);   //IF OK, x - x2 cannot overflow
	rRect.left = ARX_CLEAN_WARN_CAST_LONG(x - x2);
	rRect.top = ARX_CLEAN_WARN_CAST_LONG(y);
	rRect.right = ARX_CLEAN_WARN_CAST_LONG(x + x2);


	if (pTextManage)
	{
		pTextManage->AddText(font,
							 str,
							 rRect,
							 col,
							 bcol,
							 iTimeOut,
							 iTimeScroll,
							 fSpeed,
							 iNbLigne
							);

		return ARX_CLEAN_WARN_CAST_LONG(x2);
	}

	return 0;
}

//-----------------------------------------------------------------------------
void ARX_Allocate_Text( std::string& dest, const std::string& id_string) {
	std::string output;
	PAK_UNICODE_GetPrivateProfileString(id_string, "default", output);
	dest = output;
}

//-----------------------------------------------------------------------------
struct _FONT_HEADER
{
	ULONG   ulVersion;
	USHORT  usNumTables;
	USHORT  usSearchRange;
	USHORT  usEntrySelector;
	USHORT  usRangeShift;
};

//-----------------------------------------------------------------------------
struct _FONT_TABLE_HEADER
{
	ULONG	ulTag;
	ULONG	ulCheckSum;
	ULONG	ulOffset;
	ULONG	ulLength;
};

//-----------------------------------------------------------------------------
struct _FONT_NAMING_HEADER
{
	USHORT	usFormat;
	USHORT	usNbNameRecords;
	USHORT	usOffsetStorage;	//(from start of table)
};

//-----------------------------------------------------------------------------
struct _FONT_NAMING_NAMERECORD
{
	USHORT	usPlatformID;
	USHORT	usPlatformSpecificEncodingID;
	USHORT	usLanguageID;
	USHORT	usNameID;
	USHORT	usStringLength;
	USHORT	usStringOffset;		//from start of storage area (in bytes)
};

ULONG LilEndianLong(ULONG ulValue) {
	return MAKELONG(
			MAKEWORD(HIBYTE(HIWORD(ulValue)), LOBYTE(HIWORD(ulValue))),
			MAKEWORD(HIBYTE(LOWORD(ulValue)), LOBYTE(LOWORD(ulValue)))
	);
}

USHORT LilEndianShort(USHORT ulValue) {
	return MAKEWORD(HIBYTE(ulValue), LOBYTE(ulValue));
}

std::string GetFontName( const std::string& _lpszFileName)
{
	DWORD dwSize;
	DWORD dwRead;
	int   iResult;

	HANDLE hFile = CreateFile(_lpszFileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		LogError << "FontName :: File not Found - " << _lpszFileName;
		return "";
	}

	dwSize = GetFileSize(hFile, NULL);

	// Read the font header
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	_FONT_HEADER FH;
	iResult = ReadFile(hFile, &FH, sizeof(FH), &dwRead, NULL);

	if (iResult == 0)
		LogWarning << "FontName :: Unable to read font header - " << _lpszFileName;

	// Read the font table header
	for (int i = 0; i < FH.usNumTables; i++)
	{
		_FONT_TABLE_HEADER FTH;
		iResult = ReadFile(hFile, &FTH, sizeof(FTH), &dwRead, NULL);

		if (iResult == 0)
			LogWarning << "FontName :: Unable to read font table header - " << _lpszFileName;

		char szName[5];
		szName[0] = LOBYTE(LOWORD(FTH.ulTag));
		szName[1] = HIBYTE(LOWORD(FTH.ulTag));
		szName[2] = LOBYTE(HIWORD(FTH.ulTag));
		szName[3] = HIBYTE(HIWORD(FTH.ulTag));
		szName[4] = 0;


		// Check for "name" in the extracted bytes
		if (strcmp(szName, "name") == 0)
		{
			FTH.ulOffset = LilEndianLong(FTH.ulOffset);
			SetFilePointer(hFile, FTH.ulOffset, NULL, FILE_BEGIN);

			// Read font naming header
			_FONT_NAMING_HEADER FNH;
			iResult = ReadFile(hFile, &FNH, sizeof(FNH), &dwRead, NULL);

			if (iResult == 0)
			{
				LogWarning << "FontName :: Unable to read font naming header - " << _lpszFileName;
			}

			FNH.usNbNameRecords = LilEndianShort(FNH.usNbNameRecords);
			FNH.usOffsetStorage = LilEndianShort(FNH.usOffsetStorage);

			for (int j = 0; j < FNH.usNbNameRecords; j++)
			{
				_FONT_NAMING_NAMERECORD FNN;
				iResult = ReadFile(hFile, &FNN, sizeof(FNN), &dwRead, NULL);

				if (iResult == 0)
					LogWarning << "FontName :: Unable to read font naming namerecord - " << _lpszFileName;

				FNN.usNameID = LilEndianShort(FNN.usNameID);
				FNN.usPlatformID = LilEndianShort(FNN.usPlatformID);
				FNN.usStringLength = LilEndianShort(FNN.usStringLength);
				FNN.usStringOffset = LilEndianShort(FNN.usStringOffset);
				FNN.usLanguageID = LilEndianShort(FNN.usLanguageID);

				if (FNN.usLanguageID == 1033)
					if (FNN.usNameID == 1)
					{
						SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
						SetFilePointer(hFile, FTH.ulOffset + FNH.usOffsetStorage + FNN.usStringOffset, NULL, FILE_BEGIN);
						

						u16 szName[256];

						ZeroMemory(szName, 256);
						assert(FNN.usStringLength < 256);
 
						iResult = ReadFile(hFile, szName, FNN.usStringLength, &dwRead, NULL);

						if (iResult == 0)
							LogWarning << "FontName :: Unable to read font name - " << _lpszFileName;

						size_t len = FNN.usStringLength / 2;
						
						for(size_t i = 0; i < len; i++) {
							szName[i] = LilEndianShort(szName[i]);
						}
						
						string result;
						result.reserve(len);
						sf::Unicode::UTF16ToUTF8(szName, &szName[len], std::back_inserter(result));
						
						CloseHandle(hFile);
						return result;
					}
			}
		}
	}

	LogError << "FontName :: Unable to match \"name\" in any Font Table Header";
	CloseHandle(hFile);
	return ""; // Return empty font name
}

void _ShowText(char * text)
{
	if (GDevice)
	{
		GDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0L);

		if (danaeApp.DANAEStartRender())
		{

			HDC hDC;

			if (danaeApp.m_pddsRenderTarget)
			{
				if (SUCCEEDED(danaeApp.m_pddsRenderTarget->GetDC(&hDC)))
				{
					SetTextColor(hDC, RGB(0, 255, 0));
					SetBkMode(hDC, TRANSPARENT);
					ExtTextOut(hDC, 0, 0, 0, NULL, text, lstrlen(text), NULL);
					danaeApp.m_pddsRenderTarget->ReleaseDC(hDC);
				}
			}

			danaeApp.DANAEEndRender();

			danaeApp.m_pFramework->ShowFrame();
		}
	}
}

//-----------------------------------------------------------------------------

int Traffic(int iFontSize)
{
	iFontSize = (int)(float)(iFontSize * Yratio);

	if (CHINESE_VERSION)
	{
		if (iFontSize < 14)
			iFontSize = 12;
		else if (iFontSize < 15)
			iFontSize = 14;
		else if (iFontSize < 18)
			iFontSize = 15;
		else if (iFontSize <= 29)
			iFontSize = 18;
		else
			iFontSize = 30;
	}

	return iFontSize;
}

HFONT _CreateFont(
    int nHeight,               // height of font
    int nWidth,                // average character width
    int nEscapement,           // angle of escapement
    int nOrientation,          // base-line orientation angle
    int fnWeight,              // font weight
    DWORD fdwItalic,           // italic attribute option
    DWORD fdwUnderline,        // underline attribute option
    DWORD fdwStrikeOut,        // strikeout attribute option
    DWORD fdwCharSet,          // character set identifier
    DWORD fdwOutputPrecision,  // output precision
    DWORD fdwClipPrecision,    // clipping precision
    DWORD fdwQuality,          // output quality
    DWORD fdwPitchAndFamily,   // pitch and family
    std::string lpszFace          // typeface name
)
{

	/*
	ANSI_CHARSET
	BALTIC_CHARSET
	CHINESEBIG5_CHARSET
	DEFAULT_CHARSET
	EASTEUROPE_CHARSET
	GB2312_CHARSET
	GREEK_CHARSET
	HANGUL_CHARSET
	MAC_CHARSET
	OEM_CHARSET
	RUSSIAN_CHARSET
	SHIFTJIS_CHARSET
	SYMBOL_CHARSET
	TURKISH_CHARSET
	VIETNAMESE_CHARSET
	*/

	if (EAST_EUROPE)
	{
		fdwCharSet = CHINESEBIG5_CHARSET;
	}

	//HFONT  ret = CreateFont(
	HFONT ret = CreateFont(
	                nHeight,               // height of font
	                nWidth,                // average character width
	                nEscapement,           // angle of escapement
	                nOrientation,          // base-line orientation angle
	                fnWeight,              // font weight
	                fdwItalic,           // italic attribute option
	                fdwUnderline,        // underline attribute option
	                fdwStrikeOut,        // strikeout attribute option
	                fdwCharSet,          // character set identifier
	                fdwOutputPrecision,  // output precision
	                fdwClipPrecision,    // clipping precision
	                fdwQuality,          // output quality
	                fdwPitchAndFamily,   // pitch and family
	                lpszFace.c_str()     // typeface name
	            );

	if (!ret)
	{
		LogError << FontError() << " creating font " << lpszFace;
	}

	return ret;
}


string getFontFile() {
	string tx= "misc" PATH_SEPERATOR_STR "Arx.ttf";
	if(!FileExist(tx.c_str())) {
		tx = "misc" PATH_SEPERATOR_STR "ARX_default.ttf"; // Full path
	}
	return tx;
}

//-----------------------------------------------------------------------------
void ARX_Text_Init()
{
	
	ARX_Text_Close();

	Localisation_Init();
	
	string tx = getFontFile();
	lpszFontIngame = GetFontName(tx);
	
	LogInfo << "Adding Font " << tx << ": " << lpszFontIngame;

	if(AddFontResource(tx.c_str()) == 0) {
		LogError << FontError();
	}
	
	lpszFontMenu = lpszFontIngame;

	pTextManage = new TextManager();
	pTextManageFlyingOver = new TextManager();


	if (!hFontMainMenu)
	{
		int iFontSize = 48;//58;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_mainmenu_size", "58", szUT);
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		if (!hFontMainMenu)
		{
			hFontMainMenu = _CreateFont(
								iFontSize,
								0, 0, 0, FW_NORMAL, false, false, false,
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								ANTIALIASED_QUALITY,
								VARIABLE_PITCH,
								lpszFontMenu.c_str());
			if(!hFontMainMenu) {
				LogError << "error loading main menu font";
			}
		}
	}

	if (!hFontMenu)
	{
		int iFontSize = 32;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_menu_size", "32", szUT);
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		if (!hFontMenu)
		{
			hFontMenu = _CreateFont(
							iFontSize,
							0, 0, 0, FW_NORMAL, false, false, false,
							DEFAULT_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY,
							VARIABLE_PITCH,
							lpszFontMenu.c_str());
			if(!hFontMenu) {
				LogError << "error loading menu font";
			}
		}
	}

	if (!hFontControls)
	{
		int iFontSize = 16;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_menucontrols_size", "22", szUT);
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		if (!hFontControls)
		{
			hFontControls = _CreateFont(
								iFontSize,
								0, 0, 0, FW_NORMAL, false, false, false,
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								ANTIALIASED_QUALITY,
								VARIABLE_PITCH,
								lpszFontMenu.c_str());
			if(!hFontControls) {
				LogError << "error loading controls font";
			}
		}
	}

	if (!hFontCredits)
	{
		int iFontSize = 32;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_menucredits_size", "36", szUT);
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		if (!hFontCredits)
		{
			hFontCredits = _CreateFont(
							   iFontSize,
							   0, 0, 0, FW_NORMAL, false, false, false,
							   DEFAULT_CHARSET,
							   OUT_DEFAULT_PRECIS,
							   CLIP_DEFAULT_PRECIS,
							   ANTIALIASED_QUALITY,
							   VARIABLE_PITCH,
							   lpszFontMenu.c_str());
			if(!hFontCredits) {
				LogError << "error loading credits font";
			}
		}
	}

	if (!hFontRedist)
	{
		int iFontSize = 16;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_redist_size", "18", szUT );
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		hFontRedist = _CreateFont(
						  iFontSize,
						  0, 0, 0, FW_NORMAL, false, false, false,
						  DEFAULT_CHARSET,
						  OUT_DEFAULT_PRECIS,
						  CLIP_DEFAULT_PRECIS,
						  ANTIALIASED_QUALITY,
						  VARIABLE_PITCH,
						  lpszFontIngame.c_str());
		if(!hFontRedist) {
			LogError << "error loading redist font";
		}
	}

	// NEW QUEST
	if (Yratio > 1.f)
	{
		Yratio *= .8f;
	}

	if (!hFontInGame)
	{
		int iFontSize = 16;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_book_size", "18", szUT );
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		if (!hFontInGame)
		{
			hFontInGame = _CreateFont(
							  iFontSize,
							  0, 0, 0, FW_NORMAL, false, false, false,
							  DEFAULT_CHARSET,
							  OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS,
							  ANTIALIASED_QUALITY,
							  VARIABLE_PITCH,
							  lpszFontIngame.c_str());
			if(!hFontInGame) {
				LogError << "error loading ingame font";
			}
		}
	}

	if (!hFontInGameNote)
	{
		int iFontSize = 16;//18;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_note_size", "18", szUT );
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		hFontInGameNote = _CreateFont(
							  iFontSize,
							  0, 0, 0, FW_NORMAL, false, false, false,
							  DEFAULT_CHARSET,
							  OUT_DEFAULT_PRECIS,
							  CLIP_DEFAULT_PRECIS,
							  ANTIALIASED_QUALITY,
							  VARIABLE_PITCH,
							  lpszFontIngame.c_str());
		if(!hFontInGameNote) {
			LogError << "error loading ingame note font";
		}
	}

	if (!InBookFont)
	{
		int iFontSize = 16;

		std::string szUT;
		PAK_UNICODE_GetPrivateProfileString( "system_font_book_size", "18", szUT );
		std::istringstream ss(szUT);
		ss >> iFontSize;
		iFontSize = Traffic(iFontSize);

		InBookFont = _CreateFont(
						 iFontSize,
						 0, 0, 0, FW_NORMAL, false, false, false,
						 DEFAULT_CHARSET,
						 OUT_DEFAULT_PRECIS,
						 CLIP_DEFAULT_PRECIS,
						 ANTIALIASED_QUALITY,
						 VARIABLE_PITCH,
						 lpszFontIngame.c_str());
		if(!InBookFont) {
			LogError << "error loading book font";
		}
	}
}

//-----------------------------------------------------------------------------
void ARX_Text_Close() {
	
	lpszFontIngame.clear();
	lpszFontMenu.clear();
	
	string tx = getFontFile();
	
	LogDebug << "Removing font " << tx;
	
	lpszFontIngame = GetFontName(tx);
	
	if(!RemoveFontResource(tx.c_str())) {
		LogError << FontError() << " while removing font " << tx;
	}
	
	Localisation_Close();

	if (pTextManage)
	{
		delete pTextManage;
		pTextManage = NULL;
	}

	if (pTextManageFlyingOver)
	{
		delete pTextManageFlyingOver;
		pTextManageFlyingOver = NULL;
	}

	if (InBookFont)
	{
		DeleteObject(InBookFont);
		InBookFont = NULL;
	}

	if (hFontRedist)
	{
		DeleteObject(hFontRedist);
		hFontRedist = NULL;
	}

	if (hFontMainMenu)
	{
		DeleteObject(hFontMainMenu);
		hFontMainMenu = NULL;
	}

	if (hFontMenu)
	{
		DeleteObject(hFontMenu);
		hFontMenu = NULL;
	}

	if (hFontControls)
	{
		DeleteObject(hFontControls);
		hFontControls = NULL;
	}

	if (hFontCredits)
	{
		DeleteObject(hFontCredits);
		hFontCredits = NULL;
	}

	if (hFontInGame)
	{
		DeleteObject(hFontInGame);
		hFontInGame = NULL;
	}

	if (hFontInGameNote)
	{
		DeleteObject(hFontInGameNote);
		hFontInGameNote = NULL;
	}

}

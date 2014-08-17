/*
 * Copyright 2014 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/MainMenu.h"

#include <iomanip>

#include "core/Application.h"
#include "core/Config.h"
#include "core/Core.h"
#include "core/Localisation.h"
#include "core/SaveGame.h"
#include "gui/MenuWidgets.h"
#include "gui/MenuPublic.h"
#include "gui/Text.h"
#include "graphics/Math.h"
#include "graphics/data/TextureContainer.h"
#include "input/Keyboard.h"
#include "window/RenderWindow.h"

CWindowMenu * pWindowMenu = NULL;
TextWidget * pDeleteConfirm = NULL;
TextWidget * pLoadConfirm = NULL;
TextWidget * pDeleteButton = NULL;
CheckboxWidget * fullscreenCheckbox = NULL;
CycleTextWidget * pMenuSliderResol = NULL;
Widget * pMenuElementApply = NULL;

void Menu2_Render_NewQuest(CWindowMenuConsole * console, Vec2i size) {
	
	{
	std::string szMenuText = getLocalised("system_menus_main_editquest_confirm");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->bCheck = false;
	console->AddMenuCenter(me);
	}
	
	{
	std::string szMenuText = getLocalised("system_menus_main_newquest_confirm");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->bCheck = false;
	console->AddMenuCenter(me);
	}
	
	CMenuPanel *pPanel = new CMenuPanel();
	
	{
	std::string szMenuText = getLocalised("system_yes");
	szMenuText += "   "; // TODO This space can probably go
	TextWidget * me = new TextWidget(BUTTON_MENUNEWQUEST_CONFIRM, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->SetPos(Vec2i(RATIO_X(size.x - (me->rZone.width() + 10)), 0));
	pPanel->AddElementNoCenterIn(me);
	}
	
	{
	std::string szMenuText = getLocalised("system_no");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(10), 0), MAIN);
	me->SetShortCut(Keyboard::Key_Escape);
	pPanel->AddElementNoCenterIn(me);
	}
	
	pPanel->Move(Vec2i(0, RATIO_Y(380)));
	
	console->AddMenu(pPanel);
}

void MainMenuCreateEditQuest(CWindowMenuConsole * console) {
	
	{
	std::string szMenuText = getLocalised("system_menus_main_editquest_load");
	TextWidget * me = new TextWidget(BUTTON_MENUEDITQUEST_LOAD_INIT, hFontMenu, szMenuText, Vec2i(0, 0), EDIT_QUEST_LOAD);
	me->lData = -1;
	console->AddMenuCenter(me);
	}

	{
	std::string szMenuText = getLocalised( "system_menus_main_editquest_save");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), EDIT_QUEST_SAVE);
	
	if(!ARXMenu_CanResumeGame()) {
		me->SetCheckOff();
		((TextWidget*)me)->lColor=Color(127,127,127);
	}
	console->AddMenuCenter(me);
	}
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->eMenuState = MAIN;
	cb->SetShortCut(Keyboard::Key_Escape);
	console->AddMenu(cb);
	}
}

void MainMenuCreateEditQuestLoad(CWindowMenuConsole * console, Vec2i size) {
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/icons/menu_main_load");
	ButtonWidget * cb = new ButtonWidget(Vec2i(0, 0), pTex);
	cb->bCheck = false;
	console->AddMenuCenter(cb);
	}
	
	std::string quicksaveName = getLocalised("system_menus_main_quickloadsave", "Quicksave");
	
	// TODO make this list scrollable
	// TODO align the date part to the right!
	
	{
	size_t quicksaveNum = 0;
	
	// Show quicksaves.
	for(size_t i = 0; i < savegames.size(); i++) {
		const SaveGame & save = savegames[i];
		
		if(!save.quicksave) {
			continue;
		}
		
		std::ostringstream text;
		text << quicksaveName << ' ' << ++quicksaveNum << "   " << save.time;
		
		Widget * e = new TextWidget(BUTTON_MENUEDITQUEST_LOAD, hFontControls, text.str(), Vec2i(RATIO_X(20), 0), NOP);
		e->lData = i;
		console->AddMenuCenterY(e);
	}
	
	// Show regular saves.
	for(size_t i = 0; i < savegames.size(); i++) {
		const SaveGame & save = savegames[i];
		
		if(save.quicksave) {
			continue;
		}
		
		std::string text = save.name +  "   " + save.time;
		
		Widget * e = new TextWidget(BUTTON_MENUEDITQUEST_LOAD, hFontControls, text, Vec2i(RATIO_X(20), 0), NOP);
		e->lData = i;
		console->AddMenuCenterY(e);
	}
	
	{
	Widget * confirm = new TextWidget(-1, hFontControls, " ", Vec2i(RATIO_X(20), 0), EDIT_QUEST_SAVE_CONFIRM);
	confirm->SetCheckOff();
	confirm->lData = -1;
	console->AddMenuCenterY(confirm);
	}
	
	// Delete button
	{
	std::string szMenuText = getLocalised("system_menus_main_editquest_delete");
	szMenuText += "   ";
	TextWidget * me = new TextWidget(BUTTON_MENUEDITQUEST_DELETE_CONFIRM, hFontMenu, szMenuText, Vec2i(0, 0), EDIT_QUEST_LOAD);
	me->SetPos(Vec2i(RATIO_X(size.x-10)-me->rZone.width(), RATIO_Y(42)));
	me->SetCheckOff();
	me->lOldColor = me->lColor;
	me->lColor = Color::grayb(127);
	console->AddMenu(me);
	pDeleteConfirm = me;
	}
	
	// Load button
	{
	std::string szMenuText = getLocalised("system_menus_main_editquest_load");
	szMenuText += "   ";
	TextWidget * me = new TextWidget(BUTTON_MENUEDITQUEST_LOAD_CONFIRM, hFontMenu, szMenuText, Vec2i(0, 0), MAIN);
	me->SetPos(Vec2i(RATIO_X(size.x-10)-me->rZone.width(), RATIO_Y(380) + RATIO_Y(40)));
	me->SetCheckOff();
	me->lOldColor = me->lColor;
	me->lColor = Color::grayb(127);
	console->AddMenu(me);
	pLoadConfirm = me;
	}
	
	// Back button
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 420)), pTex);
	cb->eMenuState = EDIT_QUEST;
	cb->SetShortCut(Keyboard::Key_Escape);
	console->AddMenu(cb);
	}
	}
}

void MainMenuCreateEditQuestSave(CWindowMenuConsole * console) {
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/icons/menu_main_save");
	ButtonWidget * cb = new ButtonWidget(Vec2i(RATIO_X(10), 0), pTex);
	cb->bCheck = false;
	console->AddMenuCenter(cb);
	}
	
	std::string quicksaveName = getLocalised("system_menus_main_quickloadsave", "Quicksave");
	size_t quicksaveNum = 0;
	
	// Show quicksaves.
	for(size_t i = 0; i < savegames.size(); i++) {
		const SaveGame & save = savegames[i];
		
		if(!save.quicksave) {
			continue;
		}
		
		std::ostringstream text;
		text << quicksaveName << ' ' << ++quicksaveNum << "   " << save.time;
		
		TextWidget * e = new TextWidget(BUTTON_MENUEDITQUEST_SAVEINFO, hFontControls,
		                                        text.str(), Vec2i(RATIO_X(20), 0.f), EDIT_QUEST_SAVE_CONFIRM);
		e->setColor(Color::grayb(127));
		e->SetCheckOff();
		e->lData = i;
		console->AddMenuCenterY(e);
	}
	
	// Show regular saves.
	for(size_t i = 0; i < savegames.size(); i++) {
		const SaveGame & save = savegames[i];
		
		if(save.quicksave) {
			continue;
		}
		
		std::string text = save.name +  "   " + save.time;
		
		Widget * e = new TextWidget(BUTTON_MENUEDITQUEST_SAVEINFO, hFontControls,
		                                        text, Vec2i(RATIO_X(20), 0.f), EDIT_QUEST_SAVE_CONFIRM);
		e->lData = i;
		console->AddMenuCenterY(e);
	}
	
	for(size_t i = savegames.size(); i <= 15; i++) {
		
		std::ostringstream text;
		text << '-' << std::setfill('0') << std::setw(4) << i << '-';
		
		TextWidget * e = new TextWidget(BUTTON_MENUEDITQUEST_SAVEINFO, hFontControls, text.str(), Vec2i(RATIO_X(20), 0), EDIT_QUEST_SAVE_CONFIRM);

		e->eMenuState = EDIT_QUEST_SAVE_CONFIRM;
		e->lData = -1;
		console->AddMenuCenterY(e);
	}

	{
	TextWidget * me01 = new TextWidget(-1, hFontControls, " ", Vec2i(RATIO_X(20), 0), EDIT_QUEST_SAVE_CONFIRM);
	me01->lData = -1;
	me01->SetCheckOff();
	console->AddMenuCenterY(me01);
	}
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 420)), pTex);
	cb->eMenuState = EDIT_QUEST;
	cb->SetShortCut(Keyboard::Key_Escape);
	console->AddMenu(cb);
	}
}

void MainMenuCreateEditQuestSaveConfirm(CWindowMenuConsole * console, Vec2i size) {
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/icons/menu_main_save");
	ButtonWidget * cb = new ButtonWidget(Vec2i(0, 0), pTex);
	cb->bCheck = false;
	console->AddMenuCenter(cb);
	}
	
	{
	std::string szMenuText = getLocalised("system_menu_editquest_newsavegame", "---");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
	me->lData = -1;
	me->eState=EDIT;
	me->ePlace=CENTER;
	console->AddMenuCenter(me);
	}
	
	CMenuPanel * pPanel = new CMenuPanel();
	
	// Delete button
	{
	std::string szMenuText = getLocalised("system_menus_main_editquest_delete");
	TextWidget * me = new TextWidget(BUTTON_MENUEDITQUEST_DELETE, hFontMenu, szMenuText, Vec2i(0, 0), EDIT_QUEST_SAVE);
	me->SetPos(Vec2i(RATIO_X(size.x-10)-me->rZone.width(), RATIO_Y(5)));
	me->lOldColor = me->lColor;
	pPanel->AddElementNoCenterIn(me);
	pDeleteButton = me;
	}
	
	// Save button
	{
	std::string szMenuText = getLocalised("system_menus_main_editquest_save");
	TextWidget * me = new TextWidget(BUTTON_MENUEDITQUEST_SAVE, hFontMenu, szMenuText, Vec2i(0, 0), MAIN);
	me->SetPos(Vec2i(RATIO_X(size.x-10)-me->rZone.width(), RATIO_Y(380)));
	pPanel->AddElementNoCenterIn(me);
	}
	
	// Back button
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->eMenuState = EDIT_QUEST_SAVE;
	cb->SetShortCut(Keyboard::Key_Escape);
	pPanel->AddElementNoCenterIn(cb);
	}

	console->AddMenu(pPanel);
}

void MainMenuOptionGroupsCreate(CWindowMenuConsole * console) {
	
	{
	std::string szMenuText = getLocalised("system_menus_options_video");
	TextWidget * me = new TextWidget(BUTTON_MENUOPTIONSVIDEO_INIT, hFontMenu, szMenuText, Vec2i(0, 0), OPTIONS_VIDEO);
	console->AddMenuCenter(me);
	}
	
	{
	std::string szMenuText = getLocalised("system_menus_options_audio");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), OPTIONS_AUDIO);
	console->AddMenuCenter(me);
	}
	
	{
	std::string szMenuText = getLocalised("system_menus_options_input");
	TextWidget * me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), OPTIONS_INPUT);
	console->AddMenuCenter(me);
	}
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->eMenuState = MAIN;
	cb->SetShortCut(Keyboard::Key_Escape);
	console->AddMenu(cb);
	}
}

void MainMenuOptionVideoCreate(CWindowMenuConsole * console, Vec2i size)
{
	std::string szMenuText;
	Widget *me;
	CMenuPanel *pc;
	TextureContainer *pTex;
	
	// Renderer selection
	{
		
		pc = new CMenuPanel();
		szMenuText = getLocalised("system_menus_options_video_renderer", "Renderer");
		szMenuText += "  ";
		me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
		me->SetCheckOff();
		pc->AddElement(me);
		CycleTextWidget * slider = new CycleTextWidget(BUTTON_MENUOPTIONSVIDEO_RENDERER, Vec2i(0, 0));
		
		slider->AddText(new TextWidget(-1, hFontMenu, "Auto-Select", Vec2i(0, 0), OPTIONS_VIDEO_RENDERER_AUTOMATIC));
		slider->selectLast();
#if ARX_HAVE_SDL1 || ARX_HAVE_SDL2
		slider->AddText(new TextWidget(-1, hFontMenu, "OpenGL", Vec2i(0, 0), OPTIONS_VIDEO_RENDERER_OPENGL));
		if(config.window.framework == "SDL") {
			slider->selectLast();
		}
#endif
		
		float fRatio    = (RATIO_X(size.x-9) - slider->rZone.width()); 
		slider->Move(Vec2i(checked_range_cast<int>(fRatio), 0));
		pc->AddElement(slider);
		console->AddMenuCenterY(pc);
		
	}
	
	{
	szMenuText = getLocalised("system_menus_options_videos_full_screen");
	if(szMenuText.empty()) {
		// TODO once we ship our own amendmends to the loc files a cleaner
		// fix would be to just define system_menus_options_videos_full_screen
		// for the german version there
		szMenuText = getLocalised("system_menus_options_video_full_screen");
	}
	szMenuText += "  ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), NOP);
	text->SetCheckOff();
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONSVIDEO_FULLSCREEN;
	cb->iState = config.video.fullscreen ? 1 : 0;
	console->AddMenuCenterY(cb);
	fullscreenCheckbox = cb;
	}
	
	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_video_resolution");
	szMenuText += "  ";
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	pMenuSliderResol = new CycleTextWidget(BUTTON_MENUOPTIONSVIDEO_RESOLUTION, Vec2i(0, 0));
	
	pMenuSliderResol->setEnabled(config.video.fullscreen);
	
	const RenderWindow::DisplayModes & modes = mainApp->getWindow()->getDisplayModes();
	for(size_t i = 0; i != modes.size(); ++i) {
		
		const DisplayMode & mode = modes[i];
		
		// find the aspect ratio
		unsigned a = mode.resolution.x;
		unsigned b = mode.resolution.y;
		while(b != 0) {
			unsigned t = a % b;
			a = b, b = t;
		}
		Vec2i aspect = mode.resolution / Vec2i(a);
		
		std::stringstream ss;
		ss << mode;
		
		if(aspect.x < 100 && aspect.y < 100) {
			if(aspect == Vec2i(8, 5)) {
				aspect = Vec2i(16, 10);
			}
			ss << " (" << aspect.x << ':' << aspect.y << ')';
		}
		
		pMenuSliderResol->AddText(new TextWidget(-1, hFontMenu, ss.str(), Vec2i(0, 0), NOP));
		
		if(mode.resolution == config.video.resolution) {
			pMenuSliderResol->selectLast();
		}
	}
	
	pMenuSliderResol->AddText(new TextWidget(-1, hFontMenu, AUTO_RESOLUTION_STRING, Vec2i(0, 0), NOP));
	
	if(config.video.resolution == Vec2i_ZERO) {
		pMenuSliderResol->selectLast();
	}

	float fRatio    = (RATIO_X(size.x-9) - pMenuSliderResol->rZone.width()); 

	pMenuSliderResol->Move(Vec2i(checked_range_cast<int>(fRatio), 0));


	pc->AddElement(pMenuSliderResol);

	console->AddMenuCenterY(pc);

	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_detail");
	szMenuText += " ";
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	
	{
	CycleTextWidget * cb = new CycleTextWidget(BUTTON_MENUOPTIONSVIDEO_OTHERSDETAILS, Vec2i(0, 0));
	szMenuText = getLocalised("system_menus_options_video_texture_low");
	cb->AddText(new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP));
	szMenuText = getLocalised("system_menus_options_video_texture_med");
	cb->AddText(new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP));
	szMenuText = getLocalised("system_menus_options_video_texture_high");
	cb->AddText(new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP));
	cb->setValue(config.video.levelOfDetail);
	
	cb->Move(Vec2i(RATIO_X(size.x-9) - cb->rZone.width(), 0));
	pc->AddElement(cb);
	}

	console->AddMenuCenterY(pc);
	
	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_video_brouillard");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONSVIDEO_FOG, Vec2i(RATIO_X(200), 0));
	((SliderWidget *)me)->setValue(config.video.fogDistance);
	pc->AddElement(me);

	console->AddMenuCenterY(pc);
	
	{
	szMenuText = getLocalised("system_menus_options_video_crosshair", "Show Crosshair");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), NOP);
	text->SetCheckOff();
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONSVIDEO_CROSSHAIR;
	cb->iState = config.video.showCrosshair ? 1 : 0;
	console->AddMenuCenterY(cb);
	}
	
	{
	szMenuText = getLocalised("system_menus_options_video_antialiasing", "antialiasing");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
	text->SetCheckOff();
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONSVIDEO_ANTIALIASING;
	cb->iState = config.video.antialiasing ? 1 : 0;
	console->AddMenuCenterY(cb);
	}
	
	ARX_SetAntiAliasing();
	
	{
	szMenuText = getLocalised("system_menus_options_video_vsync", "VSync");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
	text->SetCheckOff();
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONSVIDEO_VSYNC;
	cb->iState = config.video.vsync ? 1 : 0;
	console->AddMenuCenterY(cb);
	}
	
	{
	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_video_apply");
	szMenuText += "   ";
	me = new TextWidget(BUTTON_MENUOPTIONSVIDEO_APPLY, hFontMenu, szMenuText, Vec2i(RATIO_X(240), 0), NOP);
	me->SetPos(Vec2i(RATIO_X(size.x-10)-me->rZone.width(), RATIO_Y(380) + RATIO_Y(40)));
	me->SetCheckOff();
	pc->AddElementNoCenterIn(me);
	pMenuElementApply = me;
	}
	
	{
	pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 420)), pTex);
	cb->iID = BUTTON_MENUOPTIONSVIDEO_BACK;
	cb->eMenuState = OPTIONS;
	cb->SetShortCut(Keyboard::Key_Escape);
	pc->AddElementNoCenterIn(cb);
	}

	console->AddMenu(pc);
}

void MainMenuOptionAudioCreate(CWindowMenuConsole * console, Vec2i size)
{
	std::string szMenuText;
	Widget *me;
	CMenuPanel *pc;
	TextureContainer *pTex;
	
	// Audio backend selection
	{
		
		pc = new CMenuPanel();
		szMenuText = getLocalised("system_menus_options_audio_backend", "Backend");
		szMenuText += "  ";
		me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
		me->SetCheckOff();
		pc->AddElement(me);
		CycleTextWidget * slider = new CycleTextWidget(BUTTON_MENUOPTIONSAUDIO_BACKEND, Vec2i(0, 0));
		
		slider->AddText(new TextWidget(-1, hFontMenu, "Auto-Select", Vec2i(0, 0), OPTIONS_AUDIO_BACKEND_AUTOMATIC));
		slider->selectLast();
#if ARX_HAVE_OPENAL
		slider->AddText(new TextWidget(-1, hFontMenu, "OpenAL", Vec2i(0, 0), OPTIONS_AUDIO_BACKEND_OPENAL));
		if(config.audio.backend == "OpenAL") {
			slider->selectLast();
		}
#endif
		
		float fRatio    = (RATIO_X(size.x-9) - slider->rZone.width()); 
		slider->Move(Vec2i(checked_range_cast<int>(fRatio), 0));
		pc->AddElement(slider);
		console->AddMenuCenterY(pc);
		
	}
	
	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_audio_master_volume");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONSAUDIO_MASTER, Vec2i(RATIO_X(200), 0));
	((SliderWidget *)me)->setValue((int)config.audio.volume); // TODO use float sliders
	pc->AddElement(me);
	console->AddMenuCenterY(pc);

	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_audio_effects_volume");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), OPTIONS_AUDIO);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONSAUDIO_SFX, Vec2i(RATIO_X(200), 0));
	((SliderWidget *)me)->setValue((int)config.audio.sfxVolume);
	pc->AddElement(me);
	console->AddMenuCenterY(pc);

	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_audio_speech_volume");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), OPTIONS_AUDIO);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONSAUDIO_SPEECH, Vec2i(RATIO_X(200), 0));
	((SliderWidget *)me)->setValue((int)config.audio.speechVolume);
	pc->AddElement(me);
	console->AddMenuCenterY(pc);

	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_audio_ambiance_volume");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), OPTIONS_AUDIO);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONSAUDIO_AMBIANCE, Vec2i(RATIO_X(200), 0));
	((SliderWidget *)me)->setValue((int)config.audio.ambianceVolume);
	pc->AddElement(me);
	console->AddMenuCenterY(pc);
	
	{
	szMenuText = getLocalised("system_menus_options_audio_eax", "EAX");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), OPTIONS_INPUT);
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONSAUDIO_EAX;
	cb->iState = config.audio.eax ? 1 : 0;
	console->AddMenuCenterY(cb);
	}
	
	{
	pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->eMenuState = OPTIONS;
	cb->SetShortCut(Keyboard::Key_Escape);
	console->AddMenu(cb);
	}
}

void MainMenuOptionInputCreate(CWindowMenuConsole * console)
{
	std::string szMenuText;
	Widget *me;
	CMenuPanel *pc;
	TextureContainer *pTex;
	
	szMenuText = getLocalised("system_menus_options_input_customize_controls");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), OPTIONS_INPUT_CUSTOMIZE_KEYS_1);
	console->AddMenuCenterY(me);
	
	{
	szMenuText = getLocalised("system_menus_options_input_invert_mouse");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), OPTIONS_INPUT);
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_INVERTMOUSE;
	cb->iState = config.input.invertMouse ? 1 : 0;
	console->AddMenuCenterY(cb);
	}
	
	{
	szMenuText = getLocalised("system_menus_options_auto_ready_weapon");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), OPTIONS_INPUT);
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_AUTOREADYWEAPON;
	cb->iState = config.input.autoReadyWeapon ? 1 : 0;
	console->AddMenuCenterY(cb);
	}

	{
	szMenuText = getLocalised("system_menus_options_input_mouse_look_toggle");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), OPTIONS_INPUT);
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_MOUSELOOK;
	cb->iState = config.input.mouseLookToggle ? 1 : 0;
	console->AddMenuCenterY(cb);
	}

	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_input_mouse_sensitivity");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0.f), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONS_CONTROLS_MOUSESENSITIVITY, Vec2i(RATIO_X(200), 0));
	((SliderWidget*)me)->setValue(config.input.mouseSensitivity);
	pc->AddElement(me);
	console->AddMenuCenterY(pc);
	
	{
	szMenuText = getLocalised("system_menus_autodescription", "auto_description");
	szMenuText += " ";
	TextWidget * text = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), OPTIONS_INPUT);
	CheckboxWidget * cb = new CheckboxWidget(text);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_AUTODESCRIPTION;
	cb->iState = config.input.autoDescription ? 1 : 0;
	console->AddMenuCenterY(cb);
	}

	pc = new CMenuPanel();
	szMenuText = getLocalised("system_menus_options_misc_quicksave_slots", "Quicksave slots");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	me = new SliderWidget(BUTTON_MENUOPTIONS_CONTROLS_QUICKSAVESLOTS, Vec2i(RATIO_X(200), 0));
	((SliderWidget*)me)->setValue(config.misc.quicksaveSlots);
	pc->AddElement(me);
	console->AddMenuCenterY(pc);
	
	{
	pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->eMenuState = OPTIONS;
	cb->SetShortCut(Keyboard::Key_Escape);
	console->AddMenu(cb);
	}
}


void CUSTOM_CTRL_FUNC(CWindowMenuConsole * console, long & y, const std::string & a, int c, int d, const char * defaultText = "?", const char * specialSuffix = ""){
	CMenuPanel * pc = new CMenuPanel();
	
	std::string szMenuText = getLocalised(a, defaultText);
	szMenuText += specialSuffix;
	TextWidget * me = new TextWidget(-1, hFontControls, szMenuText, Vec2i(RATIO_X(20), 0), NOP);
	me->SetCheckOff();
	pc->AddElement(me);
	
	me = new TextWidget(c, hFontControls, "---", Vec2i(RATIO_X(150), 0), NOP);
	me->eState=GETTOUCH;
	pc->AddElement(me);
	
	me = new TextWidget(d, hFontControls, "---", Vec2i(RATIO_X(245), 0), NOP);
	me->eState=GETTOUCH;
	pc->AddElement(me);
	
	pc->Move(Vec2i(0, y));
	console->AddMenu(pc);
	y += pc->rZone.height() + RATIO_Y(3.f);
}

void MainMenuOptionControlsCreatePage1(CWindowMenuConsole * console, Vec2i size) {
	
	long y = static_cast<long>(RATIO_Y(8.f));

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_mouselook", BUTTON_MENUOPTIONS_CONTROLS_CUST_USE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_USE2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_action_combine", BUTTON_MENUOPTIONS_CONTROLS_CUST_ACTIONCOMBINE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_ACTIONCOMBINE2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_jump", BUTTON_MENUOPTIONS_CONTROLS_CUST_JUMP1, BUTTON_MENUOPTIONS_CONTROLS_CUST_JUMP2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_magic_mode", BUTTON_MENUOPTIONS_CONTROLS_CUST_MAGICMODE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_MAGICMODE2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_stealth_mode", BUTTON_MENUOPTIONS_CONTROLS_CUST_STEALTHMODE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_STEALTHMODE2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_walk_forward", BUTTON_MENUOPTIONS_CONTROLS_CUST_WALKFORWARD1, BUTTON_MENUOPTIONS_CONTROLS_CUST_WALKFORWARD2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_walk_backward", BUTTON_MENUOPTIONS_CONTROLS_CUST_WALKBACKWARD1, BUTTON_MENUOPTIONS_CONTROLS_CUST_WALKBACKWARD2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_strafe_left", BUTTON_MENUOPTIONS_CONTROLS_CUST_STRAFELEFT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_STRAFELEFT2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_strafe_right", BUTTON_MENUOPTIONS_CONTROLS_CUST_STRAFERIGHT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_STRAFERIGHT2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_lean_left", BUTTON_MENUOPTIONS_CONTROLS_CUST_LEANLEFT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_LEANLEFT2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_lean_right", BUTTON_MENUOPTIONS_CONTROLS_CUST_LEANRIGHT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_LEANRIGHT2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_crouch", BUTTON_MENUOPTIONS_CONTROLS_CUST_CROUCH1, BUTTON_MENUOPTIONS_CONTROLS_CUST_CROUCH2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_crouch_toggle", BUTTON_MENUOPTIONS_CONTROLS_CUST_CROUCHTOGGLE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_CROUCHTOGGLE2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_strafe", BUTTON_MENUOPTIONS_CONTROLS_CUST_STRAFE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_STRAFE2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_center_view", BUTTON_MENUOPTIONS_CONTROLS_CUST_CENTERVIEW1, BUTTON_MENUOPTIONS_CONTROLS_CUST_CENTERVIEW2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_freelook", BUTTON_MENUOPTIONS_CONTROLS_CUST_FREELOOK1, BUTTON_MENUOPTIONS_CONTROLS_CUST_FREELOOK2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_turn_left", BUTTON_MENUOPTIONS_CONTROLS_CUST_TURNLEFT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_TURNLEFT2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_turn_right", BUTTON_MENUOPTIONS_CONTROLS_CUST_TURNRIGHT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_TURNRIGHT2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_look_up", BUTTON_MENUOPTIONS_CONTROLS_CUST_LOOKUP1, BUTTON_MENUOPTIONS_CONTROLS_CUST_LOOKUP2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_look_down", BUTTON_MENUOPTIONS_CONTROLS_CUST_LOOKDOWN1, BUTTON_MENUOPTIONS_CONTROLS_CUST_LOOKDOWN2);
	
	CMenuPanel * pc = new CMenuPanel();
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_CUST_BACK;
	cb->eMenuState = OPTIONS_INPUT;
	cb->SetShortCut(Keyboard::Key_Escape);
	pc->AddElementNoCenterIn(cb);
	}
	
	{
	std::string szMenuText = getLocalised( "system_menus_options_input_customize_default" );
	TextWidget * me = new TextWidget(BUTTON_MENUOPTIONS_CONTROLS_CUST_DEFAULT, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->SetPos(Vec2i((RATIO_X(size.x) - me->rZone.width())*0.5f, RATIO_Y(380)));
	pc->AddElementNoCenterIn(me);
	}
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/next");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(280, 380)), pTex);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_CUST_BACK;
	cb->eMenuState = OPTIONS_INPUT_CUSTOMIZE_KEYS_2;
	cb->SetShortCut(Keyboard::Key_Escape);
	pc->AddElementNoCenterIn(cb);
	}

	console->AddMenu(pc);
	console->ReInitActionKey();
}

void MainMenuOptionControlsCreatePage2(CWindowMenuConsole * console, Vec2i size) {
	
	long y = static_cast<long>(RATIO_Y(8.f));
	
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_inventory", BUTTON_MENUOPTIONS_CONTROLS_CUST_INVENTORY1, BUTTON_MENUOPTIONS_CONTROLS_CUST_INVENTORY2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_book", BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOK1, BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOK2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_bookcharsheet", BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKCHARSHEET1, BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKCHARSHEET2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_bookmap", BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKMAP1, BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKMAP2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_bookspell", BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKSPELL1, BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKSPELL2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_bookquest", BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKQUEST1, BUTTON_MENUOPTIONS_CONTROLS_CUST_BOOKQUEST2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_drink_potion_life", BUTTON_MENUOPTIONS_CONTROLS_CUST_DRINKPOTIONLIFE1, BUTTON_MENUOPTIONS_CONTROLS_CUST_DRINKPOTIONLIFE2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_drink_potion_mana", BUTTON_MENUOPTIONS_CONTROLS_CUST_DRINKPOTIONMANA1, BUTTON_MENUOPTIONS_CONTROLS_CUST_DRINKPOTIONMANA2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_torch", BUTTON_MENUOPTIONS_CONTROLS_CUST_TORCH1, BUTTON_MENUOPTIONS_CONTROLS_CUST_TORCH2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_cancelcurrentspell", BUTTON_MENUOPTIONS_CONTROLS_CUST_CANCELCURSPELL1, BUTTON_MENUOPTIONS_CONTROLS_CUST_CANCELCURSPELL2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_precast1", BUTTON_MENUOPTIONS_CONTROLS_CUST_PRECAST1, BUTTON_MENUOPTIONS_CONTROLS_CUST_PRECAST1_2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_precast2", BUTTON_MENUOPTIONS_CONTROLS_CUST_PRECAST2, BUTTON_MENUOPTIONS_CONTROLS_CUST_PRECAST2_2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_precast3", BUTTON_MENUOPTIONS_CONTROLS_CUST_PRECAST3, BUTTON_MENUOPTIONS_CONTROLS_CUST_PRECAST3_2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_weapon", BUTTON_MENUOPTIONS_CONTROLS_CUST_WEAPON1, BUTTON_MENUOPTIONS_CONTROLS_CUST_WEAPON2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_unequipweapon", BUTTON_MENUOPTIONS_CONTROLS_CUST_UNEQUIPWEAPON1, BUTTON_MENUOPTIONS_CONTROLS_CUST_UNEQUIPWEAPON2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_previous", BUTTON_MENUOPTIONS_CONTROLS_CUST_PREVIOUS1, BUTTON_MENUOPTIONS_CONTROLS_CUST_PREVIOUS2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_next", BUTTON_MENUOPTIONS_CONTROLS_CUST_NEXT1, BUTTON_MENUOPTIONS_CONTROLS_CUST_NEXT2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_quickload", BUTTON_MENUOPTIONS_CONTROLS_CUST_QUICKLOAD, BUTTON_MENUOPTIONS_CONTROLS_CUST_QUICKLOAD2);
	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_quicksave", BUTTON_MENUOPTIONS_CONTROLS_CUST_QUICKSAVE, BUTTON_MENUOPTIONS_CONTROLS_CUST_QUICKSAVE2);

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_bookmap", BUTTON_MENUOPTIONS_CONTROLS_CUST_MINIMAP1, BUTTON_MENUOPTIONS_CONTROLS_CUST_MINIMAP2, "?", "2");

	CUSTOM_CTRL_FUNC(console, y, "system_menus_options_input_customize_controls_toggle_fullscreen", BUTTON_MENUOPTIONS_CONTROLS_CUST_TOGGLE_FULLSCREEN1, BUTTON_MENUOPTIONS_CONTROLS_CUST_TOGGLE_FULLSCREEN2, "Toggle fullscreen");
	
	CMenuPanel * pc = new CMenuPanel();
	
	{
	TextureContainer * pTex = TextureContainer::Load("graph/interface/menus/back");
	ButtonWidget * cb = new ButtonWidget(RATIO_2(Vec2i(20, 380)), pTex);
	cb->iID = BUTTON_MENUOPTIONS_CONTROLS_CUST_BACK;
	cb->eMenuState = OPTIONS_INPUT_CUSTOMIZE_KEYS_1;
	cb->SetShortCut(Keyboard::Key_Escape);
	pc->AddElementNoCenterIn(cb);
	}
	
	{
	std::string szMenuText = getLocalised( "system_menus_options_input_customize_default" );
	TextWidget * me = new TextWidget(BUTTON_MENUOPTIONS_CONTROLS_CUST_DEFAULT, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->SetPos(Vec2i((RATIO_X(size.x) - me->rZone.width())*0.5f, RATIO_Y(380)));
	pc->AddElementNoCenterIn(me);
	}

	console->AddMenu(pc);
	console->ReInitActionKey();
}

void Menu2_Render_Quit(CWindowMenuConsole * console, Vec2i size)
{
	std::string szMenuText;
	Widget *me = NULL;
	
	szMenuText = getLocalised("system_menus_main_quit");
	me=new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->bCheck = false;
	console->AddMenuCenter(me);
	
	szMenuText = getLocalised("system_menus_main_editquest_confirm");
	me=new TextWidget(-1, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->bCheck = false;
	console->AddMenuCenter(me);
	
	CMenuPanel *pPanel = new CMenuPanel();
	
	szMenuText = getLocalised("system_yes");
	me = new TextWidget(BUTTON_MENUMAIN_QUIT, hFontMenu, szMenuText, Vec2i(0, 0), NOP);
	me->SetPos(Vec2i(RATIO_X(size.x-10)-me->rZone.width(), 0));
	pPanel->AddElementNoCenterIn(me);
	
	szMenuText = getLocalised("system_no");
	me = new TextWidget(-1, hFontMenu, szMenuText, Vec2i(RATIO_X(10), 0), MAIN);
	me->SetShortCut(Keyboard::Key_Escape);
	pPanel->AddElementNoCenterIn(me);
	
	pPanel->Move(Vec2i(0, RATIO_Y(380)));
	console->AddMenu(pPanel);
}

extern CMenuState *mainMenu;

void MainMenuLeftCreate(MENUSTATE eMenuState)
{
	mainMenu->eOldMenuState=eMenuState;
	
	delete pWindowMenu, pWindowMenu = NULL;
	
	Vec2i windowMenuPos = Vec2i(20, 25);
	Vec2i windowMenuSize = Vec2i(321, 430);
	
	pWindowMenu = new CWindowMenu(windowMenuPos, windowMenuSize);
	
	Vec2i offset = Vec2i(0, 14 - 10);
	Vec2i size = windowMenuSize - offset + Vec2i(0, 20);
	
	switch(eMenuState) {
		case NEW_QUEST: {
			if(!ARXMenu_CanResumeGame())
				break;
			
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, NEW_QUEST);
			Menu2_Render_NewQuest(console, size);
			pWindowMenu->AddConsole(console);
			
			pWindowMenu->eCurrentMenuState = NEW_QUEST;
			break;
		}
		case EDIT_QUEST: {
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, EDIT_QUEST);
			MainMenuCreateEditQuest(console);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset + Vec2i(0, -40), size, EDIT_QUEST_LOAD);
			console->lData = -1;
			console->iInterligne = 5;
			
			MainMenuCreateEditQuestLoad(console, size);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset + Vec2i(0, -40), size, EDIT_QUEST_SAVE);
			console->iInterligne = 5;
			
			MainMenuCreateEditQuestSave(console);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, EDIT_QUEST_SAVE_CONFIRM);
			console->lData = -1;
			
			MainMenuCreateEditQuestSaveConfirm(console, size);
			pWindowMenu->AddConsole(console);
			}
			
			pWindowMenu->eCurrentMenuState = EDIT_QUEST;
			break;
		}
		case OPTIONS: {
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, OPTIONS);
			MainMenuOptionGroupsCreate(console);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset + Vec2i(0, -35), size, OPTIONS_VIDEO);
			MainMenuOptionVideoCreate(console, size);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, OPTIONS_AUDIO);
			MainMenuOptionAudioCreate(console, size);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, OPTIONS_INPUT);
			MainMenuOptionInputCreate(console);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, OPTIONS_INPUT_CUSTOMIZE_KEYS_1);
			MainMenuOptionControlsCreatePage1(console, size);
			pWindowMenu->AddConsole(console);
			}
			
			{
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, OPTIONS_INPUT_CUSTOMIZE_KEYS_2);
			MainMenuOptionControlsCreatePage2(console, size);
			pWindowMenu->AddConsole(console);
			}
			
			pWindowMenu->eCurrentMenuState = OPTIONS;
			break;
		}
		case QUIT: {
			CWindowMenuConsole * console = new CWindowMenuConsole(offset, size, QUIT);
			Menu2_Render_Quit(console, size);
			pWindowMenu->AddConsole(console);
			
			pWindowMenu->eCurrentMenuState = QUIT;
			break;
		}
	default: break; // Unhandled menu state.
	}	
}
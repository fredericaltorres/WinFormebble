WinFormebble
------------

# Overview

WinFormebble is a C librarary to program the Pebble smart watch
that is friendly to C# and WinForm developer.

# Api Overview

		void Form_New(Form *form, WindowHandler load, WindowHandler unload);
		void Form_Destructor(Form *form);
		void Form_AddLabel(Form *form, TextLayer * label);
		void Form_RegisterButtonHandlers(Form *form, ClickHandler selectClickHandler, ClickHandler upClickHandler, ClickHandler downClickHandler);

		GFont Font_Load(uint32_t resourceFontId);
		GFont Font_LoadSystem(const char *font_key);

		TextLayer * Label_New(GRect frame, BackGroundColorType backGroundType, GTextAlignment alignment, const char *fontName);
		void Label_SetText(TextLayer * label, const char * text);
		void Label_Destructor(TextLayer * label);
		void Label_SetFont(TextLayer * label, GFont font);
		void Label_SetSystemFont(TextLayer * label, const char *fontName);

		void Timer_Register(TimeUnits tick_units, TickHandler handler);

		MenuLayer * Menu_New(Form *form);
		void Menu_Add(char * entry);
		void Menu_Destructor(Menu menu);

		Trace_TraceError(fmt, args...) 
		Trace_TraceInformation(fmt, args...)
		Trace_TraceWarning(fmt, args...)
		Trace_TraceDebug(fmt, args...)
		Trace_TraceInformationVerbose(fmt, args...)
/* See LICENSE file for copyright and license details. */

/* appearance */
#include <X11/X.h>
#include <X11/Xutil.h>
#include "movestack.c"

#define WINDOW_REMOVE_BORDER 1 /* Remove border around the window */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, "#000000", col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  "#000000"  },
	[SchemeStatus]  = { col_gray3, "#000000",  "#000000"  }, // Statusbar right {text,background,not used but cannot be empty}
	[SchemeTagsSel]  = { col_gray4, "#000000",  "#000000"  }, // Tagbar left selected {text,background,not used but cannot be empty}
	[SchemeTagsNorm]  = { col_gray3, "#000000",  "#000000"  }, // Tagbar left unselected {text,background,not used but cannot be empty}
	[SchemeInfoSel]  = { col_gray4, "#000000",  "#000000"  }, // infobar middle  selected {text,background,not used but cannot be empty}
	[SchemeInfoNorm]  = { col_gray3, "#000000",  "#000000"  }, // infobar middle  unselected {text,background,not used but cannot be empty}
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "chromium",     NULL,       NULL,       0,        0,           -1 },
	{ "zed",          NULL,       NULL,       1 << 1,       0,       -1 }, //tag 2
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "tile",      tile },    /* first entry is default */
	{ "floating",      NULL },    /* no layout function means floating behavior */
	{ "monocle",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/** Commands automatically executed during window manager startup. */
static const char *cmd_nitrogen[] = { "nitrogen", "--restore", NULL };
static const char *cmd_picom[] = { "picom", "--experimental-backends", "--config", "/home/rodrigo/.config/picom/picom.conf", NULL };
static const char *statusbarcmd[] = { "/usr/local/bin/slstatus", NULL };
static const char *nmapplet [] = {"nm-applet",NULL};

static const char **startup_commands[] = {
    cmd_nitrogen,
    cmd_picom,
    statusbarcmd,
    nmapplet,
    NULL
};

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */

//static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };

static const char* rofidrun[] = {"rofi", "-show","drun",NULL};
static const char* rofidwindows[] = {"rofi", "-show","window",NULL};
static const char *lockscreen[] = {"/usr/local/bin/slock" , NULL };
static const char *termcmd[]  = { "/usr/local/bin/st", NULL };
static const char *webcmd[]  = { "chromium", NULL };
static const char *codeeditor[] = {"zed" , NULL};

static const char *upbrightness[]   = { "xbacklight", "-inc", "10", NULL };
static const char *downbrightness[] = { "xbacklight", "-dec", "10", NULL };

static const char *volup[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL };
static const char *voldown[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL };
static const char *volmute[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };

#define XF86XK_Search 0x1008FF1B
#define XF86XK_Lock 225

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ 0,                   XF86XK_Search,      spawn,          {.v = rofidrun } },
	{ MODKEY,                       XK_d,      spawn,          {.v = rofidrun } }, // Show app to open
	{ MODKEY,                       XK_q,      spawn,          {.v = rofidwindows } }, // Show windows already opened
	{ MODKEY,                       XK_w,      spawn,          {.v = webcmd } }, //Opens browser
	{ MODKEY,                       XK_c,      spawn,          {.v = codeeditor } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} }, // Kill focused client
	{ MODKEY|ControlMask,           XK_q,      quit,           {0} }, // Quit Window manager :/
	{ MODKEY,                       XK_b,      togglebar,      {0} }, // Toggle status bar visible or not

    { 0,                            XK_Print,  spawn,          SHCMD("maim | xclip -selection clipboard -t image/png") }, //Print Whole screen
    { ShiftMask,                    XK_Print,  spawn,          SHCMD("maim -s | xclip -selection clipboard -t image/png") }, //Print select rectangle

    { 0,            XF86XK_MonBrightnessUp,    spawn,          {.v = upbrightness } },
    { 0,            XF86XK_MonBrightnessDown,  spawn,          {.v = downbrightness } },

    { 0,               XF86XK_AudioLowerVolume, spawn, {.v = voldown } },
    { 0,               XF86XK_AudioRaiseVolume, spawn, {.v = volup } },
    { 0,               XF86XK_AudioMute,        spawn, {.v = volmute } },

    { 0, XF86XK_Lock, spawn, {.v = lockscreen } },
	{ MODKEY,                       XK_x,      spawn,          {.v = lockscreen } },


    //{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	//{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Right,  movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_Left,   movestack,      {.i = -1 } },
	{ MODKEY,                       XK_Right,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_Left,      focusstack,     {.i = -1 } },

	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_h,      incnmaster,     {.i = -1 } },

	{ MODKEY|ControlMask, XK_Left,  setmfact, {.f = -0.05} },
	{ MODKEY|ControlMask, XK_Right, setmfact, {.f = +0.05} },

	{ MODKEY,             XK_f,      togglefullscr,  {0} },

	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_space,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

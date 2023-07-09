/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */

static const unsigned int gappih    = 15;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 15;       /* vert inner gap between windows */
static const unsigned int gappoh    = 15;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 15;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */

static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
//static const char *fonts[]          = { "monospace:size=10" };
static const char *fonts[]               = { "Sf Pro:size=10", "antialias=true:autohint=true"};
static const char dmenufont[]       = "Sf Pro:size=10";

static char normfgcolor[]                = "#c0d5a0";
static char normbgcolor[]                = "#1f1e23";
static char normbordercolor[]            = "#444444";

static char selfgcolor[]                 = "#1f1e23";
static char selbgcolor[]                 = "#c0d5a0";
static char selbordercolor[]             = "#c0d5a0";

static char tagsnormfgcolor[]            = "#f1c48e";
static char tagsnormbgcolor[]            = "#1f1e23";
//static char tagsnormbordercolor[]        = "#444444";

static char tagsselfgcolor[]             = "#1f1e23";
static char tagsselbgcolor[]             = "#c0d5a0";
//static char tagsselbordercolor[]         = "#c0d5a0";

static const unsigned int baralpha = 251;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
    /*    fg                  bg                  border        */
    { normfgcolor,        normbgcolor,        normbordercolor    },  /* SchemeNorm */
    { selfgcolor,         selbgcolor,         selbordercolor     },  /* SchemeSel  */
    { normfgcolor,        normbgcolor,        "#000000"          },  /* Statusbar */ 
    { tagsselfgcolor,     tagsselbgcolor,     "#000000"          },  /* Tagbar left sel  */
    { tagsnormfgcolor,    tagsnormbgcolor,    "#000000"          },  /* Tagbar left norm */ 
    { selfgcolor,         selbgcolor,         "#000000"          },  /* Infobar middle sel */
    { normfgcolor,        normbgcolor,        "#000000"          },  /* Infobar middle norm*/
};

static const unsigned int alphas[][3]      = {
/*    fg      bg        border */
    { OPAQUE, baralpha, borderalpha }, /* SchemeNorm */
	{ OPAQUE, baralpha, borderalpha }, /* SchemeSel */
};

/* tagging */
static const char *tags[] = { "www", "note", "dev", "code", "chat", "mp3", "var", "etc" };

static const unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask

/*
    Macro with args (KEY, TAG)
    When you call it, it expands to this array of values, 
    everything remains the same except the arguments
*/
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                           KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,               KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,                 KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask,     KEY,      toggletag,      {.ui = 1 << TAG} },


/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */

static const char *dmenucmd[] = { "dmenu_run", "-c", "-l", "10", "-F", "-i", NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *brightUp[]    = { "bash", "/home/kai/.local/bin/bright", "+", NULL };
static const char *brightDown[]  = { "bash", "/home/kai/.local/bin/bright", "-", NULL };
static const char *upVol[]       = { "amixer", "set", "Master", "10+",       NULL };
static const char *downVol[]     = { "amixer", "set", "Master", "10-",       NULL };
static const char *muteVol[]     = { "bash", "/home/kai/.config/scripts/toggleMute.sh",  NULL };
static const char *emoji[]       = { "bash", "/home/kai/.local/bin/dmenuunicode", "i", NULL };
static const char *slock[]       = { "slock", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
    { MODKEY,                       XK_space,  spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	//{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~(unsigned int)0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~(unsigned int)0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_l,                       spawn,     {.v = slock } },
	{ MODKEY,                       XK_grave,      spawn,              {.v = emoji } },
    // Media keys
	{ 0,                            XF86XK_MonBrightnessUp,     spawn,     {.v = brightUp } },
	{ 0,                            XF86XK_MonBrightnessDown,   spawn,     {.v = brightDown } },
	{ 0,                            XF86XK_AudioLowerVolume,    spawn,     {.v = downVol } },
	{ 0,                            XF86XK_AudioRaiseVolume,    spawn,     {.v = upVol } },
	{ 0,                            XF86XK_AudioMute,           spawn,     {.v = muteVol } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	//{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	//{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	//{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	//{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	//{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

{{%AUTOESCAPE context="HTML"}}

<html>
<HEAD>
<META http-equiv="Content-Type" content="text/html; charset=utf-8">
<META http-equiv="Content-Language" content="en">
<META name="description" content="OpenArena stats by oastats">
<LINK rel="stylesheet" href="static/css/oastat.css" type="text/css">
<TITLE>OpenArena Stats</TITLE>
</HEAD>
<BODY>
<DIV id="marco">


<DIV class="cuadropresentacion">
<DIV class="update">Last updated on {{GENERATION_DATE}}</DIV>
<DIV class="intro">OpenArena Stats</DIV>


<TABLE class="tabladatos">
	<TR>
		<TH><DIV class="tituloup">Name</DIV></TH>
		<TH><DIV class="tituloup">Kills</DIV></TH>
		<TH><DIV class="tituloup">Deaths</DIV></TH>
		<TH><DIV class="tituloup">Last seen</DIV></TH>
		<TH><DIV class="tituloup">Is bot</DIV></TH>
        <TH><DIV class="tituloup">Gauntlet</DIV></TH>
        <TH><DIV class="tituloup">Impressive</DIV></TH>
        <TH><DIV class="tituloup">Excellent</DIV></TH>
        <TH><DIV class="tituloup">Capture</DIV></TH>
        <TH><DIV class="tituloup">Defence</DIV></TH>
        <TH><DIV class="tituloup">Assist</DIV></TH>
	</TR>
	{{#PLAYER_LIST}}
	<tr>
	<td><div class="jugador{{EVEN_LINE}}">{{PLAYER_NAME}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{PLAYER_KILLS}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{PLAYER_DEATHS}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{PLAYER_LAST_SEEN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{PLAYER_IS_BOT}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{AWARD_GAUNTLET}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{AWARD_IMPRESSIVE}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{AWARD_EXCELLENT}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{AWARD_CAPTURE}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{AWARD_DEFENCE}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{AWARD_ASSIST}}</div></td>
	<tr>
	{{/PLAYER_LIST}}
</TABLE>

<TABLE class="tabladatos">
	<TR>
        <TH><DIV class="tituloup">Name</DIV></TH>
        <TH><DIV class="tituloup">Shotgun</DIV></TH>
        <TH><DIV class="tituloup">Gauntlet</DIV></TH>
        <TH><DIV class="tituloup">Machinegun</DIV></TH>
        <TH><DIV class="tituloup">Grenade</DIV></TH>
        <TH><DIV class="tituloup">Rocket</DIV></TH>
        <TH><DIV class="tituloup">Plasma</DIV></TH>
        <TH><DIV class="tituloup">Railgun</DIV></TH>
        <TH><DIV class="tituloup">Lightning</DIV></TH>
        <TH><DIV class="tituloup">Nailgun</DIV></TH>
        <TH><DIV class="tituloup">Chaingun</DIV></TH>
        <TH><DIV class="tituloup">BFG</DIV></TH>
        <TH><DIV class="tituloup">Telefrag</DIV></TH>
        <TH><DIV class="tituloup">Falling</DIV></TH>
	</TR>
    {{#PLAYER_WEAPON_LIST}}
	<tr>
	<td><div class="jugador{{EVEN_LINE}}">{{PLAYER_NAME}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_SHOTGUN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_GAUNTLET}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_MACHINEGUN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_GRENADE}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_ROCKET}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_PLASMA}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_RAILGUN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_LIGHTNING}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_NAILGUN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_CHAINGUN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_BFG}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_TELEFRAG}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{WEAPON_FALLING}}</div></td>
	<tr>
	{{/PLAYER_WEAPON_LIST}}
</DIV>
</DIV>
</BODY>

</html>

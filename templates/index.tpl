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
		<TH><DIV class="tituloup"></DIV></TH>
		<TH><DIV class="tituloup">Last seen</DIV></TH>
		<TH><DIV class="tituloup">Is bot</DIV></TH>
	</TR>
	{{#PLAYER_LIST}}
	<tr>
	<td><div class="jugador{{EVEN_LINE}}">{{PLAYER_NAME}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{PLAYER_LAST_SEEN}}</div></td>
	<td><div class="dato{{EVEN_LINE}}">{{PLAYER_IS_BOT}}</div></td>
	<tr>
	{{/PLAYER_LIST}}
</TABLE>

</DIV>
</DIV>
</BODY>

</html>

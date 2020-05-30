#!/usr/bin/perl

print "<center><pre>";
while(<>){
	print "<div align=\"left\">";
	s/Sources:/<h1>Sources:<\/h1>/;
	s/^([A-Z])/<h2>$1<\/h2>/;
	s/\[IMAGE\] ?(.+)/<img src="$1"\/>\n<a href="$1">$1<\/a>/;
	s/^(http.+)/<a href="$1">$1<\/a>/;
	print;
	print "</div>";
}
print "</pre>";

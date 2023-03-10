# $KTH-KRB: roken.awk,v 1.7 2001/03/26 09:26:35 joda Exp $
# $NetBSD: roken.awk,v 1.1.1.3 2002/09/12 12:22:11 joda Exp $

BEGIN {
	print "#include <stdio.h>"
	print "#ifdef HAVE_CONFIG_H"
	print "#include <config.h>"
	print "#endif"
	print ""
	print "int main()"
	print "{"
	    print "puts(\"/* This is an OS dependent, generated file */\");"
	print "puts(\"\\n\");"
	print "puts(\"#ifndef __ROKEN_H__\");"
	print "puts(\"#define __ROKEN_H__\");"
	print "puts(\"\");"
}

$1 == "\#ifdef" || $1 == "\#ifndef" || $1 == "\#if" || $1 == "\#else" || $1 == "\#elif" || $1 == "\#endif" || $1 == "#ifdef" || $1 == "#ifndef" || $1 == "#if" || $1 == "#else" || $1 == "#elif" || $1 == "#endif" {
	print $0;
	next
}

{
	s = ""
	for(i = 1; i <= length; i++){
		x = substr($0, i, 1)
		if(x == "\"" || x == "\\")
			s = s "\\";
		s = s x;
	}
	print "puts(\"" s "\");"
}

END {
	print "puts(\"#define ROKEN_VERSION \" VERSION );"
	print "puts(\"\");"
	print "puts(\"#endif /* __ROKEN_H__ */\");"
	print "return 0;"
	print "}"
}

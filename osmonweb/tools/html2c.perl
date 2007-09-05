#!/usr/bin/perl
#
#=================================================================#
# Project: RTEMS web-based OS browser
#-----------------------------------------------------------------#
# File: html2c.perl
#-----------------------------------------------------------------#
#                    Copyright (c) 2003 IMD                       #
#      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      #
#                       all rights reserved                       #
#-----------------------------------------------------------------#
# this file contains a shell script to translate html files       #
# into C character vectors                                        #
#                                                                 #
#-----------------------------------------------------------------#
#   date                      history                        ID   #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
# 06.07.03  creation                                         doe  #
#*****************************************************************#
#*RCS information:                                                #
#*(the following information is created automatically,            #
#*do not edit here)                                               #
#*****************************************************************#
#* $Log$
#* Revision 1.3  2003/09/26 22:33:16  thomas
#* RTEID tasks and queues work fine
#*
#* Revision 1.2  2003/09/23 17:01:05  thomas
#* first version running with web browser
#*
#* Revision 1.1  2003/09/12 14:31:32  thomas
#* html converter for osmonweb ok
#*
#*
#*****************************************************************#
#=================================================================#


sub usage {
    print("Usage:\n");
    print("$this_script_name [options] [arguments]\n");
    print("Options:\n");
    print("  --verbose={1-5}        set verbosity during operation \n");
    print("  --debug={1-5}          set level of debug output\n");
    print("  --help                 print this help and abort\n");
    print("Mandantory arguments:\n");
    print("  --src-file=<filename>       specify html source file\n");
    print("  --dest-base=<filebasename>  specify base name for destination\n");
    print("   extensions \".c\" and \".h\" will be appended automatically\n");    
}

sub fatal {
    print ("ERROR: $this_script_name aborted: ");
    print (@_);
    print("\n");
    exit 1;
}

sub open_and_read {
    $src_name = shift(@_);
    my $src_file;

    unless(open($src_file,"<$src_name")) {
	fatal ("cannot open source file \"",$src_name, "\"");
    }
    $linnum = 0;
    # 
    # read whole file into buffer
    #
    $curr_read_bytes=1;
    $read_bytes=0;
    $src_string = "";
    while ($curr_read_bytes > 0) {
	$curr_read_bytes = read($src_file,$src_string,4096,$read_bytes);
	$read_bytes = $read_bytes + $curr_read_bytes;
	$linenum++;
    }
#    print $buffer;
    return 1;
}

sub open_dests_and_init {
    my $dst_base = shift(@_);

    unless(open(dst_c_file,">$dst_base" . ".c")) {
	fatal ("cannot open destination file \"",$dst_base,".c","\"");
    }
    unless(open(dst_h_file,">$dst_base" . ".h")) {
	fatal ("cannot open destination file \"",$dst_base,".h","\"");
    }
    #####################
    # init .c file
    #####################
    print dst_c_file "/*\n";
    print dst_c_file " * file: $dst_base.c\n";
    print dst_c_file " * Do not edit here!\n";
#    print dst_c_file " * Created with the following command line:\n";
    print dst_c_file " */\n";
    print dst_c_file " \#include \"$dst_base.h\"\n";

    #####################
    # init .c file
    #####################
    print dst_h_file "/*\n";
    print dst_h_file " * file: $dst_base.h\n";
    print dst_h_file " * Do not edit here!\n";
#    print dst_h_file " * Created with the following command line:\n";
    print dst_h_file " */\n";

    return 1;
}

sub print_string_to_c {
    my $string = shift(@_);
    my $chars_per_line = 50;
    my $chars_in_this_line = 0;
    my $string_idx         = 0;    
    my $string_len         = length($string);
    my @char_list          = split(//,$string);

    while ($string_idx < $string_len) {
	#
	# print new line start, if needed
	# 
	if ($chars_in_this_line == 0) {
	    print dst_c_file "      \"";
	}
	$curr_char = $char_list[$string_idx++];
	if (($curr_char eq "\"") ||
	    ($curr_char eq "\\")) {
	    print dst_c_file "\\$curr_char";
	    $chars_in_this_line = $chars_in_this_line + 2;
	}
	elsif ($curr_char eq "\n") {
	    print dst_c_file "\\n";
	    $chars_in_this_line = $chars_in_this_line + 2;
	}
	else {
	    print dst_c_file $curr_char;
	    $chars_in_this_line++;
	}
	#    
	# print end of line, if needed
        #
	if (($chars_in_this_line >= $chars_per_line) ||
	    ($curr_char eq "\n") ||
	    ($string_idx >= $string_len)) {
	    $chars_in_this_line = 0;
	    print dst_c_file "\"\n";
	}
    }
}

##################################
# start of main script...
#
# create/clear all needed variables
$this_script_name=$0;
$verbose=0;
$debug= 0;
$src_file="";
$dest_base="";
$print_help=0;
#
# get parameter options
# 
use Getopt::Long;
&GetOptions("verbose=i"            => \$verbose,
            "debug=i"              => \$debug,
            "help"                 => \$print_help,
            "src-file=s"           => \$src_file,
            "dest-base=s"          => \$dest_base);
if ($print_help) {
    usage();
    exit 0;
}

#
# check for mandantory arguments
#

if (($src_file      eq "") ||
    ($dest_base     eq "")) {
    fatal ("missing argument. Try option --help");
}

#
# open source file, read it into one big string variable
#
unless(open_and_read($src_file)) {
    fatal ("cannot read source file \"$src_file\"");
}
#
# create destionation .h/.c files, create headers
# 
unless(open_dests_and_init($dest_base)) {
    fatal ("cannot create destination files \"$dst_base.[ch]\"");
}

#
# remove any "replacement" content
#
@split_string = split(/<!--%sample.*?%-->/,$src_string);

$non_sample_string = join('',@split_string);
# 
# split source string for each destination variable
#
#
# find start of pseudocomment: store in variable
#
@split_string = split(/<!-- destvar:/,$non_sample_string,2);

$src_string   = $split_string[1];

while ($src_string ne "") {
    #
    # find variable name
    #
    @split_string = split(/ -->/,$src_string,2);
    $dest_var     = $split_string[0];
    $src_string   = $split_string[1];
    print $dest_var,"\n";    
    #
    # find end of variable contents
    #
    @split_string = split(/<!-- destvar:/,$src_string,2);
    $dest_var_cont= $split_string[0];
    $src_string   = $split_string[1];
    #
    # declare variable in header file
    #
    print dst_h_file "   extern const char $dest_var\[\];\n";
    #
    # define variable in c file
    #
    print dst_c_file "   const char $dest_var\[\] =\n";
    #
    # print content of string variable
    # with escapes to certain characters
    # 
    print_string_to_c($dest_var_cont);

    print dst_c_file "   ;\n\n";
    
}
# 
# scan through source file
# send output to destination files
#

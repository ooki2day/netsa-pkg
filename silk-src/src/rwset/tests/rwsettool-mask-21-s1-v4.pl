#! /usr/bin/perl -w
# MD5: 7227327b3350859c87b021b9b8ebfbc8
# TEST: ./rwsettool --mask=21 ../../tests/set1-v4.set | ./rwsetcat

use strict;
use SiLKTests;

my $rwsettool = check_silk_app('rwsettool');
my $rwsetcat = check_silk_app('rwsetcat');
my %file;
$file{v4set1} = get_data_or_exit77('v4set1');
my $cmd = "$rwsettool --mask=21 $file{v4set1} | $rwsetcat";
my $md5 = "7227327b3350859c87b021b9b8ebfbc8";

check_md5_output($md5, $cmd);

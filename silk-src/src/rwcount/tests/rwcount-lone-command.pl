#! /usr/bin/perl -w
# STATUS: ERR
# TEST: ./rwcount

use strict;
use SiLKTests;

my $rwcount = check_silk_app('rwcount');
check_features(qw(stdin_tty));
my $cmd = "$rwcount";

exit (check_exit_status($cmd) ? 1 : 0);

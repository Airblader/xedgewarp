#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# ISSUE #5
# Check that touching the corner of an output considers both possible directions.
###################################################################################################

# Layout:
# #######
#
# +---+
# | 1 +---+
# +---+ 2 |
#     +---+
#
run_xedgewarp(outputs => [
    '200x200+0+0',
    '200x200+200+100'
]);

warp_pointer(199, 0);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped to the output on the right despite also touching the top border');
is($pointer->{y}, 100, 'pointer is warped to the top of the new output');

exit_xedgewarp;

###################################################################################################

done_testing;

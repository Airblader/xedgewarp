#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# At a corner where two borders are dead, the cursor should warp in whichever direction
# has a neighboring output, even if it is not the primary direction.
#
# Layout:
# #######
#
# +--------+
# |        |
# |   1    +------+
# |        |  2   |
# +--------+------+-------+
# |   3    |      4       |
# +--------+--------------+
###################################################################################################

run_xedgewarp(outputs => [
    '256x144+0+0',
    '192x108+256+36',
    '256x144+0+144',
    '256x144+256+144'
]);

warp_pointer(128, 72);

# Top-right corner of output 4: right border is dead with no neighbor,
# top border is dead but output 2 is a valid top neighbor.
# Expects warp to the bottom-right corner of output 2.
warp_pointer(511, 144);
$pointer = get_pointer;
is($pointer->{x}, 447, 'pointer is warped to the neighboring output (x)');
is($pointer->{y}, 143, 'pointer is warped to the bottom-right of the new output (y)');

exit_xedgewarp;

###################################################################################################

done_testing;

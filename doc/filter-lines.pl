#!/usr/bin/perl

use warnings;
use strict;

my $step = shift @ARGV // 100;
my $head = shift @ARGV // 1000;

while (<>) {
	if (m/^(\d+)\s/ && ($1 < $head || $1 % $step == 0)) {
		print;
	}
}

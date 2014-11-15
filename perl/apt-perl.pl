#!/usr/bin/env perl

# this converts a perl module to a good guess as to what
# it might be named by debian maintainers

use strict;
use warnings;

for my $module (@ARGV) {
    $module =~ s/::/-/g;
    $module = lc($module);
    my $lib_name = "lib$module-perl";
    print $lib_name, "\n";
}

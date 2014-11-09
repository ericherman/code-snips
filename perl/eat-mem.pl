#!/usr/bin/env perl

use strict;
use warnings;
use Proc::ProcessTable;

sub memory_usage {
    my $t = new Proc::ProcessTable;
    foreach my $p ( @{ $t->table } ) {
        if ( $p->pid eq $$ ) {
            return $p->size;
        }
    }
}

my $starting_mem = memory_usage();

sub key_mem_report {
    my ($cnt) = @_;
    my $mem = memory_usage();
    my $mbytes = int( $mem / ( 1024 * 1024 ) );
    my $approx = $cnt ? int( ( $mem - $starting_mem ) / $cnt ) : 0;
    return
        "Memory usage: $mbytes MB."
      . " Object count: $cnt."
      . " Approx bytes per object: $approx";
}

# turn off output buffering
$| = 1;

print "starting memory usage: $starting_mem\n";

{
    my $keys_to_store = scalar @ARGV ? int( $ARGV[0] ) : 2**32;
    my $intstr_to_int = {};
    my $key           = 0;
    my $cnt;
    while ( $key++ < $keys_to_store ) {
        $intstr_to_int->{$key} = 1;
        if ( $key % 16384 == 0 ) {
            $cnt = scalar keys %$intstr_to_int;
            print "\r", key_mem_report($cnt), " ...";
        }
    }
    $cnt = scalar keys %$intstr_to_int;
    print "\r", key_mem_report($cnt), ".\n";
}

print "final memory usage:    ", memory_usage(), "\n";
print "Hit enter to finish ...";
readline(*STDIN);

#!/usr/bin/perl
use strict;
use warnings;
use Data::Dumper;
{ package Data::Dumper; our ( $Indent, $Sortkeys, $Terse, $Useqq ) = (1) x 4 }

use Net::IP;

my $ip0 = Net::IP->new('8.8.8.8');
my $ip1 = Net::IP->new('dead:beef::/32');
my $ip2 = Net::IP->new('127.0.0.1');
my $ip3 = Net::IP->new('::1');
my $ip4 = Net::IP->new('0000:0000:0000:0000:0000:0000:0000:0001');
my $ip5 = Net::IP->new('::192.168.1.2');
my $ip6 = Net::IP->new('::ffff:192.168.1.2');

my $ip7a = Net::IP->new('2001:0db8:0000:0000:0000:ff00:0042:8329');

#removing leading zeroes in each group:
my $ip7b = Net::IP->new('2001:db8:0:0:0:ff00:42:8329');

#omit consecutive sections of zeroes:
my $ip7c = Net::IP->new('2001:db8::ff00:42:8329');

print Dumper(
    {
        ip0     => $ip0->short(),
        ip0hex  => $ip0->hexip(),
        ip1     => $ip1->short(),
        ip1hex  => $ip1->hexip(),
        ip2     => $ip2->short(),
        ip2hex  => $ip2->hexip(),
        ip3     => $ip3->short(),
        ip3hex  => $ip3->hexip(),
        ip4     => $ip4->short(),
        ip4hex  => $ip4->hexip(),
        ip5     => $ip5->short(),
        ip5hex  => $ip5->hexip(),
        ip6     => $ip6->short(),
        ip6hex  => $ip6->hexip(),
        ip7a    => $ip7a->short(),
        ip7ahex => $ip7a->hexip(),
        ip7b    => $ip7b->short(),
        ip7bhex => $ip7b->hexip(),
        ip7c    => $ip7c->short(),
        ip7chex => $ip7c->hexip(),
    }
);

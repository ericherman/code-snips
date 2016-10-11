#!/usr/bin/perl
use strict;
use warnings;
use Data::Dumper;
{ package Data::Dumper; our ( $Indent, $Sortkeys, $Terse, $Useqq ) = (1) x 4 }

use Data::UUID;

my $uuids = Data::UUID->new;

my $uuid1bin = $uuids->create();
my $uuid1str = lc( $uuids->to_string($uuid1bin) );

# NameSpace_DNS
# NameSpace_URL
# NameSpace_OID
# NameSpace_X500
my $namespace = NameSpace_URL;
my $name      = "example.com";
my $uuid2str  = lc( $uuids->create_from_name_str( $namespace, $name ) );
my $uuid2bin  = $uuids->from_string($uuid2str);

my $uuid3bin = $uuids->from_string($uuid1str);
my $uuid3str = lc( $uuids->to_string($uuid3bin) );

my $cmp1v2 = $uuids->compare( $uuid1bin, $uuid2bin );
my $cmp2v1 = $uuids->compare( $uuid2bin, $uuid1bin );
my $cmp1v1 = $uuids->compare( $uuid1bin, $uuid1bin );
my $cmp1v3 = $uuids->compare( $uuid1bin, $uuid3bin );

print Dumper(
    {
        uuids    => $uuids,
        uuid1bin => $uuid1bin,
        uuid1str => $uuid1str,
        uuid2bin => $uuid2bin,
        uuid2str => $uuid2str,
        uuid3bin => $uuid3bin,
        uuid3str => $uuid3str,
        cmp1v2   => $cmp1v2,
        cmp2v1   => $cmp2v1,
        cmp1v1   => $cmp1v1,
        cmp1v3   => $cmp1v3
    }
);

#!/usr/bin/perl

# 2015 Mar 18 - add date of most recent commit
# 2012 Oct 18 - ignore whitespace when finding line owner
# 2012 Mar 05 - add number of commits
# 2010 Oct 13 - sort by most lines, then name
# 2010 Aug 26 - modified to accomidate file moves
# 2010 Aug 03 - script acquired from Peter Brakemeier

use strict;
use warnings;

if ( not scalar @ARGV ) {
	print "specify a file name\n";
	exit 1;
}

foreach my $module (@ARGV) {
	if ( scalar @ARGV > 1 ) {
		print $module, "\n";
	}
	my $qmodule = quotemeta($module);
	my $result  = `git blame -w $qmodule`;

	my %names;
	my $total_lines;

	foreach my $line ( split /\n/, $result ) {
		( my $hash_and_maybe_path, my $name_date_etc ) = split /\(/,
			$line;
		( my $name, my $date ) =
			$name_date_etc =~ m/^(.*?)\s+(\d{4}-\d{2}-\d{2})/si;

		$name ||= "Unknown";
		$names{$name}{lines}++;
		$total_lines++;

		my ($commit) = split / /, $hash_and_maybe_path;
		$names{$name}{commits}{$commit}++;
		if (       ( not $names{$name}{date} )
			or ( $names{$name}{date} lt $date ) )
		{
			$names{$name}{date} = $date;
		}
	}

	my $comparator =
		sub { $names{$b}{lines} <=> $names{$a}{lines} || $a cmp $b };
	foreach my $name ( sort $comparator keys %names ) {
		my $lines     = $names{$name}{lines};
		my $pct_lines = ( $lines / $total_lines ) * 100;
		my $commits   = scalar keys %{ $names{$name}{commits} };
		my $date      = $names{$name}{date};
		print sprintf( "%5.1f %% (%5i lines, %3d commits) %s %s\n",
			$pct_lines, $lines, $commits, $date, $name );
	}
}

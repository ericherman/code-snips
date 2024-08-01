#!/usr/bin/perl
# apply-boiler-plate.pl
# Copyright (C) 2018 Eric Herman <eric@freesa.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
#	https://www.gnu.org/licenses/gpl-3.0.txt

use strict;
use warnings;

sub extract_copyrights {
	my ($file) = @_;
	my $qfile = quotemeta($file);
	my $result =
		`git log --pretty=format:'%ad %aN <%aE>' --date=short $qfile`;

	my $name_count = {};
	my $name_years = {};

	for my $line ( split /\n/, $result ) {
		( my $year, my $who ) =
			$line =~ m/^(\d{4})-\d{2}-\d{2}\s(.*)/si;
		$name_count->{$who}++;
		$name_years->{$who}->{$year}++;
	}
	my $copyright = '';
	for my $who (
		sort { $name_count->{$b} <=> $name_count->{$a} }
		keys %$name_count
		)
	{
		my $years = join ", ", sort keys %{ $name_years->{$who} };
		$copyright .= "   Copyright (C) $years $who\n";
	}
	return $copyright;
}

my $lgpl_2_1_or_later_boiler_plate = <<'BOILERPLATE';
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

	https://www.gnu.org/licenses/lgpl-2.1.txt
BOILERPLATE

my $gpl_2_or_later_boiler_plate = <<'BOILERPLATE';
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/gpl-2.0.txt
BOILERPLATE

my $gpl_3_or_later_boiler_plate = <<'BOILERPLATE';
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/gpl-3.0.txt
BOILERPLATE

my $lgpl_3_or_later_boiler_plate = <<'BOILERPLATE';
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
BOILERPLATE

my $lgpl_2_1_only_boiler_plate = <<'BOILERPLATE';
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation;
   version 2.1 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

	https://www.gnu.org/licenses/lgpl-2.1.txt
BOILERPLATE

my $gpl_2_only_boiler_plate = <<'BOILERPLATE';
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/gpl-2.0.txt
BOILERPLATE

my $gpl_3_only_boiler_plate = <<'BOILERPLATE';
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/gpl-3.0.txt
BOILERPLATE

my $lgpl_3_only_boiler_plate = <<'BOILERPLATE';
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
BOILERPLATE

my $licenses = {
	'LGPLv2.1+'    => $lgpl_2_1_or_later_boiler_plate,
	'LGPLv2.1only' => $lgpl_2_1_only_boiler_plate,
	'GPLv2+'       => $gpl_2_or_later_boiler_plate,
	'GPLv2only'    => $gpl_2_only_boiler_plate,
	'LGPLv3+'      => $lgpl_3_or_later_boiler_plate,
	'LGPLv3only'   => $lgpl_3_only_boiler_plate,
	'GPLv3+'       => $gpl_3_or_later_boiler_plate,
	'GPLv3only'    => $gpl_3_only_boiler_plate,
};

for my $key ( keys %$licenses ) {
	if ( !$licenses->{$key} ) {
		die "whoops! No boilerplate for key '$key'!\n";
	}
}

my @argscopy = @ARGV;
my $license  = shift @argscopy;

my $license_boilerpate = $licenses->{$license};

if ( !$license_boilerpate ) {
	die "could not identify license '$license'\n"
		. "valid options are: '"
		. join( "', '", sort keys %$licenses ) . "'\n";
}

foreach my $file (@argscopy) {
	system "mv -v $file ${file}.orig";
	open( my $fh, '>', $file );
	my $file_basename = `basename $file`;
	chomp $file_basename;
	print $fh "/* $file_basename\n";
	print $fh extract_copyrights($file), "\n";
	print $fh $license_boilerpate;
	print $fh " */\n";
	close $fh;
	system "cat ${file}.orig >> $file";
	system "git diff ${file}";
}

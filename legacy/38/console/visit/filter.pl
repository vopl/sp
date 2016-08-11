use strict;

my $xStart = undef;
my $xLast = undef;
my $cnt=-1;
my $step=1;

my $minPeriod = 1.5*60*60;
my $maxPeriod = 4000*60*60;

my $m_t=[];
my $m_tIdx=0;

#########
open(OUT, ">s.3d") or die $!;
print OUT "time2\tperiod2\treal2\timag2\n";
open(OUTM_T, ">t") or die $!;
open(OUTM_X, ">x") or die $!;
open(OUTM_RE, ">re") or die $!;
open(OUTM_IM, ">im") or die $!;










my $dir = "../out_small_50/spectr/";

opendir(my $D, $dir);
while(my $fn=readdir($D))
{
	next if $fn eq '.' or $fn eq '..';
	warn $fn;
	mkOne($dir.$fn, 4);
}
closedir($D);

foreach(@$m_t)
{
	print OUTM_T $_,"\n";
}
exit;

close OUT;
close OUTM_T;
close OUTM_X;
close OUTM_RE;
close OUTM_IM;


sub mkOne($$)
{
open(INF, "<".shift) or die $!;

#my $key = shift;
my $prev2arr = undef;
while(<INF>)
{
	my $line = $_;
	chomp;
	my $arr = [split /\s/];

	next if $arr->[1] < $minPeriod;
	next if $arr->[1] > $maxPeriod;
	$xStart = $arr->[0] unless defined $xStart;

	if($xLast != $arr->[0])
	{
		print OUTM_X $xLast-$xStart,"\n" if $m_tIdx;
		print OUTM_RE "\n" if $m_tIdx;
		print OUTM_IM "\n" if $m_tIdx;

		$cnt++;
		$xLast = $arr->[0];
		$m_tIdx=0;
		$prev2arr = undef;
	}
	next if $cnt % $step;

	if(!$cnt)
	{
		push @$m_t, $arr->[1];
		$m_tIdx++;
	}
	else
	{
		next if $m_tIdx >= @$m_t;
		$m_tIdx++;
	}


	$arr->[0] -= $xStart;





	my $out = [$arr->[0], $arr->[1], $arr->[2], $arr->[3]];

	print OUT join("\t", @$out);
	print OUT "\n";

	print OUTM_RE $arr->[2],"\t";
	print OUTM_IM $arr->[3],"\t";

}

print "$cnt\n";
close INF;

}
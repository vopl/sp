use strict;
use Data::Dumper;
use Statistics::Basic qw(:all);
use IO::Handle;
 
$|=1;
my $dir = "../out_small_15/response/";

my $minT = 60*20;
my $maxT = 60*60*60000;

my $tArrLen = 10/60; #количество отсчетов к статистике будет <=$t*$tArrLen
my $xLen = 60*60*24*5; #интервал дампировани€ 
my $vals={};# аккумул€тор дл€ статистики
my $lastx=undef;
my $skipAmount=0;

my $ts=[];
my $tsw=0;
sub mkOne($$)
{
	open(INF, "<".shift) or die $!;

	while(<INF>)
	{
		my $line = $_;
		chomp;
		my $arr = [split /\s/];
		
		next if @$arr != 4;

		my $x = $arr->[0];
		my $t = $arr->[1];
		
		next if $t<$minT || $t>$maxT;
		
		my $r = $arr->[2];
		my $i = $arr->[3];
		
		$lastx = $x unless defined $lastx;
		
		if($x - $lastx > $xLen)
		{
			if(--$skipAmount < 0)
			{
				foreach my $k (sort {$a<=>$b;} keys %$vals)
				{
					my @vals = sort {$a<=>$b} @{$vals->{$k}};
					
					my $bnd = int(@vals*0.1) || 1;
					# my $avg = $vals[@vals/2];
					# my $var = $vals[-$bnd]-$vals[$bnd];
					
					@vals = @vals[$bnd..@vals-1-$bnd];
					my $avg = mean(\@vals);
					my $var = stddev($avg->query_vector);
					
					
					if($k*$tArrLen-1 > @{$vals->{$k}})
					{
						$avg=0;
					}
					
					printf(OUT "%.16g\t%.16g\t%.16g\t%.16g\n", $x,$k,$avg,$var);
					
					printf(OUTM_M "%.16g\t", $avg);
					printf(OUTM_V "%.16g\t", $var);
					
					if(0==$tsw)
					{
						push @$ts, $k;
					}
				}
				printf(OUTM_M "\n");
				printf(OUTM_V "\n");

				printf(OUTM_X "%.16g\n", $x);
				
				if(0==$tsw)
				{
					foreach my $t (@$ts)
					{
						printf(OUTM_T "%.16g\n", $t);
					}
					$tsw = 2;
				}
			}			
			$lastx = $x;
		}
		
		my $amp = sqrt($r*$r + $i*$i);
		
		$vals->{$t} = [] unless $vals->{$t};
		
		push(@{$vals->{$t}}, $amp);
		
		shift @{$vals->{$t}} while scalar(@{$vals->{$t}}) > $t*$tArrLen;
	}
	
	close INF;

}















###########################################################
open(OUT, ">s.3d") or die $!; OUT->autoflush(1);
print OUT "x\tt\tm\tv\n";
open(OUTM_T, ">t") or die $!; OUTM_T->autoflush(1);
open(OUTM_X, ">x") or die $!; OUTM_X->autoflush(1);
open(OUTM_M, ">m") or die $!; OUTM_M->autoflush(1);
open(OUTM_V, ">v") or die $!; OUTM_V->autoflush(1);

opendir(my $D, $dir);
while(my $fn=readdir($D))
{
	next if $fn eq '.' or $fn eq '..';
	warn $fn;
	mkOne($dir.$fn, 4);
}
closedir($D);


close OUT;
close OUTM_T;
close OUTM_X;
close OUTM_M;
close OUTM_V;
exit;

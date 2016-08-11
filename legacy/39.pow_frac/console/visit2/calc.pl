
opendir($d, 'P:/finance/spectrumSeparator/nhel.2/39.pow_frac/console/') or die $!;
open $OUT, '>out.3d';
print $OUT "time2\tperiod2\tamp2\tpow2\n";

while(my $d1 =readdir($d))
{
	if($d1 =~ /out_small_(\d+\.\d+)/)
	{
		my $pow = $1;
		next if $pow < 10.00;
		print $pow,"\n";
		
		open($F, '<../'.$d1.'/spectr/000792547200') or die $!;
		
		while(<$F>)
		{
			chomp;
			my @arr = split("\t", $_);
			$arr[2] = sqrt($arr[2]*$arr[2] + $arr[3]*$arr[3]);
			$arr[3] = $pow;
			
			print $OUT join("\t", @arr), "\n";
		}
		close($F);
	}
}
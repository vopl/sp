use Data::Dumper;
open IN, '<'.$ARGV[0] or die $!;

my $str = join("", (<IN>));

$str =~ s/\^2/_p_2/g;
$str =~ s/\%pi/pi/g;
$str =~ s/\r|\n|$//g;

$str =~ s/sin(\([^\(]+\))_p_2/sin_p_2(\1)/g;
$str =~ s/cos(\([^\(]+\))_p_2/cos_p_2(\1)/g;


$str = "//expr\n\n".$str;
my $repls={};

while(1)
{
	my $h={};
	while($str =~ m/((cos|sin|cos_p_2|sin_p_2|)\([^\(]+?\))/g)
	{
		#print 'f:'.$1."\n";
		$h->{$1} = mkName($1);
	}
	
	if(%$h)
	{
		foreach my $k(keys %$h)
		{
			if(!$repls->{$k})
			{
				$str = str_replace($k, $h->{$k}, $str);
				
				$knb = $k;
				$knb =~ s/\r|\n|$//g;
				$knb =~ s/\s|$//g;
				$knb =~ s/\(/ OB /g;
				$knb =~ s/\)/ CB /g;

				$str = str_replace('//expr', "TReal $h->{$k} = $knb;\n//expr", $str);
				$repls->{$k} = $h->{$k};
			}
		}
	}
	else
	{
		last;
	}
	$h = {};
}


$str =~ s/OB/(/g;
$str =~ s/CB/)/g;

		print $str;
#print Dumper($repls);

my $ncnt = 0;
my $names = {};
sub mkName($)
{
	#return "_".$ncnt++;
	my $n = shift;
	
	$n =~ s/\+/_plus_/g;
	$n =~ s/\-/_minus_/g;
	$n =~ s/\*/_mul_/g;
	$n =~ s/\\/_div_/g;
	$n =~ s/\(/_ob_/g;
	$n =~ s/\)/_cb_/g;
	$n =~ s/\s+//g;
	$n =~ s/\r|\n|$//g;
	
	return $names->{$n} if $names->{$n};
	$names->{$n} = "_".$ncnt++;
	return $names->{$n};
}


sub str_replace
{
  my $search = shift;							# what to find
  my $replace = shift;							# what to replace it with
  my $subject = shift;							# the scalar we are operating on
  if (! defined $subject) { return -1; }		# exit if all three required parameters are missing (!)
  my $count = shift;							# number of occurrences to replace
  if (! defined $count) { $count = -1; }		# set $count to -1 (infinite) if undefined

  # start iterating
  my ($i,$pos) = (0,0);
  while ( (my $idx = index( $subject, $search, $pos )) != -1 )	# find next index of $search, starting from our last position
  {
    substr( $subject, $idx, length($search) ) = $replace;		# replace $search with $replace

    $pos=$idx+length($replace);		# jump forward by the length of $replace as it may be
									# longer or shorter than $search was, and if we don't
									# compensate for this we end up in a different portion
									# of the string.

    if ($count>0 && ++$i>=$count) { last; }				# stop iterating if we have reached the limit ($count)
  }

  return $subject;
}
1;
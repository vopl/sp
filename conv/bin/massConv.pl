#!/usr/bin/perl

use strict;
use warnings;

use File::Find;
use File::Spec;
use File::stat;
use File::Basename;
use File::Path qw(make_path);

my $srcDir = '/home/vopl/run/voxforge';
my $dstDir = '/home/vopl/run/voxforge.out';

my $cmdPrefix = '/home/vopl/projects/sp/conv/bin/sp-conv --ppw=1 --splp=100 --efmin=0.2 --efmax=20000 --efcount=1000 --eftype=flog --sfminmult=100 --sfmaxmult=1 --sfcountmult=1 --fps=1000 --inititersmax=1 --itersmax=1';



$| = 1;

local $SIG{INT} = \&signal_handler;
sub signal_handler
{
    kill -9 -$$;
    die "Caught a signal [".shift()."] [$!]";
}

mkdir $dstDir;



sub entry()
{
    return unless -f $File::Find::name && $File::Find::name =~ /.wav$/;

    my $outName = $dstDir.'/'.File::Spec->abs2rel($File::Find::name, $srcDir);
    my $outNameReady = $outName;

    $outName =~ s/.wav$/.spectr/;
    $outNameReady =~ s/.wav$/.ready/;
    
    return if -f $outNameReady;# уже переработан
    
    if(-f $outName)
    {
        my $now = time;
        my $mt = stat($outName)->mtime;
        return if $now - $mt < 1*60; #был затронут менее часа назад, пропустить такой, вдруг с ним кто работает
        
        utime $now, $now, $outName;# не вполне четкая синхронизация, тронуть, секунду подождать и посмотреть
        sleep 1;
        return if stat($outName)->mtime != $now;# кто то другой захватил
    }
    
    make_path(dirname($outName));
    
    my $cmd = "$cmdPrefix --in-file=$File::Find::name --out-file=$outName";
    print "exec $cmd\n";
    
    my $ret = system($cmd);
    exit($ret) if $ret;
    
    open(my $file, '>>', $outNameReady) || die "Couldn't touch $outNameReady: $!";
    close($file);
}

for(;;)
{
    find(\&entry, $srcDir);
    
    print "loop, now some sleep";
    sleep 10;
}

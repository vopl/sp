#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;

use File::Find;
use File::Spec;
use File::stat;
use File::Basename;
use File::Path qw(make_path);

my $srcDir = '/root/share99/voxforge/uncompressed';
my $dstDir = '/root/share99/voxforge.out_10k_10x';

my $cmdPrefix = './sp-conv --ppw=1 --splp=100 --efmin=0.2 --efmax=20000 --efcount=10000 --eftype=flog --sfminmult=100 --sfmaxmult=1 --sfcountmult=10 --fps=1000 --inititersmax=10 --itersmax=1';



$| = 1;

my $stop = 0;
my $workerPid;

local $SIG{TERM} = \&signal_handler;
local $SIG{INT} = \&signal_handler;
sub signal_handler
{
    $stop = 1;
    warn "kill $workerPid";
    kill 'TERM', $workerPid;
    die "Caught a signal [".shift()."] [$!]";
}

mkdir $dstDir;

for(;;)
{
    my @entries;
    find({wanted=>sub {push(@entries, $File::Find::name) if -f $File::Find::name && $File::Find::name =~ /.wav$/;}, no_chdir=>1}, $srcDir);

    for my $entry (sort @entries)
    {
        my $outName = $dstDir.'/'.File::Spec->abs2rel($entry, $srcDir);
        my $outNameReady = $outName;

        $outName =~ s/.wav$/.spectr/;
        $outNameReady =~ s/.wav$/.ready/;

        next if -f $outNameReady;# уже переработан

        make_path(dirname($outName));

        if(-f $outName)
        {
            my $now = time;
            my $mt = stat($outName)->mtime;
            next if $now - $mt < 60*60; #был затронут менее часа назад, пропустить такой, вдруг с ним кто работает

            utime $now, $now, $outName;# не вполне четкая синхронизация, тронуть, секунду подождать и посмотреть
            sleep 1;
            next if stat($outName)->mtime != $now;# кто то другой захватил
        }
        else
        {
            open(my $file, '>>', $outName) || die "Couldn't touch $outName: $!";
            close($file);
        }

        my $cmd = "$cmdPrefix --in-file=$entry --out-file=$outName";
        print "exec $cmd\n";

        $workerPid = fork() or exec($cmd);
        waitpid($workerPid, 0);
        if($?)
        {
            warn "sp-conv failed: $?";
            exit($?);
        }

        open(my $file, '>>', $outNameReady) || die "Couldn't touch $outNameReady: $!";
        close($file);
    }

    print "loop, now some sleep";
    sleep 10;
}

#!/usr/bin/perl
use strict;
use warnings;
use XML::Simple;
use File::Copy;
use File::Path;
use File::Copy::Recursive qw(fcopy rcopy dircopy fmove rmove dirmove);
use File::Basename;
use Cwd 'abs_path';
use Data::Dumper;
use Sys::Hostname;
use Socket;
use Time::HiRes qw(time);
use POSIX qw(strftime);



my $user ="lange";
my $pass ="toto";

my $new_user ="lange";
my $new_pass ="lange";

my $cmd;
my $res;
my $fichier;


### Dockers
$user ="root";
$pass ="root";
my $file ="Dockers";
open($fichier,$file) || die ("Erreur d'ouverture du fichier de fichierAlire") ;
while (<$fichier>)
{
    ### Add user ###
    $_ =~ s/\n//g;
    my @list = split(/\t/, "$_");
    my $p = "\$(perl -e'print crypt(\"". $new_pass . "\", \"aa\")')";
    $cmd = "sshpass -p" . $pass ." ssh -l" . $user . " -p" . $list[2] . " " . $list[1] . " \" useradd -p " . $p . " ". $new_user . " -m \"";
    $res = `$cmd`;
    
    ### scp ssh ###
    $cmd = "sshpass -p" . $new_pass ." scp  -P" . $list[2] . "  -r ssh/ " . $new_user . "@" . $list[1] .":~/.ssh";
    $res = `$cmd`;
    #    print $cmd ."\n";
}

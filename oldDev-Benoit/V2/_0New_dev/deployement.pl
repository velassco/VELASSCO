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


my $t = time;
my $PATH= abs_path();
my $dir = fileparse($PATH);


if (@ARGV eq 0)
{
    die "A configuraiton file is required";
}
### List of variables ###
my $install_path = "/tmp/install";
my $tool_path = "/tmp/tools";

my $user = "lange";
my $path = 0;

my $master = 0;
my $secondery = 0;
my $fs = 0;

my $storage_kind = 0;
my $storage_path = 0;
my $storage_share = 0;
my @storage = ();
my $hbase = 0;
my @hive = ();
my @pig = ();
my @nfs = ();


my @real_nodes = ();
my @vbox_nodes = ();
my @vbox_ip = ();
my @dockers_nodes = ();
my @dockers_nodes2 = ();
my @dockers_ip = ();


my $cpt_VM = 0;
### List of nodes ###





### Start ###
my $parser = XML::Simple->new( KeepRoot => 1 );
my $doc = $parser->XMLin(@ARGV);

### Set Paths ###
Path($doc->{VELaSSCo}->{path});

### Start VMs and containers ###
### Set variables ###
Real_node($doc->{VELaSSCo}->{real});

### Debug ###
sub printt
{
    print "Real \n";
    foreach my $elem (@real_nodes)
    {
        print "\t " . $elem . ", ";
    }
    print "\nvbox \n";
    foreach my $elem (@vbox_nodes)
    {
        print "\t " .  $elem . ", ";
    }
    print "\nDocker \n";
    foreach my $elem (@dockers_nodes)
    {
        print "\t " .  $elem . ", ";
    }
    
    
    print "\nH Master \n";
    print "\t " . $master . "\n";
    
    print "\nH sec \n";
    print "\t " . $secondery . "\n";
    
    
    print "\nStorage \n";
    foreach my $elem (@storage)
    {
        print "\t " .  $elem . ", ";
    }
    print "\nHbase \n";
    
    print "\t " .  $hbase . ", ";
    
    print "\nHive \n";
    foreach my $elem (@hive)
    {
        print "\t " .  $elem . ", ";
    }
    print "\nPig \n";
    foreach my $elem (@pig)
    {
        print "\t " .  $elem . ", ";
    }
    print "\nNFS \n";
    foreach my $elem (@nfs)
    {
        print "\t " .  $elem . ", ";
    }
    print "\n";
}

sub Path
{
    
	my ($var) = @_;
    if ( $var->{install} )
    {
        $install_path = $var->{install};
    }
    if ( $var->{tools} )
    {
        $tool_path =  $var->{tools};
    }
    
}

sub Real_node
{
    my ($var) = @_;
    if ( is_array( $var->{node} ) )
    {
        foreach my $realNode ( @{ $var->{node} } )
        {
            
            push(@real_nodes, $realNode->{host});
            eval_tools_path($realNode);
            eval_install_path($realNode);
            # eval_hadoop($realNode);
            if ( $realNode->{virtual} )
            {
                if ( is_array($realNode->{virtual}->{vbox}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{vbox} } )
                    {
                        push(@vbox_nodes, $virtNode->{host});
                        push(@vbox_ip, $virtNode->{ip});
                        # eval_hadoop($virtNode);
                    }
                    
                }
                elsif ($realNode->{virtual}->{vbox})
                {
                    push(@vbox_nodes, $realNode->{virtual}->{vbox}->{host});
                    push(@vbox_ip, $realNode->{virtual}->{vbox}->{ip});
                    #  eval_hadoop($realNode->{virtual}->{vbox});
                    
                }
                if ( is_array($realNode->{virtual}->{dockers}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{dockers} } )
                    {
                        push(@dockers_nodes, $virtNode->{host});
                        # eval_hadoop($virtNode);
                    }
                    
                }
                elsif($realNode->{virtual}->{dockers})
                {
                    push(@dockers_nodes,  $realNode->{virtual}->{dockers}->{host});
                    #  eval_hadoop($realNode->{virtual}->{dockers});
                }
                
            }
        }
    }
    else
    {
        
        
        push(@real_nodes, $var->{node}->{host});
        eval_tools_path($var->{node});
        eval_install_path($var->{node});
        # eval_hadoop($var->{node});
        if ( $var->{node}->{virtual} )
        {
            if ( is_array($var->{node}->{virtual}->{vbox}) )
            {
                foreach my $virtNode ( @{ $var->{node}->{virtual}->{vbox} } )
                {
                    push(@vbox_nodes, $virtNode->{host});
                    push(@vbox_ip, $virtNode->{ip});
                    #     eval_hadoop($virtNode);
                    
                }
                
            }
            elsif($var->{node}->{virtual}->{vbox})
            {
                push(@vbox_nodes, $var->{node}->{virtual}->{vbox}->{host});
                push(@vbox_ip, $var->{node}->{virtual}->{vbox}->{ip});
                #  eval_hadoop($var->{node}->{virtual}->{vbox});
                
            }
            
            if ( is_array($var->{node}->{virtual}->{dockers}) )
            {
                foreach my $virtNode ( @{ $var->{node}->{virtual}->{dockers} } )
                {
                    push(@dockers_nodes, $virtNode->{host});
                    #  eval_hadoop($virtNode);
                    
                }
                
            }
            elsif ($var->{node}->{virtual}->{dockers})
            {
                push(@dockers_nodes, $var->{node}->{virtual}->{dockers}->{host});
                #  eval_hadoop($var->{node}->{virtual}->{dockers});
                
            }
        }
    }
    
    
    ### Start vms ###
    if ( is_array( $var->{node} ) )
    {
        foreach my $realNode ( @{ $var->{node} } )
        {
            vagrantGenHeader($realNode);
            if ( $realNode->{virtual} )
            {
                if ( is_array($realNode->{virtual}->{vbox}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{vbox} } )
                    {
                        writeVirtualMachine($realNode, $virtNode);
                    }
                    
                }
                elsif($realNode->{virtual}->{vbox})
                {
                    writeVirtualMachine($realNode, $realNode->{virtual}->{vbox});
                }
                if ( is_array($realNode->{virtual}->{dockers}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{dockers} } )
                    {
                        
                        start_dockers($realNode,$virtNode);
                    }
                    
                }
                elsif($realNode->{virtual}->{dockers})
                {
                    start_dockers($realNode,$realNode->{virtual}->{dockers});
                }
            }
            vagrantGenFooter($realNode);
        }
    }
    else
    {
        vagrantGenHeader($var->{node});
        if ( $var->{node}->{virtual} )
        {
            
            if ( is_array($var->{node}->{virtual}->{vbox}) )
            {
                foreach my $virtNode ( @{ $var->{node}->{virtual}->{vbox} } )
                {
                    
                    writeVirtualMachine($var->{node}, $virtNode);
                }
                
            }
            elsif($var->{node}->{virtual}->{vbox})
            {
                writeVirtualMachine($var->{node}, $var->{node}->{virtual}->{vbox});
                
            }
            
            if ( is_array($var->{node}->{virtual}->{dockers}) )
            {
                foreach my $virtNode ( @{ $var->{node}->{virtual}->{dockers} } )
                {
                    start_dockers($var->{node},$virtNode);
                    
                }
                
            }
            elsif($var->{node}->{virtual}->{dockers})
            {
                start_dockers($var->{node},$var->{node}->{virtual}->{dockers});
                
            }
        }
        vagrantGenFooter($var->{node});
    }
    
    
    ### Eval HAdoop -> host to IP ###
    if ( is_array( $var->{node} ) )
    {
        foreach my $realNode ( @{ $var->{node} } )
        {
            eval_hadoop($realNode);
            if ( $realNode->{virtual} )
            {
                if ( is_array($realNode->{virtual}->{vbox}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{vbox} } )
                    {
                        
                        eval_hadoop($virtNode);
                    }
                    
                }
                elsif ($realNode->{virtual}->{vbox})
                {
                    
                    eval_hadoop($realNode->{virtual}->{vbox});
                    
                }
                if ( is_array($realNode->{virtual}->{dockers}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{dockers} } )
                    {
                        
                        eval_hadoop($virtNode);
                    }
                    
                }
                elsif($realNode->{virtual}->{dockers})
                {
                    
                    eval_hadoop($realNode->{virtual}->{dockers});
                }
                
            }
        }
    }
    else
    {
        
        eval_hadoop($var->{node});
        if ( $var->{node}->{virtual} )
        {
            if ( is_array($var->{node}->{virtual}->{vbox}) )
            {
                foreach my $virtNode ( @{ $var->{node}->{virtual}->{vbox} } )
                {
                    
                    eval_hadoop($virtNode);
                    
                }
                
            }
            elsif($var->{node}->{virtual}->{vbox})
            {
                
                eval_hadoop($var->{node}->{virtual}->{vbox});
                
            }
            
            if ( is_array($var->{node}->{virtual}->{dockers}) )
            {
                foreach my $virtNode ( @{ $var->{node}->{virtual}->{dockers} } )
                {
                    eval_hadoop($virtNode);
                    
                }
                
            }
            elsif ($var->{node}->{virtual}->{dockers})
            {
                eval_hadoop($var->{node}->{virtual}->{dockers});
                
            }
        }
    }
    config_hadoop_file();
    
}


sub config_hadoop_file
{
    
    my $cmd ="rm -rf conf; mkdir conf";
    my  $res = `$cmd`;
    $cmd ="cat _conf/hosts > conf/hosts";
    $res = `$cmd`;
    
    $cmd ="cp -R _bin bin";
    $res = `$cmd`;
    $cmd ="cat _conf/capacity-scheduler.xml > conf/capacity-scheduler.xml";
    $res = `$cmd`;
    
    my $i = 0;
    foreach my $val (@real_nodes)
    {
        my($addr)=inet_ntoa((gethostbyname($val))[4]);
        $cmd = "echo " . $addr . " >> conf/listNodes\n";
        $res = `$cmd`;
        $cmd = "echo '" .$addr ."\t" . $val . "' >> conf/hosts\n";
        $res = `$cmd`;
        
    }
    $i = 0;
    foreach my $val(@vbox_nodes)
    {
        $cmd = "echo " . $vbox_ip[$i] . " >> conf/listNodes\n";
        $res = `$cmd`;
        $cmd = "echo '" . $vbox_ip[$i] ."\t" . $val . "' >> conf/hosts\n";
        $res = `$cmd`;
        $i++;
    }
    $i = 0;
    foreach my $val(@dockers_nodes)
    {
        $cmd = "echo " . $dockers_ip[$i] . " >> conf/listNodes\n";
        $res = `$cmd`;
        $cmd = "echo '" .$dockers_ip[$i] ."\t" . $val . "' >> conf/hosts\n";
        $res = `$cmd`;
        $i++;
    }
    
    
    $cmd = "sed  's/MASTERNODE/" . $master . "/g' _conf/core-site.xml > conf/core-site-temp.xml\n";;
    $res = `$cmd`;
    $cmd ="sed  's/MASTERUSER/" . $user . "/g'  conf/core-site-temp.xml > conf/core-site.xml\n";
    $res = `$cmd`;
    $cmd ="sed 's/MASTERNODE/" . $master . "/g' _conf/hdfs-site.xml > conf/hdfs-site-temp.xml\n";
    $res = `$cmd`;
    $cmd ="sed 's|HADOOP_DIR|" . $storage_path . "|g' conf/hdfs-site-temp.xml > conf/hdfs-site.xml\n";
    $res = `$cmd`;
    $cmd ="sed  's|MASTERNODE|" . $master . "|g' _conf/yarn-site.xml > conf/yarn-site.xml\n";
    $res = `$cmd`;
    $cmd = "echo " . $master . " > conf/master\n";
    $res = `$cmd`;
    $cmd = "sed  's/MASTERNODE/" . $master . "/g' _conf/mapred-site.xml > conf/mapred-site.xml\n";;
    $res = `$cmd`;
    
    $cmd ="sed  's|MASTER_HADOOP_LOC|" . $tool_path . "|g' _conf/bashrc > conf/bashrc-tmp\n";
    $res = `$cmd`;
    $cmd ="sed  's|MASTER_HADOOP_CONF|" . $install_path . "|g' conf/bashrc-tmp > conf/bashrc\n";
    $res = `$cmd`;
    
    foreach my $val(@storage)
    {
        $cmd =  "echo " . aIP($val) . " >> conf/slaves\n";
        $res = `$cmd`;
    }
    
    $cmd ="sed  's/MASTERNODE/" . $hbase . "/g' _conf/hbase-site.xml > conf/hbase-site.xml\n";
    $res = `$cmd`;
    
    foreach my $val(@storage)
    {
        $cmd =  "echo " . aIP($val) . " >> conf/regionservers";
        $res = `$cmd`;
    }
    
    $cmd = "sed  's|JAVA_HOME_TO_CHANGE|" . $tool_path . "/java|g' _conf/hadoop-config.sh > conf/hadoop-config.sh\n";;
    $res = `$cmd`;
    $cmd = "sed  's|JAVA_HOME_TO_CHANGE|" . $tool_path . "/java|g' _conf/hadoop-env.sh > conf/hadoop-env.sh\n";;
    $res = `$cmd`;
    
    $cmd = "sed  's|JAVA_HOME_TO_CHANGE|" . $tool_path . "/java|g' _conf/hbase-env.sh > conf/hbase-env.sh\n";;
    $res = `$cmd`;
    
    foreach my $val(@real_nodes)
    {
        $cmd = "scp -r conf\ " . $val . ":" . $install_path;
        $res = `$cmd`;
        $cmd = "scp -r bin\ " . $val . ":" . $install_path;
        $res = `$cmd`;

    }
    
    foreach my $val(@real_nodes)
    {
        $cmd = "scp -r conf/hadoop-env.sh " . $val . ":" . $tool_path . "/hadoop/etc/hadoop";
        $res = `$cmd`;
        $cmd = "scp -r conf/hadoop-config.sh " . $val . ":" . $tool_path . "/hadoop/libexec";
        $res = `$cmd`;
        $cmd = "scp -r conf/hbase-env.sh " . $val . ":" . $tool_path . "/hbase/conf";
        $res = `$cmd`;
    }
}

sub eval_hadoop
{
    my ($node) = @_;
    if ( $node->{hadoop}->{master} )
    {
        if ($master eq 0)
        {
            $master = aIP($node->{host});
            
        }
        else
        {
            
            die "Error: Two masters are specified \n";
        }
    }
    
    if ( $node->{hadoop}->{secondary})
    {
        if ($secondery eq 0)
        {
            
            $secondery = aIP($node->{host});
        }
        else
        {
            die "Error: Two Secondary nodes are specified \n";
        }
        
    }
    
    if ( $node->{hadoop}->{hbase} )
    {
        
        $hbase = aIP($node->{host});
    }
    if ( $node->{hadoop}->{hive} )
    {
        
        push(@hive,aIP($node->{host}));
    }
    if ( $node->{hadoop}->{pig} )
    {
        
        push(@pig,aIP($node->{host}));
    }
    if ( $node->{hadoop}->{NFS} )
    {
        push(@nfs,aIP($node->{host}));
    }
    if ( $node->{hadoop}->{FS} )
    {
        push(@storage,$node->{host});
        if( $node->{hadoop}->{FS}->{kind} )
        {
            if($storage_kind eq 0)
            {
                $storage_kind = $node->{hadoop}->{FS}->{kind};
            }
            else
            {
                die "Error: Two File systems are set! \n";
            }
        }
        if( $node->{hadoop}->{FS}->{path} )
        {
            if($storage_path eq 0)
            {
                $storage_path = $node->{hadoop}->{FS}->{path};
            }
            else
            {
                die "Error: Two File systems are set! \n";
            }
        }
        if( $node->{hadoop}->{FS}->{share} )
        {
            if($storage_share eq 0)
            {
                $storage_share = $node->{hadoop}->{FS}->{share};
            }
            else
            {
                die "Error: Two File systems are set! \n";
            }
        }
        
    }
}
sub eval_install_path
{
    my ($node) = @_;

    my $cmd ="ssh " . $node->{host} ." rm -rf  " . $install_path;
    my $res = `$cmd`;
    
    $cmd ="ssh " . $node->{host} ." mkdir -p  " . $install_path ."/conf";
    $res = `$cmd`;
    
    $cmd ="ssh shark \"cat /etc/hosts >> $install_path/conf/hosts\";";
    $res =`$cmd`;
}
sub eval_tools_path
{
    my ($node) = @_;
    #    print Dumper($node);
    
    my $cmd ="ssh " . $node->{host} ." ls " . $tool_path;
        print $cmd . " \n";
    my $res = `$cmd`;
    
    $cmd ="ssh " . $node->{host} ." mkdir -p " . $tool_path;
        print $cmd . " \n";
     $res = `$cmd`;

    
    if(index($res, "hadoop") == -1)
    {
        print "Copying Hadoop ...(". $node->{host} . ")\n";
        $cmd ="scp -r tools/hadoop " . $node->{host} .":" . $tool_path;
                print $cmd . " \n";
        `$cmd`;
    }
    
    if(index($res, "hbase") == -1)
    {
        print "Copying Hbase ...(". $node->{host} . ")\n";
        $cmd ="scp -r tools/hbase " . $node->{host} .":" . $tool_path;
        print $cmd . " \n";        
        `$cmd`;
    }
    
    if(index($res, "hive") == -1)
    {
        print "Copying Hive ...(". $node->{host} . ")\n";
        $cmd ="scp -r tools/hive " . $node->{host} .":" . $tool_path;
        `$cmd`;
    }
    
    if(index($res, "pig") == -1)
    {
        print "Copying Pig ...(". $node->{host} . ")\n";
        $cmd ="scp -r tools/pig " . $node->{host} .":" . $tool_path;
        `$cmd`;
    }
    
    if(index($res, "jdk") == -1)
    {
        print "Copying JDK ...(". $node->{host} . ")\n";
        $cmd ="scp -r tools/jdk* " . $node->{host} .":" . $tool_path;
        `$cmd`;
    }
    #    if(index($res, "jdk") == -1)
    #{
    #   print "Copying JDK ...(". $node->{host} . ")\n";
    #   $cmd ="ssh " . $node->{host} ." ln -s " . $tool_path . "/jdk* " . $tool_path ."/java";
    #   print $cmd
    #   `$cmd`;
    #}
    
    if(index($res, "vbox") == -1)
    {
        print "Copying vbox cfg files ...(". $node->{host} . ")\n";
        $cmd ="scp -r tools/vbox " . $node->{host} .":" . $tool_path;
        `$cmd`;
    }
}

sub start_vbox
{
    my ($node) = @_;
}

sub start_dockers
{
    my ($node_host,$node) = @_;
    my $cmd = "ssh " . $node_host->{host} . " docker ps";
    my $res = `$cmd`;
    if(index( $res, $node->{host}) != -1)
    {
        die "A Container named: " . $node->{host}  . " already exists\n";
    }
    
    $cmd ="ssh " . $node_host->{host} . " docker run -d -P --name "
    . $node->{host}
    . " -h "
    . $node->{host}
    . " -v "
    . $install_path
    . ":"
    . $install_path
    . " -v "
    . $tool_path
    . ":"
    . $tool_path
    . " -v "
    . $install_path
    . "/conf/hosts:/etc/hosts"
    ." sylvainlasnier/ssh ";
    if (!($storage_share eq 0))
    {
        $cmd .= "-v " . $storage_share . ":" . $storage_share;
    }
    `$cmd`;
    
    $cmd = "ssh " . $node_host->{host} . " \"docker inspect --format '{{ .NetworkSettings.IPAddress }}' " . $node->{host} ."\"";
    $res = `$cmd`;
    $res=~ s/^\s+//;
    $res=~ s/\s+$//;
    push(@dockers_ip, $res);
    
    $cmd = "ssh " . $node_host->{host} . " \"docker inspect --format '{{ .Config.Hostname }}' " . $node->{host} ."\"";
    $res = `$cmd`;
    $res=~ s/^\s+//;
    $res=~ s/\s+$//;
    push(@dockers_nodes2,$res);
}

sub vagrantGenHeader
{
    my ($node) = @_;
    my $cmd ="rm -rf ". $node->{host} ."; mkdir " . $node->{host};
    `$cmd`;
    my $path = $node->{host} ."/Vagrantfile";
    open(  MYFILE, '>', $path );
    print MYFILE "VAGRANTFILE_API_VERSION = \"2\" \n";
    print MYFILE "Vagrant.configure(VAGRANTFILE_API_VERSION) do |config| \n";
    print MYFILE "\t config.vm.box = \"centos7.0\" \n";
    print MYFILE "\t config.vm.synced_folder  \"" . $install_path . "\",\"" . $install_path . "\" \n";
    print MYFILE "\t config.vm.synced_folder  \"" . $tool_path . "\",\"" . $tool_path . "\" \n";
    if (!($storage_share eq 0))
    {
        print MYFILE "\t config.vm.synced_folder = \"" . $storage_share . "\"," . $storage_share . " \n";
    }
    
    close (MYFILE);
}
sub writeVirtualMachine
{
    my ($node, $virtualNode) = @_;
    my $path = $node->{host} ."/Vagrantfile";
    open(  MYFILE, '>>', $path );
    print MYFILE "\t config.vm.define :" . $virtualNode->{host} . " do |" . $virtualNode->{host} . "|\n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.box = \"chef/ubuntu-14.04\" \n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.provider :virtualbox do |vb| \n";
    print MYFILE "\t\t \t vb.customize [\"modifyvm\", :id, \"--memory\", 2048]\n";
    print MYFILE "\t\t \t vb.cpus = 1 \n";
    print MYFILE "\t\t end \n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.hostname = \"" . $virtualNode->{host} . "\"\n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.network \"private_network\", ip: \"" . $virtualNode->{ip} . "\"\n";
    
    
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.provision :shell, :path => \"" . $tool_path . "/vbox/bootstrap.sh\"\n";
    print MYFILE "\t end\n\n";
    close (MYFILE);
}

sub vagrantGenFooter
{
    my ($node) = @_;
    my $path = $node->{host} ."/Vagrantfile";
    open(  MYFILE, '>>', $path );
    print MYFILE "end\n";
    close (MYFILE);
    if(@vbox_ip - $cpt_VM > 0)
    {
        my $cmd = "scp -r " . $node->{host} . "\ " . $node->{host} . ":" . $install_path;
        my $res = `$cmd`;
        
        $cmd ="ssh " . $node->{host} . " \"cd ".  $install_path ."/" . $node->{host} . "; vagrant up\"";
        # print $cmd . "\n";
        `$cmd`;
        $cpt_VM =@vbox_ip + $cpt_VM;
    }
}

########## Utils ##########
sub is_array {
    my ($var) = @_;
    return ( ref($var) eq 'ARRAY' );
}


sub aIP
{
    
    my ($node) = @_;
    my $test = 0;
    my $addr = "0";
    if($test ==0)
    {
        my $i = 0;
        while ($addr eq "0" && $i < @vbox_nodes)
        {
            if($vbox_nodes[$i] eq $node)
            {
                $addr = $vbox_nodes[$i];
            }
            $i++;
        }
        if($addr eq "0")
        {
            $i = 0;
            while ($addr eq "0" && $i < @dockers_nodes)
            {
                if($dockers_nodes[$i] eq $node)
                {
                    $addr = $dockers_nodes2[$i];
                }
                $i++;
            }
            
        }
        if ($addr eq "0")
        {
            $addr = inet_ntoa((gethostbyname($node))[4]);
        }
        
    }
    else
        
    {
        
        my $i = 0;
        while ($addr eq "0" && $i < @vbox_nodes)
        {
            if($vbox_nodes[$i] eq $node)
            {
                $addr = $vbox_ip[$i];
            }
            $i++;
        }
        if($addr eq "0")
        {
            $i = 0;
            while ($addr eq "0" && $i < @dockers_nodes)
            {
                if($dockers_nodes[$i] eq $node)
                {
                    $addr = $dockers_ip[$i];
                }
                $i++;
            }
            
        }
        if ($addr eq "0")
        {
            $addr = inet_ntoa((gethostbyname($node))[4]);
        }
    }
    return $addr;
}

$t = time - $t;
print "Deployement time : " .$t ."\n";
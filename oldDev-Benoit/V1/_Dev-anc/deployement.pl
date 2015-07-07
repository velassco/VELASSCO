#!/usr/bin/perl
use strict;
use warnings;
use XML::Simple;
use File::Copy;
use File::Path;
use File::Copy::Recursive qw(fcopy rcopy dircopy fmove rmove dirmove);
use File::Basename;
use Cwd 'abs_path';

my $PATH= abs_path();
my $dir = fileparse($PATH);

### List of nodes ###
my $master_node = 0;
my $master_node_l = 0;
my $master_node_p = 0;
my $sec_node = 0;
my $hive_node = 0;
my $hbase_node = 0;
my $pig_node = 0;

my $install_path ="~";
my $master_host = 0;


### Start ###
check();
my $parser = XML::Simple->new( KeepRoot => 1 );
my $doc = $parser->XMLin('dep.xml');

if ( is_array( $doc->{real}->{node} ) )
{
    foreach my $realNode ( @{ $doc->{real}->{node} } )
    {
        if ( $realNode->{virtual} )
        {
            vagrantGenHeader($realNode);
            if ( is_array($realNode->{virtual}->{node}) )
            {
                foreach my $virtNode ( @{ $realNode->{virtual}->{node} } )
                {
                    writeVirtualMachine($realNode, $virtNode);
                }
                
            }
            else
            {
                my $virtNode = $realNode->{virtual}->{node};
                writeVirtualMachine($realNode, $virtNode);
            }
            vagrantGenFooter($realNode);
        }
    }
}
else
{
    my $realNode = $doc->{real}->{node};
    if ( $realNode->{virtual} )
    {
        vagrantGenHeader($realNode);
        if ( is_array($realNode->{virtual}->{node}) )
        {
            foreach my $virtNode ( @{ $realNode->{virtual}->{node} } )
            {
                writeVirtualMachine($realNode, $virtNode);
            }
            
        }
        else
        {
            my $virtNode = $realNode->{virtual}->{node};
            writeVirtualMachine($realNode, $virtNode);
        }
        vagrantGenFooter($realNode);
    }
    
}



configNodes();
startVMs();
#sshNodes();
#fsConfigure();
#stopVMs();
cleanRepo();
die "NOw, login to your master node, and launch: install_script.sh, located in tools.\n";

sub fsConfigure
{
    my $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop namenode -format\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"start-all.sh\"'";
    print $cmd ."\n";
    `$cmd`;
    ##Gen
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /user\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /tmp\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -chmod g+w   /tmp\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -chmod g+w   /user\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /user/vagrant\"'";
    print $cmd ."\n";
    `$cmd`;
    
    ##Hbase
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /user/hbase\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /hbase\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -chmod g+w   /user/hbase\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -chmod g+w   /hbase\"'";
    print $cmd ."\n";
    `$cmd`;
    
    ##Hive
    
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /user/hive\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -mkdir       /user/hive/warehouse\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -chmod g+w   /user/hive\"'";
    print $cmd ."\n";
    `$cmd`;
    $cmd ="sshpass -p ".$master_host->{pass}." ssh ". $master_host->{login}."@".$master_host->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"hadoop fs -chmod g+w   /user/hive/warehouse\"'";
    print $cmd ."\n";
    `$cmd`;
}

sub sshNodes
{
    if ( is_array($doc->{real}->{node}) )
    {
        foreach my $realNode ( @{ $doc->{real}->{node} } )
        {
            if ( $realNode->{virtual} )
            {
                if ( is_array( $realNode->{virtual}->{node} ) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{node} } )
                    {
                        my $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"ssh -o StrictHostKeyChecking=no vagrant@" . $virtNode->{host} ." ls\"'";
                        print $cmd . "\n";
                        `$cmd`;
                    }
                    
                }
                else
                {
                    my $virtNode = $realNode->{virtual}->{node} ;
                    my $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"ssh -o StrictHostKeyChecking=no vagrant@" . $virtNode->{host} ." ls\"'";
                    print $cmd . "\n";
                    `$cmd`;
                }
            }
        }
    }
    else
    {
        my $realNode = $doc->{real}->{node} ;
        
        if ( $realNode->{virtual} )
        {
            if ( is_array( $realNode->{virtual}->{node} ) )
            {
                foreach my $virtNode ( @{ $realNode->{virtual}->{node} } )
                {
                    my $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"ssh -o StrictHostKeyChecking=no vagrant@" . $virtNode->{host} ." ls\"'";
                    
                    print $cmd . "\n";
                    `$cmd`;
                }
                
            }
            else
            {
                my $virtNode = $realNode->{virtual}->{node} ;
                my $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." 'sshpass -p " . $master_node_p ." ssh " . $master_node ." -l" . $master_node_l ." \"ssh -o StrictHostKeyChecking=no vagrant@" . $virtNode->{host} ." ls\"'";
                print $cmd . "\n";
                `$cmd`;
            }
        }
    }
}

sub startVMs
{
    
    if ( is_array($doc->{real}->{node}) )
    {
        foreach my $realNode ( @{ $doc->{real}->{node} } )
        {
            if ( $realNode->{path} )
            {
                $install_path = $realNode->{path};
                
            }
            
            print "copying files on " . $realNode->{host} ."\n";
            my $execution = `pwd`;
            my $cmd ="sshpass -p ".$realNode->{pass}." scp -r ". $PATH .  " ".$realNode->{login}."@".$realNode->{host}.":" . $install_path . "/";
            $execution = `$cmd`;
            
            if ( $realNode->{virtual} )
            {
                print "Starting Vms on " . $realNode->{host} ."\n";
                $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." \"cd " . $install_path . "/" . $dir . "; mv " . $realNode->{host} . " Vagrantfile;\"" ;
                $execution = `$cmd`;
                
                $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." \"cd " . $install_path . "/" . $dir . "; vagrant up ;\"" ;
                $execution = `$cmd`;
            }
            if ( $realNode->{path} )
            {
                $install_path = "~";
            }
            
        }
        
    }
    else
    {
        my $realNode = $doc->{real}->{node};
        if ( $realNode->{path} )
        {
            $install_path = $realNode->{path};
        }
        
        print "copying files on " . $realNode->{host} ."\n";
        my $execution = `pwd`;
        my $cmd ="sshpass -p ".$realNode->{pass}." scp -r ". $PATH .  " ".$realNode->{login}."@".$realNode->{host}.":" . $install_path . "/";
        $execution = `$cmd`;
        
        if ( $realNode->{virtual} )
        {
            print "Starting Vms on " . $realNode->{host} ."\n";
            $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." \"cd " . $install_path . "/" . $dir . "; mv " . $realNode->{host} . " Vagrantfile;\"" ;
            $execution = `$cmd`;
            
            $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." \"cd " . $install_path . "/" . $dir . "; vagrant up ;\"" ;
            $execution = `$cmd`;
        }
        if ( $realNode->{path} )
        {
            $install_path = "~";
        }
    }
}

sub stopVMs
{
    if ( is_array($doc->{real}->{node}) )
    {
        foreach my $realNode ( @{ $doc->{real}->{node} } )
        {
            if ( $realNode->{path} )
            {
                $install_path = $realNode->{path};
            }
            if ( $realNode->{virtual} )
            {
                print "Suspend Vms on " . $realNode->{host} ."\n";
                my $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." \"cd " . $install_path . "/" . $dir . "; vagrant suspend ;\"" ;
                my $execution = `$cmd`;
            }
            if ( $realNode->{path} )
            {
                $install_path = "~";
            }
        }
    }
    else
    {
        my $realNode = $doc->{real}->{node};
        if ( $realNode->{path} )
        {
            $install_path = $realNode->{path};
        }
        if ( $realNode->{virtual} )
        {
            print "Suspend Vms on " . $realNode->{host} ."\n";
            my $cmd ="sshpass -p ".$realNode->{pass}." ssh ". $realNode->{login}."@".$realNode->{host}." \"cd " . $install_path . "/" . $dir . "; vagrant suspend ;\"" ;
            my $execution = `$cmd`;
        }
        if ( $realNode->{path} )
        {
            $install_path = "~";
        }
    }
    
}

sub configNodes
{
    ## copy HAdoop configuration ##
    my $cmd =" cp tools/conf/hosts tools/";
    `$cmd`;
    
    open(  MYFILE, '>', "tools/hadoop/etc/hadoop/master" );
    close (MYFILE);
    open(  MYFILE, '>', "tools/hadoop/etc/hadoop/slaves" );
    close (MYFILE);
    
    open(  MYFILE, '>', "tools/listNodes" );
    close (MYFILE);
    
    if ( is_array($doc->{real}->{node}) )
    {
        foreach my $realNode ( @{ $doc->{real}->{node} } )
        {
            if ( $realNode->{virtual} )
            {
                if ( is_array($realNode->{virtual}->{node}) )
                {
                    foreach my $virtNode ( @{ $realNode->{virtual}->{node} } )
                    {
                        writeHost($virtNode);
                        if ( $virtNode->{hadoop} )
                        {
                            MASTER($virtNode,$realNode);
                            SECONDARY($virtNode);
                            HDFS($virtNode);
                            HBase($virtNode);
                            Hive($virtNode);
                            Pig($virtNode);
                        }
                    }
                }
                else
                {
                    my $virtNode = $realNode->{virtual}->{node};
                    writeHost($virtNode);
                    if ( $virtNode->{hadoop} )
                    {
                        MASTER($virtNode,$realNode);
                        SECONDARY($virtNode);
                        HDFS($virtNode);
                        HBase($virtNode);
                        Hive($virtNode);
                        Pig($virtNode);
                    }
                    
                }
            }
            writeHost($realNode);
            if ( $realNode->{hadoop} )
            {
                MASTER($realNode,$realNode);
                SECONDARY($realNode);
                HDFS($realNode);
                HBase($realNode);
                Hive($realNode);
                Pig($realNode);
            }
            
        }
    }
    else
    {
        my $realNode = $doc->{real}->{node};
        if ( $realNode->{virtual} )
        {
            if ( is_array($realNode->{virtual}->{node}) )
            {
                foreach my $virtNode ( @{ $realNode->{virtual}->{node} } )
                {
                    writeHost($virtNode);
                    if ( $virtNode->{hadoop} )
                    {
                        MASTER($virtNode,$realNode);
                        SECONDARY($virtNode);
                        HDFS($virtNode);
                        HBase($virtNode);
                        Hive($virtNode);
                        Pig($virtNode);
                    }
                }
            }
            else
            {
                my $virtNode = $realNode->{virtual}->{node};
                writeHost($virtNode);
                if ( $virtNode->{hadoop} )
                {
                    MASTER($virtNode,$realNode);
                    SECONDARY($virtNode);
                    HDFS($virtNode);
                    HBase($virtNode);
                    Hive($virtNode);
                    Pig($virtNode);
                }
                
            }
        }
        writeHost($realNode);
        if ( $realNode->{hadoop} )
        {
            MASTER($realNode,$realNode);
            SECONDARY($realNode);
            HDFS($realNode);
            HBase($realNode);
            Hive($realNode);
            Pig($realNode);
        }
    }
}

sub writeHost
{
    my ($node) = @_;
    open(  MYFILE, '>>', "tools/hosts" );
    print MYFILE "$node->{ip}\t$node->{host}\n";
    close (MYFILE);
    open(  MYFILE, '>>', "tools/listNodes" );
    print MYFILE "$node->{host}\n";
    close (MYFILE);
    
}

sub HADOOP_LOC
{
    my ($node) = @_;
    if ( $node->{hadoop}->{master} )
    {
        my $temp = $node->{path}."/tools";
        
        my $cmd ="sed  's|MASTER_HADOOP_LOC|" . $temp . "|g' tools/conf/bashrc > tools/bashrc\n";
        #print "#### " . $cmd;
        `$cmd`;
    }
}

sub MASTER
{
    my ($node, $realN) = @_;
    if ( $node->{hadoop}->{master} )
    {
        if ( $master_node != 0)
        {
            die "Master node already set\n";
        }
        $master_host = $realN;
        $master_node = $node->{ip};
        $master_node_l = $node->{login};
        $master_node_p = $node->{pass};
        HADOOP_LOC($node);
        my $cmd ="sed  's/MASTERNODE/" . $node->{host} . "/g' tools/conf/core-site.xml > tools/hadoop/etc/hadoop/core-site-temp.xml\n";
        #print "#### " . $cmd;
        `$cmd`;
        $cmd ="sed  's/MASTERUSER/" . $master_node_l . "/g'  tools/hadoop/etc/hadoop/core-site-temp.xml > tools/hadoop/etc/hadoop/core-site.xml\n";
        #print "#### " . $cmd;
        `$cmd`;
        #`rm -rf tools/hadoop/etc/hadoop/core-site-temp.xml`;
        
        $cmd ="sed 's/MASTERNODE/" . $node->{host} . "/g' tools/conf/hdfs-site.xml > tools/hadoop/etc/hadoop/hdfs-site.xml\n";
        #        print "#### " . $cmd;
        `$cmd`;
        $cmd ="sed  's/MASTERNODE/" . $node->{host} . "/g' tools/conf/yarn-site.xml > tools/hadoop/etc/hadoop/yarn-site.xml\n";
        #        print "#### " . $cmd;
        `$cmd`;
        open(  MYFILE, '>>', "tools/hadoop/etc/hadoop/master" );
        print MYFILE "$node->{host} \n";
        close (MYFILE);
    }
}
sub SECONDARY
{
    my ($node) = @_;
    if ( $node->{hadoop}->{secondary} )
    {
    }
}
sub HDFS
{
    my ($node) = @_;
    if ( $node->{hadoop}->{hdfs} )
    {
        open(  MYFILE, '>>', "tools/hadoop/etc/hadoop/slaves" );
        print MYFILE "$node->{host} \n";
        close (MYFILE);
    }
}
sub HBase
{
    my ($node) = @_;
    if ( $node->{hadoop}->{hbase} )
    {
        if ( $hbase_node != 0)
        {
            die "Hbase node already set\n";
        }
        $hbase_node =  $node->{ip};
        my $cmd ="sed  's/MASTERNODE/" . $node->{host} . "/g' tools/conf/hbase-site.xml > tools/hbase/conf/hbase-site.xml\n";
        `$cmd`;
        $cmd = "cat tools/listNodes > tools/hbase/conf/regionservers";
        `$cmd`;
    }
}
sub Hive
{
    my ($node) = @_;
    if ( $node->{hadoop}->{master} )
    {
        my $temp = $node->{path}."/tools";
        my $cmd ="sed  's|MASTER_HADOOP_LOC|" . $temp . "|g' tools/conf/hive-env.sh > tools/hive/conf/hive-env.sh\n";
        #print "#### " . $cmd;
        `$cmd`;
    }
}

sub Pig
{
    my ($node) = @_;
}


sub check
{
    system("which", "vagrant");
    if ( $? == 256 )
    {
        die "you have to install Vagrant : http://www.vagrantup.com/downloads.html\n";
    }
    
    system("which", "ssh");
    if ( $? == 256 )
    {
        die "you have to install ssh\n";
    }
    system("which", "scp");
    if ( $? == 256 )
    {
        die "you have to install ssh\n";
    }
    
    system("which", "sshpass");
    if ( $? == 256 )
    {
        die "you have to install sshpass\n";
    }
    
}

sub writeVirtualMachine
{
    my ($node, $virtualNode) = @_;
    
    my $path = $node->{host} ;
    open(  MYFILE, '>>', $path );
    print MYFILE "\t config.vm.define :" . $virtualNode->{host} . " do |" . $virtualNode->{host} . "|\n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.box = \"" . $virtualNode->{os} ."\" \n";
    #    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.box_url = \"" . $virtualNode->{os_url} ."\" \n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.provider :virtualbox do |vb| \n";
    print MYFILE "\t\t \t vb.customize [\"modifyvm\", :id, \"--memory\", " . $virtualNode->{memory} . "]\n";
    print MYFILE "\t\t \t vb.cpus = " . $virtualNode->{core} . " \n";
    print MYFILE "\t\t end \n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.hostname = \"" . $virtualNode->{host} . "\"\n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.network \"private_network\", ip: \"" . $virtualNode->{ip} . "\"\n";
    print MYFILE "\t\t " . $virtualNode->{host} . ".vm.provision :shell, :path => \"tools/bootstrap.sh\"\n";
    print MYFILE "\t end\n\n";
    close (MYFILE);
}

sub createDirectory
{
    my ($node) = @_;
    unless(mkdir($node->{host}, 0755))
    {
		printf "\t/!\\ Unable to create". $node->{host}. "\n";
	}
}

############## Vagrant ##############
sub vagrantGenHeader
{
    my ($node) = @_;
    my $path = $node->{host};
    open(  MYFILE, '>', $path );
    print MYFILE "VAGRANTFILE_API_VERSION = \"2\" \n";
    print MYFILE "Vagrant.configure(VAGRANTFILE_API_VERSION) do |config| \n";
    print MYFILE "\t config.vm.box = \"centos7.0\" \n";
    
    close (MYFILE);
}

sub vagrantGenFooter
{
    my ($node) = @_;
    my $path = $node->{host};
    open(  MYFILE, '>>', $path );
    print MYFILE "end\n";
    close (MYFILE);
    
}


sub cleanRepo
{
    my $cmd ="rm tools/hosts";
    `$cmd`;
    if ( is_array($doc->{real}->{node}) )
    {
        foreach my $realNode ( @{ $doc->{real}->{node} } )
        {
            $cmd ="rm " . $realNode->{host};
            `$cmd`;
        }
        
    }
    else
    {
        my $realNode = $doc->{real}->{node};
        {
            $cmd ="rm " . $realNode->{host};
            `$cmd`;
        }
        
    }
    $cmd ="rm tools/hbase/conf/regionservers";
    `$cmd`;
}

sub is_array {
	my ($var) = @_;
	return ( ref($var) eq 'ARRAY' );
}
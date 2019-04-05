#!/usr/bin/perl
use 5.016;
use warnings;
use threads;
use Test::More tests => 85;
use IO::Socket::INET;
use Getopt::Long;

# horrible gut digging to persuade `explain` to print source code
no warnings 'once';
$Data::Dumper::Deparse = 1;

my $server = $ENV{AFINA_SERVER} // "127.0.0.1";
my $port = $ENV{AFINA_PORT} // "8080";
my $silent = 0;

my ($rfifo, $wfifo) = @ENV{"AFINA_RFIFO", "AFINA_WFIFO"};

GetOptions(
	"address=s" => \$server,
	"port=i" => \$port,
	"silent" => \$silent,
	"rfifo=s" => \$rfifo,
	"wfifo=s" => \$wfifo,
) or die "Usage: $0 [-a server] [-p port]\n";

sub make_afina_socket {
	my ($write_command) = @_;
	if (defined $rfifo and defined $wfifo) {
		open my $wf, ">", $rfifo; # afina reads from there; we write
		open my $rf, "<", $wfifo; # afina writes there; we read
		return ($wf, $rf);
	} elsif (defined $rfifo and !defined($wfifo) and $write_command) {
		# this is a FIFO command for write-only Afina
		open my $wf, ">", $rfifo;
		return($wf, 1);
	} else {
		# testing sockets or it's a read command for write-only Afina
		my $socket = IO::Socket::INET::->new(
			PeerAddr => "$server:$port",
			Proto => "tcp"
		);
		return ($socket, $socket);
	}
}

sub afina_request {
	my ($request, $write_cmd) = @_;
	my ($wf, $rf) = make_afina_socket($write_cmd);
	ok($wf, "Connected to Afina for writing");
	ok($rf, "Connected to Afina for reading");
	ok(
		ref $request ? $request->($wf)
			: print($wf $request),
		"Sent request"
	);
	$silent or note ref $request ? explain $request : $request =~ s/^/-> /mrg;
	if ($wf->isa("IO::Socket::INET")) {
		ok(shutdown($wf, SHUT_WR()), "Closed writing end of connection");
	} else {
		ok(close($wf), "Closed writing end of connection");
	}
	my $received;
	if ($write_cmd and defined($rfifo) and !defined($wfifo)) {
		return;
	} elsif ($rf->isa("IO::Socket::INET")) {
		$received .= $_ while (<$rf>); # networked Afina must properly close socket when client is done
	} else {
		sysread($rf, $received, 65536); # do a single read, don't expect Afina to close fd
	}
	$silent or note $received =~ s/^/<- /mrg;
	$received;
}

sub afina_test {
	my ($request, $response, $desc, $wr) = @_;
	my $reply = afina_request($request, $wr);
	if ($wr and defined($rfifo) and !defined($wfifo)) {
		# this Afina only reads FIFO and this is a FIFO command, no reply to check
		pass($desc);
		select(undef, undef, undef, .1); # allow for the command handling to happen
	} else {
		ref $response ? like($reply, $response, $desc)
			: is($reply, $response, $desc);
	}
}

afina_test("set foo 0 0 6\r\nfoobar\r\n", "STORED\r\n", "Set command", 1);

afina_test("get foo\r\n", "VALUE foo 0 6\r\nfoobar\r\nEND\r\n", "Get the value we just set", 0);

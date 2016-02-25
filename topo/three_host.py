"""Custom topology example

Two hosts connect by the same middle host.

   host --- host --- host

Adding the 'topos' dict with a key/value pair to generate our newly defined
topology enables one to pass in '--topo=mytopo' from the command line.
"""

from mininet.topo import Topo

class MyTopo( Topo ):
    "Simple topology example."

    def __init__( self ):
        "Create custom topo."

        # Initialize topology
        Topo.__init__( self )

        # Add hosts and switches
        leftHost = self.addHost( 'h1' )
        rightHost = self.addHost( 'h2' )
        midHost = self.addHost( 'h3' )

        # Add links
        self.addLink( leftHost, midHost )
        self.addLink( midHost, rigntHost )


topos = { 'mytopo': ( lambda: MyTopo() ) }

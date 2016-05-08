/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Network topology
//
//       n0    n1   n2   n3   n4  n5  n6   n7
//       |     |    |    |	  |   |   |    |
//       ===================================
//                   LAN
//
// - UDP flows from n0 to n1 and back
// - DropTail queues 
// - Tracing of queues and packet receptions to file "udp-echo.tr"

#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/rng-seed-manager.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpEchoExample");

int main (int argc, char *argv[])
{
	srand((unsigned int)time(NULL));
 	int random = 1 + rand() %1000 ;
 	RngSeedManager::SetSeed(random);
	/*
	// Users may find it convenient to turn on explicit debugging
	// for selected modules; the below lines suggest how to do this
	*/

	#if 0
		LogComponentEnable ("UdpEchoExample", LOG_LEVEL_INFO);
  		LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
  		LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);
	#endif
	
	// Allow the user to override any of the defaults and the above Bind() at
	// run-time, via command-line arguments
	
  	bool useV6 = false;
  	Address serverAddress,serverAddress1,serverAddress2,serverAddress3,serverAddress4;

  	uint32_t maxPacketCount = 1000;
  	uint32_t packetSize = 4; 
  	Time interPacketInterval = Seconds (0.25);
  	double interval_ip = 0.25;
  	std::string filename = "udp-echonew.tr";

  	CommandLine cmd;
  	cmd.AddValue ("useIpv6", "Use Ipv6", useV6);
  	cmd.AddValue ("MaxPackets","Maximum packets to send" , maxPacketCount);
  	cmd.AddValue ("Interval","Time interval (in sec)" , interval_ip);
  	cmd.AddValue ("PacketSize","Size of packet (in B)",packetSize);
  	cmd.AddValue ("FileName","file name of .tr",filename);
  	cmd.Parse (argc, argv);
  	interPacketInterval = Seconds(interval_ip);
	
	// Explicitly create the nodes required by the topology (shown above).
	
  	NS_LOG_INFO ("Create nodes.");
  	NodeContainer n;
  	n.Create (8);

  	InternetStackHelper internet;
  	internet.Install (n);

  	NS_LOG_INFO ("Create channels.");
	
	// Explicitly create the channels required by the topology (shown above).
	
  	CsmaHelper csma;
  	csma.SetChannelAttribute ("DataRate", StringValue ("1024Kbps"));
  	csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  	csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));
  	NetDeviceContainer d = csma.Install (n);

	// We've got the "hardware" in place.  Now we need to add IP addresses.

  	NS_LOG_INFO ("Assign IP Addresses.");
  	if (useV6 == false){
  		Ipv4AddressHelper ipv4;
      	ipv4.SetBase ("10.1.1.0", "255.255.255.0");
      	Ipv4InterfaceContainer i = ipv4.Assign (d);
      	serverAddress1 = Address(i.GetAddress (4));
       	serverAddress2 = Address(i.GetAddress (5));
        serverAddress3 = Address(i.GetAddress (2));
        serverAddress4 = Address(i.GetAddress (3));
    }
  	else{
  		Ipv6AddressHelper ipv6;
      	ipv6.SetBase ("2001:0000:f00d:cafe::", Ipv6Prefix (64));
      	Ipv6InterfaceContainer i6 = ipv6.Assign (d);
      	serverAddress = Address(i6.GetAddress (1,1));
    }

  	NS_LOG_INFO ("Create Applications.");
	//
	// Create a UdpEchoServer application on node 4,5,2,3.
	//
  
  	//1
  	uint16_t port = 9;  // well-known echo port number
  	UdpEchoServerHelper server1 (port);
  	ApplicationContainer apps1 = server1.Install (n.Get (4));
  	apps1.Start (Seconds (1.0));
  	apps1.Stop (Seconds (10.0));
  	//2
	UdpEchoServerHelper server2 (port);
  	ApplicationContainer apps2 = server2.Install (n.Get (5));
  	apps2.Start (Seconds (1.0));
  	apps2.Stop (Seconds (10.0));
  	//3
  	UdpEchoServerHelper server3 (port);
  	ApplicationContainer apps3 = server3.Install (n.Get (2));
  	apps3.Start (Seconds (1.0));
  	apps3.Stop (Seconds (10.0));
  	//4
  	UdpEchoServerHelper server4 (port);
  	ApplicationContainer apps4 = server4.Install (n.Get (3));
  	apps4.Start (Seconds (1.0));
  	apps4.Stop (Seconds (10.0));

	// Create a UdpEchoClient application to send UDP datagrams from node zero to
	// node one.

 	//1
  	UdpEchoClientHelper client1 (serverAddress1, port);
  	client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  	client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
  	client1.SetAttribute ("PacketSize", UintegerValue (packetSize));
  	apps1 = client1.Install (n.Get (0));
  	apps1.Start (Seconds (2.0));
  	apps1.Stop (Seconds (10.0));
  	//2
   	UdpEchoClientHelper client2 (serverAddress2, port);
  	client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  	client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  	client2.SetAttribute ("PacketSize", UintegerValue (packetSize));
  	apps2 = client2.Install (n.Get (1));
  	apps2.Start (Seconds (2.0));
  	apps2.Stop (Seconds (10.0));
  	//3
   	UdpEchoClientHelper client3 (serverAddress3, port);
  	client3.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  	client3.SetAttribute ("Interval", TimeValue (interPacketInterval));
  	client3.SetAttribute ("PacketSize", UintegerValue (packetSize));
  	apps3 = client3.Install (n.Get (6));
  	apps3.Start (Seconds (2.0));
  	apps3.Stop (Seconds (10.0));
  	//4
   	UdpEchoClientHelper client4 (serverAddress4, port);
  	client4.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  	client4.SetAttribute ("Interval", TimeValue (interPacketInterval));
  	client4.SetAttribute ("PacketSize", UintegerValue (packetSize));
  	apps4 = client4.Install (n.Get (7));
  	apps4.Start (Seconds (2.0));
  	apps4.Stop (Seconds (10.0));

	#if 0
	//
	// Users may find it convenient to initialize echo packets with actual data;
	// the below lines suggest how to do this
	//
  		client.SetFill (apps.Get (0), "Hello World");
  		client.SetFill (apps.Get (0), 0xa5, 1024);
  		uint8_t fill[] = { 0, 1, 2, 3, 4, 5, 6};
  		client.SetFill (apps.Get (0), fill, sizeof(fill), 1024);
	#endif

  	AsciiTraceHelper ascii;
  	csma.EnableAsciiAll (ascii.CreateFileStream (filename));
  	csma.EnablePcapAll ("udp-echo", false);

	/* Now, do the actual simulation. */
  	NS_LOG_INFO ("Run Simulation.");
  	Simulator::Run ();
  	Simulator::Destroy ();
  	NS_LOG_INFO ("Done.");
}

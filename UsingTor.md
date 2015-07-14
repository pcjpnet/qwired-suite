# What is Tor? #
(copied from www.torproject.org)

Tor is free software and an open network that helps you defend against a form of network surveillance that threatens personal freedom and privacy, confidential business activities and relationships, and state security known as traffic analysis.

Tor protects you by bouncing your communications around a distributed network of relays run by volunteers all around the world: it prevents somebody watching your Internet connection from learning what sites you visit, and it prevents the sites you visit from learning your physical location. Tor works with many of your existing applications, including web browsers, instant messaging clients, remote login, and other applications based on the TCP protocol.

Hundreds of thousands of people around the world use Tor for a wide variety of reasons: journalists and bloggers, human rights workers, law enforcement officers, soldiers, corporations, citizens of repressive regimes, and just ordinary citizens. See the Who Uses Tor? page for examples of typical Tor users. See the overview page for a more detailed explanation of what Tor does, and why this diversity of users is important.


# Why would I want to use Qwired Client/Server with Tor? #
Privacy. Tor not only allows you to access any already running Qwired Server anonymously, but it also allows you to set up a hidden server (hidden service) which can not be tracked down to an IP-address or ISP. By allowing Tor-Clients only, the client, nor the server can be tracked down.


# Are there any technical limitations? #
Yes. Network latency and transfer speeds are generally -much- higher. It is generally against the etiquette to transfer big loads of data (such as movies or disk images) using Tor, as it only makes the network even slower.

# How do I set up Qwired Client to use Tor? #
This is rather simple. As Qwired Client now supports connections through proxies, all you have to do is to open the Qwired Preferences dialog and select the following:

  * Proxy type: SOCKS5
  * Proxy server: 127.0.0.1
  * Proxy port: 9050

This is all you need to do to tunnel Qwired-related data through Tor. All you need to do now, is to install a Tor client and start it up. Please see the Tor Project Homepage for download links and instructions.


# How do I set up a hidden Qwired Server? #
Please see http://www.torproject.org/docs/tor-hidden-service.html.en .
If you want to provide a server via Tor only, please consider to block incoming TCP ports on the server machines on port 2000 and 2001.
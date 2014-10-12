require 'socket'
s = UDPSocket.new
s.send("hello!!!", 0, '10.1.1.255', 100)
#gets
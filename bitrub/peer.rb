class Peer
  attr_reader :id
  attr_reader :ip
  attr_reader :port
  def initialize(id, ip, port)
    @id = id
    @ip = ip
    @port = port
  end
end

class LocalPeer < Peer
  def initialize(id, port)
    @id = id
    @port = port
  end
end

def new_peer_id
  chars = ("a" .. "z").to_a + ("A".."Z").to_a + ("0".."9").to_a
  id = ""
  1.upto(20) { |i| id << chars[rand(chars.size - 1)] }
  id
end

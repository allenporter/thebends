require 'uri'
require 'cgi'

class GetResponse
  attr_reader :error_message
  attr_reader :complete
  attr_reader :incomplete
  attr_reader :interval
  attr_reader :min_interval
  attr_reader :peers
  def initialize(x)
    keys = bdecode(x)
    if keys.has_key?("failure reason")
      @error_message = keys["failure reason"]
    else
      @complete = keys["complete"]
      @incomplete = keys["incomplete"]
      @interval = keys["interval"]
      @min_interval = keys["min interval"]
      # TODO: Support original list as well as compact format
      if keys["peers"].class != String
        raise ArgumentError, "Unsupported peers type"
      end
      @peers = peers_from_compact_string(keys["peers"])
    end
  end

  def success?
    @error_message.nil?
  end

  def peers_from_compact_string(peers)
    if peers.length % 6 != 0
      raise ArgumentError, "Invalid peers length"
    end
    p = [ ]
    num_peers = peers.length / 6
    for i in 0 .. (peers.length / 6) - 1
      start = i * 6
      peer = peers[start .. start + 5]
      host = peer[0 .. 3].unpack("C4").join(".")
      port = peer[4 .. 5].unpack("n")[0]
      p.push("#{host}:#{port}")
    end
    p
  end
end

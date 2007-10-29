class HandshakeRequest
  attr_reader :message
  def initialize(metainfo, peer)
    @message = ""
    protocol = "BitTorrent protocol"
    @message << protocol.length
    @message << protocol
    # All integers encoded as 4 byte big endian
    # 8 bytes of zeros (reserved)
    (0 .. 7).each { @message << 0 }
    # 20 byte sha1 hash of the torrent
    info_hash = [metainfo.info_hash].pack("H*")
    if info_hash.length != 20
      raise ArgumentError, "Invalid length of torrent hash"
    end
    @message << info_hash
    if peer.id.length != 20
      raise ArgumentError, "Invalid length of peer id hash"
    end
    @message << peer.id
  end
end

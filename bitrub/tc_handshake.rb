require 'test/unit'
require 'handshake'
require 'metainfo'
require 'peer'

class TC_Bencode < Test::Unit::TestCase
  def test_get_request
    metainfo = MetainfoReader.new("testdata/fonts.torrent").metainfo
    peer_id = new_peer_id()
    peer = LocalPeer.new(peer_id, 6882)
    handshake = HandshakeRequest.new(metainfo, peer)
    assert_equal("\023BitTorrent protocol\000\000\000\000\000\000\000\0009\346\324/\030\263\325_.y\233Wk\v;\357\372N\361\211" << peer_id,
                 handshake.message)
  end
end

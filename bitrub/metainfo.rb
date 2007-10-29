require 'bencode'
require 'digest/sha1'
require 'uri'

class Metainfo
  attr_accessor :announce
  attr_accessor :name
  attr_accessor :info_hash
  attr_accessor :piece_length
  attr_accessor :pieces
  attr_accessor :length
  attr_accessor :files
  attr_accessor :files_length
  def initialize(string)
    h = bdecode(string)
    if h.class != Hash
      raise ArgumentError, "Invalid hash"
    end
    @announce = URI.parse(h["announce"])
    info = h["info"]
    @info_hash = Digest::SHA1.hexdigest(bencode(info))
    @name = info["name"]
    @piece_length = info["piece length"]
    if info.key?("files")
      @files = info["files"]
      @files_length = 0
      @files.each { |file|
        @files_length += file["length"]
      }
    else
      @length = info["length"]
    end
    pieces = info["pieces"]
    if pieces.length % 20 != 0
      raise ArgumentError, "Invalid pieces length"
    end
    @pieces = [ ]
    for i in 0 .. (pieces.length / 20 - 1)
      start = i * 20
      s = pieces[start .. (start + 19)]
      @pieces.push(s.unpack("H*")[0])
    end
  end
end

class MetainfoReader
  attr_reader :metainfo
  def initialize(file)
    s = ""
    File.open(file, "r").each{ |line|
      s += line
    }
    @metainfo = Metainfo.new(s)
  end
end

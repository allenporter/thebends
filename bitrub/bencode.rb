# Author: Allen Porter <allen@thebends.org>
# Based on bencode from BitTorrent

$decode = { }
$encode = { }

def bdecode(x)
  begin
    d = $decode[x[0].chr].new(x, 0)
  rescue
    raise ArgumentError, "not a valid bencoded string"
  end
  if d.num != x.length
    raise ArgumentError, "invalid bencoded value (data after valid prefix)"
  end
  d.result
end

class PartialDecoder
  attr_reader :result
  attr_reader :num
end

class DecodeInt < PartialDecoder
  def initialize(x, f)
    f += 1
    newf = x.index('e', f)
    if newf.nil? or newf == f
      raise ArgumentError
    end
    if x[f].chr == '-'
      if x[f + 1].chr == '0'
        raise ArgumentError
      end
    elsif x[f].chr == '0' and newf != (f + 1)
      raise ArgumentError
    end
    @result = x[f .. newf - 1].to_i
    @num = newf + 1
  end
end

class DecodeString < PartialDecoder
  def initialize(x, f)
    colon = x.index(':', f)
    if colon.nil?
      raise ArgumentError
    end
    n = x[f .. colon].to_i
    if x[f].chr == '0' and colon != f+1
      raise ArgumentError
    end
    colon += 1
    @result = x[colon .. colon + n - 1]
    @num = colon+n
  end
end

class DecodeList < PartialDecoder
  def initialize(x, f)
    r = [ ]
    f = f + 1
    while x[f].chr != 'e'
      decoded = $decode[x[f].chr].new(x, f)
      f = decoded.num
      r.push(decoded.result)
    end
    @result = r
    @num = f + 1
  end
end

class DecodeDict < PartialDecoder
  def initialize(x, f)
    r = { }
    f = f + 1
    while x[f].chr != 'e'
      decoded_key = DecodeString.new(x, f)
      decoded_value = $decode[x[decoded_key.num].chr].new(x, decoded_key.num)
      r[decoded_key.result] = decoded_value.result
      f = decoded_value.num
    end
    @result = r
    @num = f + 1
  end
end

$decode['l'] = DecodeList
$decode['d'] = DecodeDict
$decode['i'] = DecodeInt
$decode['0'] = DecodeString
$decode['1'] = DecodeString
$decode['2'] = DecodeString
$decode['3'] = DecodeString
$decode['4'] = DecodeString
$decode['5'] = DecodeString
$decode['6'] = DecodeString
$decode['7'] = DecodeString
$decode['8'] = DecodeString
$decode['9'] = DecodeString

def bencode(x)
  encoder = $encode[x.class].new(x)
  encoder.result.join
end

class Encoder
  attr_reader :result
end

class EncodeInt < Encoder
  def initialize(x)
    @result = [ "i", x.to_s, "e" ]
  end
end

class EncodeString < Encoder
  def initialize(x)
    @result = [ x.length, ":", x ]
  end
end

class EncodeList < Encoder
  def initialize(x)
    @result = [ "l" ]
    x.each do |item|
      @result.push(bencode(item))
    end
    @result.push("e")
  end
end

class EncodeDict < Encoder
  def initialize(x)
    @result = [ "d" ]
    x.keys.sort.each do |key|
      if key.class != String:
        raise ArgumentError, "Key was not a string"
      end
      @result.push(bencode(key), bencode(x[key]))
    end
    @result.push("e")
  end
end

$encode[Hash] = EncodeDict
$encode[Array] = EncodeList
$encode[Fixnum] = EncodeInt
$encode[Bignum] = EncodeInt
$encode[String] = EncodeString

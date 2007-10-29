// zlib.js
// An implementation of the deflate compression algorithm.  Currently only
// supports using no compression, since it is the minimum amount of work to get
// a png library working.
// TODO: Support real compression and inflate() method
//
// See:
// RFC1950, RFC1951
// http://en.wikipedia.org/wiki/DEFLATE
// http://zlib.net/feldspar.html

// Deflate data (not using compression!)
function deflate(uncompressed, blockSize) {
  var cminfo = parseInt((Math.log(blockSize) / Math.log(2)) - 8);
  if (uncompressed.length > blockSize) {
    throw "Data is larger than the block size";
  }
  var cmf = (cminfo << 4) | 0x8;
  // fdict = 0, flevel = 0
  var flg = 31 - ((cmf * 256 + 0) % 31);

  var data = new Array(7);
  data[0] = cmf;
  data[1] = flg;
  data[2] = 1;  // final
  var blockLength = uncompressed.length;
  var blockLengthComp = (~blockLength & 0xffff);
  data[3] = blockLength & 0xff;
  data[4] = (blockLength & 0xff00) >> 8;
  data[5] = blockLengthComp & 0xff;
  data[6] = (blockLengthComp & 0xff00) >> 8;

  data = data.concat(uncompressed);
  var checksum = new uint32(adler32(uncompressed));
  checksum.appendToArray(data);
  return data;
}

function adler32(buf) {
  return update_adler32(1, buf);
}

function update_adler32(adler, buf) {
  var base = 65521; // largest prime smaller than 65536
  var s1 = adler & 0xffff;
  var s2 = (adler >> 16) & 0xffff;
  var n;
  for (n = 0; n < buf.length; n++) {
    s1 = (s1 + buf[n]) % base;
    s2 = (s2 + s1) % base;
  }
  return ((s2>>>0) << 16) + (s1>>>0) >>> 0;
}

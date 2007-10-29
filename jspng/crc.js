// http://www.libpng.org/pub/png/spec/1.1/PNG-CRCAppendix.html

function CRC() {
  var table = [ ];
  var c;
  var n, k;
  for (n = 0; n < 256; ++n) {
    c = n;
    for (k = 0; k < 8; k++) {
      if (c & 1) {
        c = 0xedb88320 ^ (c >>> 1);
      } else {
        c = c >>> 1;
      }
    }
    table[n] = c;
  }

  function update_crc(buf) {
    var c = -1 >> 0;
    var n;
    for (n = 0; n < buf.length; ++n) {
      var b = buf[n];
      var d = c ^ b;
      var e = d & 0xff;
      var f = c >>> 8;
      c = table[(c ^ buf[n]) & 0xff] ^ (c >>> 8);
    }
    return c;
  }

  this.compute = function(buf) {
    return ((update_crc(buf) >>> 0) ^ (-1 >>> 0)) >>> 0;
  }
}

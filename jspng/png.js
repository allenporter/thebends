// TODO(allen): Support more color types, more bit depths, make it easier for
// callers to simply pass an array of pixels instead of having to know the
// details of the PNG format
//
// PNG Reference:
// http://www.libpng.org/pub/png/spec/1.1/PNG-Contents.html

var crc = new CRC();

// The firefox "btoa()" function is faster than our base64 function, but it
// works on a string.  More of this code should probably be converted to work
// directly with strings or arrays of strings so that we don't have to do this
// conversion all at once.
function bytesToString(bytes) {
  // Lets see it
  var i;
  for (i = 0; i < bytes.length; i++) {
    bytes[i] = String.fromCharCode(bytes[i]);
  }
  return bytes.join("");
}

function Png(pixels) {
  this.width = 0;
  this.height = 0;
  this.bitDepth = 8;  // 8-bit used by tests
  this.colorType = 0x0; // grayscale, no alpha used by tests
  this.pixels = [ ];
  this.getString = function() {
    return bytesToString(this.getBytes());
  }

  this.getBytes = function() {
      var data = [ 137, 80, 78, 71, 13, 10, 26, 10 ];
      IHDR(data, this.width, this.height, this.bitDepth, this.colorType);
      var left = this.pixels.length;
      // TODO(allen): Blocks must occur on scanline boundaries, i think.
      // Multiple blocks do not currently work.
      var blockSize = Math.pow(2, 15); //1024; //256; //512;
      var blockCount = parseInt((left - 1) / blockSize) + 1;
      var i;
      for (i = 0; i < blockCount; i++) {
        var start = i * blockSize;
        var length = blockSize;
        var d = this.pixels.slice(start, length);
        data = data.concat(IDAT(d, blockSize));
      }
      IEND(data);
      return data;
    }
  return this;
}

// Appends the IHDR section to the data array
function IHDR(data, width, height, bitDepth, colorType) {
  var start = data.length;
  var length = 21;
  data[start + length - 1] = 0; // preallocate
  data.splice(start, 4, 0x00, 0x00, 0x00, 0x0d);
  start += 4;
  var crcStart = start;
  var s = "IHDR";
  data.splice(start, 4, s.charCodeAt(0), s.charCodeAt(1),
                        s.charCodeAt(2), s.charCodeAt(3));
  start += 4;
  data.splice(start, 4,
              (width & 0xff000000) >> 24,
              (width & 0xff0000) >> 16,
              (width & 0xff00) >> 8,
              (width & 0xff));
  start += 4;
  data.splice(start, 4,
              (height & 0xff000000) >> 24,
              (height & 0xff0000) >> 16,
              (height & 0xff00) >> 8,
              (height & 0xff));
  start += 4;
  data[start++] = bitDepth;
  data[start++] = colorType;
  data[start++] = 0;  // no compression
  data[start++] = 0;  // standard filters
  data[start++] = 0;  // no interlace
  if (start != data.length) { throw "Internal error"; }
  addCRC(data, crcStart, data.length);
}

// Appends the IEND section to the data array.
function IEND(data) {
  var start = data.length;
  data.splice(start, 4, 0x00, 0x00, 0x00, 0x00);
  var s = "IEND";
  data.splice(start + 4, 4, s.charCodeAt(0), s.charCodeAt(1),
                            s.charCodeAt(2), s.charCodeAt(3));
  addCRC(data, start + 4, start + 8);
}

// Returns an IDAT section for the specified block of pixels
function IDAT(pixels, blockSize) {
  var data = deflate(pixels, blockSize);

  var length = data.length;
  data.splice(0, 0,
              (length & 0xff000000) >> 24,
              (length & 0xff0000) >> 16,
              (length & 0xff00) >> 8,
              (length & 0xff));
  
  var s = "IDAT";
  data.splice(4, 0, s.charCodeAt(0), s.charCodeAt(1),
                    s.charCodeAt(2), s.charCodeAt(3));
  addCRC(data, 4, data.length);
  return data;
} 

// Appends the CRC to the data array.  CRC is computed starting at 'start' for
// len bytes.
function addCRC(data, start, len) {
  var data_crc = crc.compute(data.slice(start, len));
  var c = new uint32(data_crc);
  c.appendToArray(data);
}

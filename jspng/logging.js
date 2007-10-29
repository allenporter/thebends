var logging = {
  text : null,
  init : function() {
    if (logging.text != null) {
      return;  // already initialized
    }
    // TODO: Make controls for hiding/resizing frame
    var frame = document.createElement("iframe");
    frame.setAttribute("align", "right");
    frame.setAttribute("style", "border: solid 1px;");
    document.body.appendChild(frame);
    var doc = frame.contentWindow.document;
    doc.open();
    doc.write("<html><body></body></html>");
    doc.close();
    logging.text = doc.body;
    logging.text.setAttribute("style", "background: lightgray;font-size: 70%");
  },
  info : function(str) {
    this.text.innerHTML += str + "<br>";
  }
}

